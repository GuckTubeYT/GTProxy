#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdint.h>

char** strsplit(const char* s, const char* delim, size_t* nb);
int fast_compare(const char *ptr0, const char *ptr1, int len);
char* CatchMessage(const char *message, ...);
int findArray(char** array, char* val);
char* arrayJoin(char** array, char* joinVal, char autoRemove);
char* generateHex(int len);
char* generateGID();
char* generateKlv(char* gameVersion, char* hash, char* rid, char* protocol, char isAndroid);
int findStr(char* str, char* toFind);
char isStr(unsigned char* str, unsigned char* toFind);
char includeStr(const unsigned char* str, const unsigned char* toFind, int len);
int32_t protonHash(const char* data);

#endif // UTILS_H_INCLUDED
