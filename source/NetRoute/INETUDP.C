/*****************************************************************************
 NAME:       INETUDP.C

 PURPOSE:    Provide internet routing using the UDP protocol of TCP/IP
             protocol suite.

 SYNOPSIS:   This program runs under RMX and receives messages to be
             routed through the internet in the local Telecredit buffer
             format. The message may be in either the in_msg or out_msg
             buffer but it always arrives at it's destination in the
             in_msg buffer. Internet messages must be preceded with a
             valid internet header as defined in the inet.h include file.
             The internet header contains the name of the host the message
             is to be sent to and a tran code which is used for local
             routing when the message reaches it's destination.

             The name of the host you want to send a message to must
             be present in the TCP/IP Hosts file that is created in
             the etc directory when the Intersoft TCP/IP software
             is installed. In addition, the RMXROUTER UDP service port
             must be present in the services file in the same directory
             ( all hosts must use the same port number ).

             For more information about the socket interface calls used
             in this program, refer to the "Intersoft's TCP/IP for iRMX
             INSTALLATION GUIDE AND PROGRAMMER'S REFERENCE MANUL".

 NOTES:      UDP is a connectionless transport service that is very fast
             but unreliable. There is no guarantee that your messages will
             arrive at their destination or arrive in the same order you sent
             them. When testing this program on an Ethernet LAN it proves
             to be extremely reliable, even with high transaction volumes.
             When designing your application however, you must keep in
             mind the fact that the protocol does not guarantee delivery.
             Messages can go into the bit bucket for the following reasons:

               1. The host you are sending to is recognized on the
                  network but is not able to receive your message for
                  any reason (ie., it is not up and running, etc. ).

               2. Your host generates messages too fast. A program
                  attempting to send messages through the internet at
                  flank speed will lose more than half the datagrams.
                  If you have some valid reason to do this you should delay
                  at least .01 seconds between messages to insure
                  delivery.

             The only failures that can be detected are:

                 1. Network not present (ie., your host is not connected).
                 2. Destination host or destination network does not exist.

 *****************************************************************************/

#include "tcenv.h"
#include <stdlib.h>
#include <stdio.h>
#include <rmxc.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include "so_ic286.h"
#include "tclib.h"
#include "netio.h"
#include "system.h"
#include "inetrout.h"

/* THIS PROGRAM'S LOGICAL DESTINATIONS */
#define DROP_FAILED_MSG_DEST 1
#define FREE_BUFFERS_DEST    2

/* Define local routing information */
#define MYJOBNAME   "INETUDP"

/* Define maximum number of entries in the name cache */
#define MAXNAMECACHE    100

/* Define maximum size of a datagram */
#define MAXDATAGRAM     4096
#define MAXHOSTNAME     4

/* Define size of UDP buffer */
#define UDPBUFSIZE      5120

/* Define Task Stack Sizes */

#define RECV_TASK_STACK        10000

static int data_segment;
static int datagram_msgcnt;
static int myjobnumber;

/* Format of a message within a datagram. */
#pragma noalign(datagram_msg)
struct datagram_msg
  {
   WORD   len;
   struct inethdr hdr;
   char   text[ MAXDATAGRAM - sizeof(WORD) * 2 - sizeof(struct inethdr) ];
  };

/* Global datgram buffer. If config->inet.block_delay is not zero then we block the
   messages in this buffer until we are ready to send */

#pragma noalign(datagram)
static struct datagram
 {
  WORD   len;
  struct datagram_msg msg1;
 } datagram;

/* Sockets */

static int send_socket;
static int recv_socket;
static TOKEN recv_task_tkn;

/* Whoami Information */

static int my_port;

/* Name Cache */

static int nc_count=0;     /* count of names in cache        */
static int nc_i=0;         /* name cache index (next insert) */
static struct
 {
  char      name[MAXHOSTNAME];
  DWORD     addr;
 } nc[MAXNAMECACHE];

/* Statistics buffer */

static struct
 {
  time_t startup;
  int datagrams_sent;
  int datagrams_received;
  int messages_sent;
  int messages_received;
  int invalid_messages_received;
  int send_errors;
  int recv_errors;
 } stats;

static struct inetroute_table *trantbl;
static struct sysconfig *config;

