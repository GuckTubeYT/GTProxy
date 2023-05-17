#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdint.h>

#include "utils.h"
#include "../sha256/sha256.h"

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
    char* result = malloc(len ? (len * 2) + 1 : 18);
    char* hexVal = "0123456789ABCDEF";
    if (!len) {
        for (int a = 0, b = 0; a < 17; a++) {
            result[a] = tolower(hexVal[rand() % 16]), b++;
            if (b == 3) result[a] = ':', b = 0;
        }
        result[17] = '\0';
    } else {
        for (int a = 0; a < len * 2; a++) result[a] = hexVal[rand() % 16];
        result[len * 2] = '\0';
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

char* sha256Gen(char* data) {
    char* result = malloc(65); // 32 + 1
    BYTE sha256res[32];
    SHA256_CTX ctx;
    sha256Init(&ctx);
    sha256UpdateLen(&ctx, data);
    sha256Final(&ctx, sha256res);

    for (int a = 0, b = 0; a < 32; a++) {
        sprintf(result + b, "%02x", sha256res[a]);
        b += 2;
    }
    result[64] = '\0';

    return result;
}

char* generateKlv(char* gameVersion, char* hash, char* rid, char* protocol, char isAndroid) {
    char* gameVersion_sha256 = sha256Gen(gameVersion);
    char* hash_sha256 = sha256Gen(hash);
    char* protocol_sha256 = sha256Gen(protocol);
    char* rid_sha256 = sha256Gen(rid);

    char* result = sha256Gen(CatchMessage("%s198c4213effdbeb93ca64ea73c1f505f%s82a2e2940dd1b100f0d41d23b0bb6e4d%sc64f7f09cdd0c682e730d2f936f36ac2%s27d8da6190880ce95591215f2c9976a6", gameVersion_sha256, hash_sha256, protocol_sha256, rid_sha256));

    free(gameVersion_sha256);
    free(hash_sha256);
    free(protocol_sha256);
    free(rid_sha256);

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

char isStr(unsigned char* str, unsigned char* toFind, char isEndLine) {
    for (int a = 0; a < strlen(toFind); a++) {
        if (str[a] != toFind[a]) return 0;
    }
    if (str[strlen(toFind)] != '\0' && isEndLine) return 0;
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

int32_t protonHash(const char* data) {
    int hash = 0x55555555;
    while(*data) hash = (hash >> 27) + (hash << 5) + *data++;

    return hash;
}
