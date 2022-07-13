#ifndef __UNSNUM_H
#define __UNSNUM_H
#include <limits.h>
void atou(char*,unsigned long&);
void atou(char*,unsigned short&);
void atou(char*,unsigned int&);
void atou(char*,unsigned char&);
void AtoU(char*,unsigned long&);
void AtoU(char*,unsigned short&);
void AtoU(char*,unsigned int&);
void AtoU(char*,unsigned char&);
void utoa(char*,unsigned long,int width=0);
void utoa(char*,unsigned short,int width=0);
void utoa(char*,unsigned int,int width=0);
void utoa(char*,unsigned char,int width=0);
void UtoA(char*,unsigned long,int width=0);
void UtoA(char*,unsigned short,int width=0);
void UtoA(char*,unsigned int,int width=0);
void UtoA(char*,unsigned char,int width=0);
bool isEmpty(unsigned char);
bool isEmpty(unsigned short);
bool isEmpty(unsigned int);
bool isEmpty(unsigned long);
bool Empty(unsigned char&);
bool Empty(unsigned short&);
bool Empty(unsigned int&);
bool Empty(unsigned long&);
#endif