void fail_msg(struct netmsg *msg,int reason)
/*****************************************************************************
 PURPOSE: Handle a message failure by either returning or dropping on the
          floor.
 ARGS:    msgtoken .. token for the message
          reason   .. failure reason code
 RETURNS: nothing
 *****************************************************************************/
{
 struct inethdr *hdr=
   (struct inethdr *)msg->in_msg;

 /* We return the message only if there is an R in the flag of the header */

 if ( hdr->flag == 'R' )
   {
    msg->m_type=MTYPE_RETURN;
    msg->rtn_status=reason;
    hdr->flag='F';
    msg->msg_to=msg->msg_from;
    msg->msg_from=myjobnumber;
    netwrite(msg);
   }
 else
  {
   clogger(W_MSG,__FILE__,__LINE__,
          "FAILED MESSAGE FROM JOB %d DROPPED", msg->msg_from);
   netdest(DROP_FAILED_MSG_DEST,&msg->msg_to,hdr->dcid);
   netwrite(msg);
  }
}

void fail_datagram(int reason)
/*****************************************************************************
 PURPOSE:  Fail all the messages in the datagram buffer.
 ARGS:     none
 RETURNS:  nothing
 *****************************************************************************/
{
 /* This function intentionally left blank. We will throw the datagram
    on the floor until we figure out what to do with it */
}

int destjob(char *trancode)
/*****************************************************************************
 PURPOSE:  Look up a trancode in the trancode table
 ARGS:     trancode .. the 2 character tran code to look-up
 RETURNS:  destination job number
 *****************************************************************************/
{
 register int i;
 WORD wtrancode;

 if ( trancode[0]=='*' && trancode[1]=='*' ) return(myjobnumber);

 memcpy(&wtrancode,trancode,sizeof(WORD));

 for (i=0;i<config->inet.tbl.count;++i)
  if ( wtrancode == trantbl[i].trancode )
    return( trantbl[i].destjob );

 return(config->inet.default_dest);
}

DWORD lookup_address(char *name)
/*****************************************************************************
 PURPOSE: Look up address for a host name using global cache.
 ARGS:    name .. the name to look up
 RETURNS: the address or 0 if not found
 *****************************************************************************/
{
 register i;
 struct hostent *host;
 DWORD addr;
 char   hname[MAXHOSTNAME+1];

 for (i=0;i<nc_count;++i)
  if ( memicmp(name,nc[i].name,MAXHOSTNAME)==0 )
    return(nc[i].addr);

 memcpy(hname,name,MAXHOSTNAME);
 hname[sizeof(hname)-1]='\0';
 host=gethostbyname(hname);
 if ( host==NULL )
  {
   clogger(W_MSG,__FILE__,__LINE__,"Host %s not found",hname);
   return(0);
  }

 bcopy( host->h_addr, (char *)&addr, sizeof(addr));
 nc[ nc_i ].addr = addr;
 memcpy(nc[ nc_i ].name,host->h_name,MAXHOSTNAME);
 if ( ++nc_i >= MAXNAMECACHE ) nc_i=0;
 if ( nc_count < MAXNAMECACHE ) ++nc_count;
 return( addr );
}

void far recv_task(void)
/*****************************************************************************
 PURPOSE: Task to receive messages from recv_socket and route locally.
 ARGS:    none
 RETURNS: Nothing. Runs as task handling datagram input from socket.
 *****************************************************************************/
{
 struct datagram dgram;
 int    status;
 int    bread;
 int    fromlen;
 struct sockaddr_in fromaddr;
 struct datagram_msg *dmsg;
 struct netmsg *msg;

 /* DELAY UNTIL JOBS OF LEVEL 1 OR GREATER ARE READY */
 netwait(1);

 while(1)
  {
   fromlen=sizeof(fromaddr);
   if ( (bread=recvfrom(recv_socket,(char *)&dgram,sizeof(dgram),0,
                    (struct sockaddr *)&fromaddr,&fromlen,&status)) == -1 )
    {
     clogger(E_MSG,__FILE__,__LINE__,
            "RECV RETURNED %d (%s)", status, socstrerror(status));
     ++stats.recv_errors;
     continue;
    }

   /* Ignore messages with incorrect length */
   if ( bread != dgram.len+sizeof(dgram.len) )
    {
     ++stats.invalid_messages_received;
     continue;
    }

   ++stats.datagrams_received;

   /* De-multiplex the message */
   dmsg=&dgram.msg1;
   while( dgram.len )
    {
     msg=initmsg(destjob(dmsg->hdr.trancode),NULL);
     msg->msg_len=dmsg->len;
     memcpy(msg->in_msg,&dmsg->hdr,msg->msg_len);
     if ( dmsg->hdr.flag == '*' ) 
       netreply(msg);
     else
       netwrite(msg);
     dgram.len -= (dmsg->len + sizeof(dmsg->len));
     dmsg=(struct datagram_msg *) &((char *)&dmsg->hdr)[ dmsg->len ];
     ++stats.messages_received;
    }
  }

}

