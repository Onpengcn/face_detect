#ifndef __BASE64_H_
#define __BASE64_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

char * base64_encode( const unsigned char * bindata, char * base64, int binlength );
int base64_decode( const char * base64, unsigned char * bindata );

void End_B64_buf(void);
char * encode_pic(const char * File_Name);
#endif
