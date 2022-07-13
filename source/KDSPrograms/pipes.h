#ifndef __PIPES_H
#define __PIPES_H

class Pipe {
  char* PipeName;
  char* IOBuf;
  int  Fd;
  int  Open();
  public:
  Pipe(char* Name);
  ~Pipe();
  bool Flush();
  int  Read(char* Buf,int Len,int Wait=-1);
  bool Write(char* Buf,int Len);
  bool Abort();
  char* GetName() { return PipeName; };
 };
#endif