void stopjob(void)
/*****************************************************************************
 PURPOSE:  Dump the statistics from the global statistics buffer to a
           file called UDPSTATS in the current directory.
 ARGS:     none
 RETURNS:  nothing
 *****************************************************************************/
{
 FILE *f;
 time_t now;
 struct tm *tblock;
 int status;
 WORD wstatus;

 rqdeletetask(recv_task_tkn,&wstatus);
 soclose(send_socket,&status);
 soclose(recv_socket,&status);

 f=fopen("UDPSTATS","wt");
 if ( f==NULL )
  {
   clogger(E_MSG,__FILE__,__LINE__,"ERROR OPENING UDPSTATS FILE");
   return;
  }

 time(&now);
 tblock = localtime(&stats.startup);
 fprintf(f,"RMXROUTER UDP STATISTICS\nSTART = %s", asctime(tblock) );
 tblock = localtime(&now);
 fprintf(f,"STOP  = %s\n", asctime(tblock) );
 fprintf(f,"DATAGRAMS SENT ................ %d\n",stats.datagrams_sent);
 fprintf(f,"DATAGRAMS RECEIVED ............ %d\n",stats.datagrams_received);
 fprintf(f,"MESSAGES SENT ................. %d\n",stats.messages_sent);
 fprintf(f,"MESSAGES RECEIVED ............. %d\n",stats.messages_received);
 fprintf(f,"INVALID MESSAGES RECEIVED ..... %d\n",stats.invalid_messages_received);
 fprintf(f,"SEND ERRORS ................... %d\n",stats.send_errors);
 fprintf(f,"RECV ERRORS ................... %d\n",stats.recv_errors);

 fclose(f);
}


int application_init(void)
/*****************************************************************************
 PURPOSE: Perform various initialization functions for this program.
 ARGS:    none
 RETURNS: 1 .. okay, 0 .. failed
 *****************************************************************************/
{
 WORD status;
 int  istatus;
 int  bufsize;
 int  so;
 struct servent *srv;
 struct sockaddr_in srv_addr;

 /* GET THE SYSTEM CONFIGURATION */
 config=getsysconfig();
 trantbl=buildptr(config->inet.tbl.tkn,0);

  /* Make sure TCP/IP is running on this host */

 rqlookupobject(rqgettasktokens(3,&status),"\006TCPJOB",100,&status);
 if ( status  )
  {
   clogger(E_MSG,__FILE__,__LINE__,"STARTUP FAILED: TCPJOB NOT RUNNING");
   return(0);
  }

 /* Get the port I am supposed to service */

 struct servent *srv;
 srv=getservbyname("rmxrouter","udp");
 if ( srv==NULL )
  {
   clogger(E_MSG,__FILE__,__LINE__,
         "INITIALIZATION FAILED: 'RMXROUTER' NOT FOUND IN SERVICES FILE");
   return(0);
  }

 my_port=srv->s_port;

 /* Close any sockets already bound to the rmxouter UDP port */

 bufsize=sizeof(srv_addr);
 for (so=0;so<31;++so)
   if ( getsockname(so,(struct sockaddr *)&srv_addr,&bufsize,&istatus) == 0 &&
        srv_addr.sin_port == my_port )
      shutdown( so, 2, &istatus );

 /* Create send and receive sockets */

 if ( (send_socket=socket(AF_INET,SOCK_DGRAM,0,&istatus)) == -1 ||
      (recv_socket=socket(AF_INET,SOCK_DGRAM,0,&istatus)) == -1 )
  {
   clogger(E_MSG,__FILE__,__LINE__,
         "SOCKET CREATION RETURNED %d (%s)",istatus,socstrerror(istatus));
   return(0);
  }

 /* Set the size of the send and receive buffers */

 bufsize=UDPBUFSIZE;
 setsockopt(send_socket,SOL_SOCKET,SO_SNDBUF,(char *)&bufsize,
            sizeof(bufsize),&istatus);
 if ( ! istatus )
  setsockopt(recv_socket,SOL_SOCKET,SO_RCVBUF,(char *)&bufsize,
             sizeof(bufsize),&istatus);
 if ( istatus )
  {
   so_error("SETSOCKOPT ERROR",status);
   return(0);
  }

 /* Bind receive socket to the service port */

 struct sockaddr_in srv_addr;
 bzero ((char *) &srv_addr, sizeof(srv_addr));
 srv_addr.sin_family = AF_INET;
 srv_addr.sin_addr.s_addr = htonl (INADDR_ANY) ;
 srv_addr.sin_port = my_port;

 if ( bind(recv_socket,(struct sockaddr *)&srv_addr,sizeof(srv_addr),
           &istatus)==-1 )
  {
   clogger(E_MSG,__FILE__,__LINE__,"BIND RETURNED %d (%s)",istatus,
          socstrerror(status));
   return(0);
  }

  /* Start the receive task */

 recv_task_tkn=rqcreatetask(0,recv_task,
               (selector)(&data_segment),NULL,RECV_TASK_STACK,0,&status);
 if ( status )
  {
   clogger(E_MSG,__FILE__,__LINE__,"RQCREATETASK RETURNED %s",
          rmxstrerror(status));
   return(0);
  }

 atnetexit(stopjob);
 time(&stats.startup);
 return(1);
}

