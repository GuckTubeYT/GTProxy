#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

char** strsplit(const char* s, const char* delim, size_t* nb);
int fast_compare(const char *ptr0, const char *ptr1, int len);
char* CatchMessage(const char *message, ...);
int findArray(char** array, char* val);
char* arrayJoin(char** array, char* joinVal);
char* generateHex(int len);
int findStr(char* str, char* toFind);

#endif // UTILS_H_INCLUDED