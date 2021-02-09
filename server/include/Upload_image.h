#ifndef __UPLOAD_IMAGE_H
#define __UPLOAD_IMAGE_H

#include <stdio.h>		  
#include <curl/curl.h>    
#include <string.h>

#include "base64.h"
#include "cJSON.h"	
int Parse_photo(char *access_token, char *Pic_name);
int match_photo(char *access_token, int order);
int faceMatch(char *B64_buf1, char *B64_buf2, char *access_token);
int faceDetect(char *B64_buf, char *json_result, char *access_token);
#endif
