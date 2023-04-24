#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdint.h>

#include "utils.h"
#include "../md5/md5.h"

char** strsplit(const char* s, const char* delim, size_t* nb) { // https://github.com/mr21/strsplit.c
    void* data;
    char* _s = ( char* )s;
    const char** ptrs;
    size_t ptrsSize, nbWords = 1,sLen = strlen( s ), delimLen = strlen( delim );

    while ( ( _s = strstr( _s, delim ) ) ) {
      _s += delimLen;
      ++nbWords;
    }
    ptrsSize = ( nbWords + 1 ) * sizeof( char* );
    ptrs =
    data = malloc( ptrsSize + sLen + 1 );
    if ( data ) {
      *ptrs =
      _s = strcpy( ( ( char* )data ) + ptrsSize, s );
      if ( nbWords > 1 ) {
        while ( ( _s = strstr( _s, delim ) ) ) {
          *_s = '\0';
          _s += delimLen;
          *++ptrs = _s;
        }
      }
      *++ptrs = NULL;
    }
    if ( nb ) *nb = data ? nbWords : 0;
    return data;
}

int fast_compare( const char *ptr0, const char *ptr1, int len) { // https://mgronhol.github.io/fast-strcmp
    int fast = len/sizeof(size_t) + 1;
    int offset = (fast-1)*sizeof(size_t);
    int current_block = 0;

    if( len <= sizeof(size_t)){ fast = 0; }


    size_t *lptr0 = (size_t*)ptr0;
    size_t *lptr1 = (size_t*)ptr1;

    while( current_block < fast ){
      if( (lptr0[current_block] ^ lptr1[current_block] )){
        int pos;
        for(pos = current_block*sizeof(size_t); pos < len ; ++pos ){
          if( (ptr0[pos] ^ ptr1[pos]) || (ptr0[pos] == 0) || (ptr1[pos] == 0) ){
            return  (int)((unsigned char)ptr0[pos] - (unsigned char)ptr1[pos]);
            }
          }
        }

      ++current_block;
      }

    while( len > offset ){
      if( (ptr0[offset] ^ ptr1[offset] )){
        return (int)((unsigned char)ptr0[offset] - (unsigned char)ptr1[offset]);
        }
      ++offset;
      }

    return 0;
}

char* CatchMessage(const char *message, ...) {
    static char* result;
    result = malloc(1024);
    va_list ap;
    va_start(ap, message);
    vsnprintf(result, 1024, message, ap);
    va_end(ap);
    return result;
}

int findArray(char** array, char* val) {
    int result = 0;
    char isFound = 0;
    doSearch:
    while(array[result]) {
        for (int a = 0; a < strlen(val); a++) {
            if (array[result][a] != val[a]) {
                result++;
                goto doSearch;
            }
        }
        isFound = 1;
        break;
    }
    if (isFound) return result;
    else return -1;
}

char* arrayJoin(char** array, char* joinVal, char autoRemove) {
    int a = 0, totalPos = 0, currentPos = 0;

    while(array[a]) {
        if (array[a][0] || (!array[a][0] && !autoRemove)) {
            totalPos += strlen(array[a++]) + strlen(joinVal);
        } else if (!array[a][0] && autoRemove) a++;
    }

    char* result = malloc(totalPos + 1);
    a = 0;

    while(array[a]) {
        if (array[a][0] || (!array[a][0] && !autoRemove)) {
            sprintf(result + currentPos, "%s%s", array[a], joinVal);
            currentPos += strlen(array[a++]) + strlen(joinVal);
        } else if (!array[a][0] && autoRemove) a++;
    }

    result[currentPos] = '\0';
    //result[pos - valLen] = '\0';

    return result;
}

char* generateHex(int len) {
    char* result = malloc(len ? len + 1 : 18);
    char* hexVal = "0123456789ABCDEF";
    if (!len) {
        for (int a = 0, b = 0; a < 17; a++) {
            result[a] = tolower(hexVal[rand() % 16]), b++;
            if (b == 3) result[a] = ':', b = 0;
        }
        result[17] = '\0';
    } else {
        for (int a = 0; a < len; a++) result[a] = hexVal[rand() % 16];
        result[len] = '\0';
    }

    return result;
}

char* generateGID() {
    char* result = malloc(37);
    char* hexVal = "0123456789abcdef";
    for (int a = 0; a < 36; a++) {
        if (a == 8 || a == 13 || a == 18 || a == 23) result[a] = '-';
        else result[a] = tolower(hexVal[rand() % 16]);
    }

    result[36] = '\0';

    return result;
}

char* generateKlv(char* gameVersion, char* hash, char* rid, char* protocol) {
    uint8_t md5Data[16];
    md5String(CatchMessage("0b02ea1d8610bab98fbc1d574e5156f3%sb414b94c3279a2099bd817ba3a025cfc%sbf102589b28a8cc3017cba9aec1306f5%sdded9b27d5ce7f8c8ceb1c9ba25f378d%s", gameVersion, hash, rid, protocol), md5Data);
    char* result = malloc(33);
    for (int a = 0, b = 0; a < 16; a++) {
        sprintf(result + b, "%02X", md5Data[a]);
        b += 2;
    }
    result[32] = '\0';
    return result;
}

int findStr(char* str, char* toFind) {
    for (int a = 0, b = 0; a < strlen(str); a++) {
        if (str[a] == toFind[b]) b++;
        else b = 0;
        if (b == strlen(toFind)) return a + 1;
    }
    return 0;
}

char isStr(unsigned char* str, unsigned char* toFind) {
    for (int a = 0; a < strlen(toFind); a++) {
        if (str[a] != toFind[a]) return 0;
    }
    if (str[strlen(toFind)] != '\0') return 0;
    return 1;
}

char includeStr(const unsigned char* str, const unsigned char* toFind, int len) {
	int toFindLen = strlen(toFind);
	for (int a = 0, b = 0; a < len; a++) {
		if (str[a] == toFind[b]) b++;
		else b = 0;
		if (toFindLen == b) return 1;
	}
	return 0;
}
