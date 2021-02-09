#ifndef __CAM_H__
#define	__CAM_H__

//摄像头初始化：打开摄像头、检查设备属性、设置设备...
int camera_init(char *devpath, unsigned int *size);

//开启摄像头
int camera_start(int fd);

//取图片(出队)
int camera_dqbuf(int fd, void **buf, unsigned int *size, unsigned int *index);

//入队
int camera_eqbuf(int fd, unsigned int index);

//关闭摄像头
int camera_close(int fd);

#endif