void send_datagram(DWORD destadr)
/****************************************************************************
 PURPOSE: Send the datagram that's currently in the global datagram buffer.
 ARGS:    destadr .. address to send the datagram to
 RETURNS: nothing
 ****************************************************************************/
{
 struct sockaddr_in srv_addr;
 int    status;

 /* Set up the server address packet */
 bzero ((char *) &srv_addr, sizeof(srv_addr));
 srv_addr.sin_family = AF_INET;
 srv_addr.sin_addr.s_addr = destadr;
 srv_addr.sin_port = my_port;

 /* Send the message */
 if ( sendto(send_socket,(char *)&datagram,
             datagram.len+sizeof(datagram.len),0,
           (struct sockaddr *)&srv_addr,sizeof(srv_addr),&status) == -1)
  {
   clogger(E_MSG,__FILE__,__LINE__,
          "SENDTO RETURNED %d (%s)", status, socstrerror(status));
    fail_datagram(INET_HOSTUNAVAIL);
    ++stats.send_errors;
   }
  else
   {
    ++stats.datagrams_sent;
    stats.messages_sent += datagram_msgcnt;
   }

 /* Reset datgram length and message count */
 datagram.len=0;
 datagram_msgcnt=0;
}

void main(void)
/*****************************************************************************
 PURPOSE: Main INETUDP function
 ARGS:    none
 RETURNS: nothing
 *****************************************************************************/
{
 struct netmsg *msg;
 DWORD destadr, curadr=0;
 struct datagram_msg *dptr=&datagram.msg1;
 struct inethdr *hdr;

 if ( netinit(MYJOBNAME) == 0xffff ||
      ! application_init() )
   exit(EXIT_FAILURE);

 myjobnumber=getmyjobnum();

 while(1)
  {
   /* Get a local message and look up the destination internet address */
   msg=netread( (config->inet.block_delay && datagram.len) ?
                config->inet.block_delay : FOREVER );
   if ( msg!=NULL )
    {
     if ( msg->m_type == MTYPE_OUT )
       hdr=(struct inethdr *)msg->out_msg;
     else
       hdr=(struct inethdr *)msg->in_msg;
     if ( (destadr = lookup_address(hdr->dcid)) == 0 )
      {
       fail_msg(msg,INET_HOSTUNKNOWN);
       continue;
      }
    }

   /* If I'm not blocking messages then I move the message to the
      datagram buffer and send it */

   if ( config->inet.block_delay == 0 )
    {
     dptr->len=msg->msg_len;
     memcpy(&dptr->hdr,hdr,msg->msg_len);
     datagram.len = msg->msg_len + sizeof(dptr->len);
     datagram_msgcnt=1;
     netdest(FREE_BUFFERS_DEST,&msg->msg_to,hdr->dcid);
     netwrite(msg);
     send_datagram(destadr);
     continue;
    }

   /* If I'm blocking messages then see if I have to send the
      current datagram for one of the following reasons:
        1. The netread timed out after block delay
        2. Message is not for same address as current datagram.
        3. Current datagram is full.
   */

   if ( datagram.len &&
        (msg == NULL || destadr != curadr ||
        (msg->msg_len + datagram.len + sizeof(datagram.len)) >
                                                        sizeof(datagram)) )
    {
     send_datagram(curadr);
     dptr=&datagram.msg1;
    }

   /* Add the message to the datagram buffer */
   curadr=destadr;
   dptr->len=msg->msg_len;
   memcpy(&dptr->hdr,hdr,msg->msg_len);
   datagram.len += msg->msg_len + sizeof(dptr->len);
   netdest(FREE_BUFFERS_DEST,&msg->msg_to,hdr->dcid);
   netwrite(msg);

   /* Point to where we insert the next message */
   dptr=(struct datagram_msg *) &((char *)&dptr->hdr)[ dptr->len ];
   ++datagram_msgcnt;
  }


}
