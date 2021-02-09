#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <mysql/mysql.h>

#include "get_access_token.h"
#include "Upload_image.h"
#include "cam.h"

/**************函数声明**************/
void *Tobuf(void *arg);
void *Saveimg(void *arg);
/**************全局变量**************/
int ret = -1,rws = -1,flag = 1, ser = -1;//服务器变量
//camera 变量
int pic = -1,cam_flag = 0, push_flag = 0;//摄像头的文件描述符
unsigned int size, photo_index;
char *pic_buf = NULL;//存放图片内容


char access_token[] = "24.68ddf08d673762c9fddf81625be98355.2592000.1613095156.282335-23125647";

int main(){
	/************************** 摄像头 ***************************/
	pic = camera_init("/dev/video0", &size);
	if(0 > pic)
		return pic;
	/****************线程****************/
	pthread_t camera_SendBuf;
	pthread_t camera_SaveImg;
	//创建线程
	if( 0 > pthread_create(&camera_SaveImg, NULL, Saveimg, NULL))
	{
		printf("pthread_create fail\n");
		return -1;
	}	
	if( 0 > pthread_create(&camera_SendBuf, NULL, Tobuf, NULL))
	{
		printf("pthread_create fail\n");
		return -1;
	}	
	pthread_detach(camera_SendBuf);
	pthread_detach(camera_SaveImg);
	
	//Parse_photo(access_token, "image.jpg");
	while(1);
	return 0;
}

void *Saveimg(void *arg)
{
	int number = 0;
	/***************数据库创建连接****************/
	MYSQL           mysql;  
    MYSQL_RES       *res = NULL;  
    MYSQL_ROW       row;  
    char            *query_str = NULL;  
    int             rc, i, fields;  
    int             rows;  
    if (NULL == mysql_init(&mysql)) {  
        printf("mysql_init(): %s\n", mysql_error(&mysql));  
        return -1;  
    }  
    if (NULL == mysql_real_connect(&mysql,  
                "localhost",  
                "root",  
                "shallnet",  
                "db_users",  
                0,  
                NULL,  
                0)) {  
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));  
        return -1;  
    }  

	/*********** 1.创建套接字 *************/
	ser = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == ser)
	{
		perror("socket");
	}
	
	printf("ser = %d\n", ser);
	
	/*********** 2.绑定IP和端口号等信息 *************/
	//填写服务器的信息
	struct sockaddr_in  ser_addr;
	ser_addr.sin_family   = AF_INET;
	ser_addr.sin_port     = htons(8080);
	ser_addr.sin_addr.s_addr = INADDR_ANY;//inet_addr("192.168.3.239");
	memset(ser_addr.sin_zero, 0, 8);
	
	socklen_t  ser_addr_len = sizeof(ser_addr);
	
	if(-1 == bind(ser, (struct sockaddr *)&ser_addr, ser_addr_len))
	{
		perror("bind");
		close(ser);
	}
	
	/*********** 3.设置最大连接数 *************/
	if(-1 == listen(ser, 5))
	{
		perror("lsiten");
		close(ser);
	}

	/*********** 4.等待连接 *************/
	//定义客户端的地址结构
	struct sockaddr_in  cli_addr;
	socklen_t   cli_addr_len = sizeof(cli_addr);
                                                           
	flag = 1;

	//实时显示视频信息
	while(1)
	{
		if(flag == 0){
			char read_buf[10] = {0};
			ret = read(rws, read_buf, sizeof(read_buf));
		/*
			if(-1 == ret)
			{
				printf("read error\n");
				//break;
			}
			*/
			if(0 == ret)
			{
				printf("client leave");
				flag = 1;
				//camera_close(pic);
				continue;
			}
			/******************处理接受的信息*****************/
			if(strstr(read_buf, "take") !=NULL){
				printf("receive:%s\n",read_buf);
				char file_name[5] = {0};
				file_name[0] = number+48;
				number++;
				strcat(file_name, ".jpg");
				FILE *fd = NULL;
				fd = fopen(file_name,"wb");
				if(NULL == fd)
				{
					perror("open");
				}
				cam_flag = 1;
				camera_dqbuf(pic, (void **)&pic_buf, &size, &photo_index);
				fwrite(pic_buf, 1, size, fd);
				camera_eqbuf(pic, photo_index);
				fclose(fd);
				//Parse_photo(access_token, "image.jpg");
				cam_flag = 0;
			}
			else if(strstr(read_buf, "push_on") !=NULL){
				printf("push on\n");
				push_flag = 1;
			}
			else if(strstr(read_buf, "push_off") !=NULL){
				printf("push off\n");
				push_flag = 0;
			}
			else if(strstr(read_buf, "compare") !=NULL){
				printf("compare\n");
				FILE *fd = NULL;
				fd = fopen("image.jpg","wb");
				if(NULL == fd)
				{
					perror("open");
				}
				cam_flag = 1;
				camera_dqbuf(pic, (void **)&pic_buf, &size, &photo_index);
				fwrite(pic_buf, 1, size, fd);
				camera_eqbuf(pic, photo_index);
				fclose(fd);
				cam_flag = 0;
				
				match_photo(access_token, 0);
			}
			else if(strstr(read_buf, "mysql") !=NULL){
				printf("mysql\n");
    		//执行插入请求  
    		    query_str = "insert into tb_users values (12345, 'justtest', '2015-5-5')";  
    		    rc = mysql_real_query(&mysql, query_str, strlen(query_str));  
    		    if (0 != rc) {  
    		        printf("mysql_real_query(): %s\n", mysql_error(&mysql));  
    		        return -1;  
    		    }  
    		//执行删除请求  
    		    query_str = "delete from tb_users where userid=10006";  
    		    rc = mysql_real_query(&mysql, query_str, strlen(query_str));  
    		    if (0 != rc) {  
    		        printf("mysql_real_query(): %s\n", mysql_error(&mysql));  
    		        return -1;  
    		    } 
			}
			else {
				printf("else\n");
				mysql_close(&mysql); 
			}
		}
		else if(flag == 1){
			printf("wait for client...\n");
			mysql_close(&mysql); 
			
			rws = accept(ser, (struct sockaddr *)&cli_addr, &cli_addr_len);
			if(-1 == rws)
			{
				//perror("accept");
				close(ser);
			}
			flag = 0;
			printf("connected\n");
			camera_start(pic);
		}
	}
}

void *Tobuf(void *arg){
	while(1)
	{
		if(flag == 0 && push_flag == 1){
			/********* 取一张图片 ************/
			ret = camera_dqbuf(pic, (void **)&pic_buf, &size, &photo_index);
			if(0 > ret)
				printf("get image error\n");
			/********* 发给客户端 ************/
			//发图片大小
			write(rws, (char *)&size, 4);

			//发送内容
			write(rws, pic_buf, size);

			/********* 入队 ************/
			camera_eqbuf(pic, photo_index);
			while(cam_flag == 1);
		}
	}
}
