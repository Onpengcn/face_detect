objects = main.o cJSON.o get_access_token.o base64.o\
		  Upload_image.o
lcurl = -L/usr/local/lib -lcurl
lcam = -L./include -lcam
lmysql = -L/usr/lib/i386-linux-gnu/ -lmysqlclient

File_jpg = *.jpg
File_json = post_callback.json

vpath %.c src
vpath %.h include

main: $(objects)
	gcc -o $@ $^ $(lmysql) $(lcurl) $(lcam) -pthread

main.o : main.c get_access_token.h Upload_image.h cam.h
	gcc -c $< -Iinclude
cJSON.o : cJSON.c cJSON.h
	gcc -c $< -Iinclude
get_access_token.o : get_access_token.c cJSON.h 
	gcc -c $< -o $@ -Iinclude
base64.o : base64.c
	gcc -c $< -Iinclude
Upload_image.o : Upload_image.c cJSON.h base64.h
	gcc -c $< -Iinclude
	

.PHONY : clean

clean :
	rm main $(objects) $(File_jpg) $(File_json) 
