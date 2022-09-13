/*
 * @Dexcripttion: X2000_Project
 * @version: 1.0.0
 * @Author: XY_zhan
 * @Date: 2022-09-13 17:09:58
 * @Email: 1756506948@qq.com
 * @Factory depot: 
 */
#ifndef ___com
#define ___com

#include <stdio.h>
typedef struct{
	int left;
    int top;
    int right;
    int bottom;
}RECT;
typedef struct
{ 
    char Y; 
    char U; 
    char V;
}stYuvColor;

enum pix_fmt{
    V4L2_PIX_FMT_YUYV,
    V4L2_PIX_FMT_NV12
};
static stYuvColor s_color_table[3] = 
{ 
    {0x4c, 0x00, 0xDF}, 
    // red 
    {0x51, 0x5A, 0xFA}, 
    // red 
    {0x28, 0xff, 0x00}, 
    // blue 
};

int YUV_Draw_Rect(unsigned char *YuvBuf, int ImgW, int ImgH, int fmt,RECT* Rect, int Color,int lineWidth);
#endif