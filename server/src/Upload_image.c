#include "Upload_image.h"
#define Post_length 1024*1024
#define request_urllen 300

FILE * fp;
char host_url[] = "https://aip.baidubce.com/rest/2.0/face/v3/detect";
char match_url[] = "https://aip.baidubce.com/rest/2.0/face/v3/match";
char faceDetect_result[1024]={0};
double faceMatch_result;

int Parse_photo(char *access_token, char *Pic_name){
	//char access_token1[] = "24.68ddf08d673762c9fddf81625be98355.2592000.1613095156.282335-23125647";
	char json_result[100] = {0};
	char *B64_buf;
	//char Pic_name[] = "image.jpg";

	B64_buf =  encode_pic(Pic_name);
	if(1 == faceDetect(B64_buf, json_result, access_token)){
		printf("curl face result error \n");
		return -1;
	}
	End_B64_buf();// release b64_buf memory
	return 0;
}

int match_photo(char *access_token, int order){
	char *B64_buf1, *B64_buf2;
	char file_number[5] = {0};
	int ret = -1;
	file_number[0] = order + 48;
	strcat(file_number, ".jpg");
	B64_buf1 = encode_pic("image.jpg");
	B64_buf2 = encode_pic(file_number);
	ret = faceMatch(B64_buf1, B64_buf2, access_token);
	if (ret == 0){
		printf("compare success\n");
		return 1;
	}
}

static size_t callback(void *ptr, size_t size, size_t nmemb, void *stream) {
	//printf("size = %d\n",size);
	//faceDetect_result = (char *)malloc(Post_length);
	//将数据写入文件
	//char faceDetect_result[1024]={0};
	fp = fopen("post_callback.json", "w");
	if(NULL == fp)
	{
		return -1;
	}
	int ret = fwrite(ptr, size, nmemb, fp);
	strcpy(faceDetect_result,(char *)ptr);
	printf("parse_result = %s\n",faceDetect_result);
	//free(faceDetect_result);
	return ret;
}

static size_t match_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
	cJSON * json = NULL;
	cJSON * json_msg = NULL;
	cJSON * json_score = NULL;
	fp = fopen("post_callback.json", "w");
	if(NULL == fp)
	{
		return -1;
	}
	int ret = fwrite(ptr, size, nmemb, fp);
	json = cJSON_Parse((char *)ptr);
	json_msg = cJSON_GetObjectItem(json, "result");
	
	json_score = cJSON_GetObjectItem(json_msg, "score");
	printf("parse_result = %s\n",cJSON_PrintUnformatted(json));
	faceMatch_result = json_score->valuedouble;
	printf("%.2lf\n", json_score->valuedouble);
	cJSON_Delete(json);
	return ret;
}

int faceMatch(char *B64_buf1, char *B64_buf2, char *access_token) {
	/***********************创建 json***********************/
	//{\"image\":\"027d8308a2ec665acb1bdf63e513bcb9\",\"image_type\":\"FACE_TOKEN\",\"face_field\":\"faceshape,facetype\"}
	char *json_toPost = (char *)malloc(Post_length);

	char request_url[request_urllen] = {0};
	strcpy(request_url, match_url);
	char *post_image1 = B64_buf1;
	char *post_image2 = B64_buf2;
	
	char post_image_type[] = "BASE64";
	char post_face_field[] = "LIVE";
	cJSON * final_json = cJSON_CreateArray();
	cJSON * post_json1 = cJSON_CreateObject();
	cJSON * post_json2 = cJSON_CreateObject();
	cJSON_AddItemToObject(post_json1, "image", cJSON_CreateString(post_image1));
	cJSON_AddItemToObject(post_json1, "image_type", cJSON_CreateString(post_image_type));
	cJSON_AddItemToObject(post_json1, "face_type", cJSON_CreateString(post_face_field));
	cJSON_AddItemToObject(post_json2, "image", cJSON_CreateString(post_image2));
	cJSON_AddItemToObject(post_json2, "image_type", cJSON_CreateString(post_image_type));
	cJSON_AddItemToObject(post_json2, "face_type", cJSON_CreateString(post_face_field));
	cJSON_AddItemToArray(final_json, post_json1);
	cJSON_AddItemToArray(final_json, post_json2);
	json_toPost = cJSON_PrintUnformatted(final_json);
	if (request_urllen < strlen(request_url)){
		printf("url_len out of length\n");
		return 1;
	}
	//组合请求url = request_url + "?access_token=" + access_token;
	strcat(request_url, "?access_token=");
	strcat(request_url, access_token);
	CURL *curl = NULL;
    CURLcode result_code;
    int is_success;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, request_url);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_toPost);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, match_callback);
		result_code = curl_easy_perform(curl);
        if (result_code != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(result_code));
            is_success = 1;
            return is_success;
        }
        curl_easy_cleanup(curl);
        is_success = 0;
		
		printf("success parse photo\n");
    } else {
        fprintf(stderr, "curl_easy_init() failed.");
        is_success = 1;
    }
	free(json_toPost);
	if(faceMatch_result > 20){
		printf("match success\n");
		return 0;
	}
	printf("face match not success\n");
	cJSON_Delete(final_json);
	cJSON_Delete(post_json1);
	cJSON_Delete(post_json2);
    return is_success;
}

int faceDetect(char *B64_buf, char *json_result, char *access_token) {
	/***********************创建 json***********************/
	//{\"image\":\"027d8308a2ec665acb1bdf63e513bcb9\",\"image_type\":\"FACE_TOKEN\",\"face_field\":\"faceshape,facetype\"}
	char *json_toPost = (char *)malloc(Post_length);
	//char json_toPost[Post_length] = {0};
	//char *json_toPost;
	char request_url[request_urllen] = {0};
	strcpy(request_url, host_url);
	char *post_image = B64_buf;
	char post_image_type[] = "BASE64";
	char post_face_field[] = "age,beauty,gender";
	cJSON * post_json = cJSON_CreateObject();
	cJSON_AddItemToObject(post_json, "image", cJSON_CreateString(post_image));
	cJSON_AddItemToObject(post_json, "image_type", cJSON_CreateString(post_image_type));
	cJSON_AddItemToObject(post_json, "face_field", cJSON_CreateString(post_face_field));
	json_toPost = cJSON_PrintUnformatted(post_json);
//	printf("post_json_size = %d\n",sizeof(cJSON_Print(post_json)));
	if (request_urllen < strlen(request_url)){
		printf("url_len out of length\n");
		return 1;
	}
	//组合请求url = request_url + "?access_token=" + access_token;
	strcat(request_url, "?access_token=");
	strcat(request_url, access_token);
	CURL *curl = NULL;
    CURLcode result_code;
    int is_success;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, request_url);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_toPost);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
		result_code = curl_easy_perform(curl);
        if (result_code != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(result_code));
            is_success = 1;
            return is_success;
        }
       // json_result = faceDetect_result;
        curl_easy_cleanup(curl);
        is_success = 0;
		printf("success parse photo\n");
    } else {
        fprintf(stderr, "curl_easy_init() failed.");
        is_success = 1;
    }
	free(json_toPost);
	cJSON_Delete(post_json);
    return is_success;
}
