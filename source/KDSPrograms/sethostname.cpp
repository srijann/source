#include <unistd.h>

void main(int argc,char *argv[])
{
 if ( argc > 1 )
   sethostname(argv[1],strlen(argv[1]));
}