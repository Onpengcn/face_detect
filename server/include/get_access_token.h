/*===============================================================
*   Copyright (C) 2021 All rights reserved.
*   
*   文件名称：get_access_token.h
*   创 建 者：liujing
*   创建日期：2021年01月12日
*   描    述：
*
*   更新日志：
*
================================================================*/
#ifndef _GET_ACCESS_TOKEN_H
#define _GET_ACCESS_TOKEN_H

#include <stdio.h>
#include <string.h>
#include "curl/curl.h"
#include "cJSON.h"


size_t curl_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

int get_access_token(char *access_token, char *AK, char *SK);

#endif
