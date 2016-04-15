#ifndef		_VIDEO_LIB_H_
#define		_VIDEO_LIB_H_


/***** VideoSaveJPG *****
filename ：文件名
quality  ：JPG质量 0-100（越大越好）
**************************/
int VideoSaveJPG(char *filename,int quality);

/***** SetVideo_Win *****
设置视频窗口的位置和大小
注意： 不是任何高度和宽度都可以的
**************************/
int SetVideo_Win(int left,int top,int width,int height);

/***** Video_init *****
视频初始化
如果初始化的视频没有输入视频将返回失败
**************************/
int Video_init(int left,int top,int width,int height);


#endif
