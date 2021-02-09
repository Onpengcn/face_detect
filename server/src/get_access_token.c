#include "get_access_token.h"
FILE * fp;

	/*
	 *   Get access_token function
	 */
	/*
	char access_token[200] = {0};
	int ret = get_access_token(access_token, "v5SOuNDD9CpR9ocBeWaIDNOe", "6WN9ChKQwMgg5umBWA79wNohFa8ZXBhp");
	if(ret < 0){
		printf("get token error\n");
		return -1;
	}
	printf("access_token = %s\n",access_token);
	*/

//回调函数
size_t curl_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	cJSON * json = NULL;
	cJSON * json_access_token = NULL;
	//将数据写入文件 
	int ret = fwrite(ptr, size, nmemb, fp);
	json = cJSON_Parse(ptr);
	if (NULL == json){
		printf("json_parse error\n");
		return (size_t)-1;
	}

	json_access_token = cJSON_GetObjectItem(json, "access_token");
	printf("json_access_token = %s",json_access_token->valuestring);
	
	cJSON_Delete(json);
	return ret;
}

int get_access_token(char *access_token, char *AK, char *SK)
{
	CURL * curl;
	/*********0.构建url ************/
	strcat(access_token,"https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials");
	strcat(access_token,"&client_id=");
	strcat(access_token, AK);
	strcat(access_token,"&client_secret=");
	strcat(access_token, SK);
	printf("access_token_URL = %s\n",access_token);
	/******** 1.初始化所有可能的调用 **********/
	curl_global_init(CURL_GLOBAL_ALL);

	/******** 2.初始化CURL类型的指针 **********/
	curl = curl_easy_init();

	/******** 3.设置URL **********/
	curl_easy_setopt(curl, CURLOPT_URL, access_token);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	//打开文件
	fp = fopen("access_token.txt", "w");
	if(NULL == fp)
	{
		curl_easy_cleanup(curl);
		return -1;
	}

	/******** 4.设置回调函数 **********/
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
	/******** 5.执行 **********/
	curl_easy_perform(curl);

	/******** 6.回收 **********/
	curl_easy_cleanup(curl);
	
	return 0;
}




























