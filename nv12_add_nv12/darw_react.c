/*
 * @Dexcripttion: X2000_Project
 * @version: 1.0.0
 * @Author: XY_zhan
 * @Date: 2022-09-13 17:06:41
 * @Email: 1756506948@qq.com
 * @Factory depot: 
 */
#include "com.h"
#include <math.h>

static int YUY_Draw_value(unsigned char * YuvBuf, int ImgW, int ImgH, int fmt,int PntX, int PntY,unsigned char buff[])
{
    int ret = -1;
	
    if(YuvBuf == NULL)
		return ret;

	switch(fmt)
	{
        case V4L2_PIX_FMT_YUYV:
        {
            int Yoffset = PntY * ImgW * 2 + (PntX * 2); 
			int Uoffset,Voffset;
			/*Y U Y V Y U Y V
	    pexl  0   1   2   3
        data  0 1 2 3 4 5 6 7
			 *Y U Y V Y U Y V */
			 
			if(PntX % 2 == 0)
			{
				Uoffset =  PntY * ImgW * 2 + (PntX * 2 + 1);
				Voffset =  PntY * ImgW * 2 + (PntX * 2 + 3);
			}
			else
			{
				Uoffset =  PntY * ImgW * 2 + (PntX * 2 - 1);
				Voffset =  PntY * ImgW * 2 + (PntX * 2 + 1);
			}
			
            YuvBuf[Yoffset] = buff[0];
			YuvBuf[Uoffset] =  buff[1];
			YuvBuf[Voffset] =  buff[2];
		}
		    ret = 0;
		break;
		
		case V4L2_PIX_FMT_NV12:
		{
            int Yoffset = PntY * ImgW + PntX;
			int Uoffset,Voffset;
			
		    Uoffset =  ImgH * ImgW + (PntY / 2) * ImgW + PntX / 2 * 2 ;
		    Voffset =  Uoffset + 1;
			
            YuvBuf[Yoffset] = buff[0];
			YuvBuf[Uoffset] =  buff[1];
			YuvBuf[Voffset] =  buff[2];
		}
		    ret = 0;
		break;

		default:
		    ret = -2;
		break;
	}
    return ret;
}


static int YUY_Draw_Piont(unsigned char * YuvBuf, int ImgW, int ImgH, int fmt,int PntX, int PntY, int Color)
{
    int ret = -1;
	
    if(YuvBuf == NULL)
		return ret;

	switch(fmt)
	{
        case V4L2_PIX_FMT_YUYV:
        {
            int Yoffset = PntY * ImgW * 2 + (PntX * 2); 
			int Uoffset,Voffset;
			/*Y U Y V Y U Y V
	    pexl  0   1   2   3
        data  0 1 2 3 4 5 6 7
			 *Y U Y V Y U Y V */
			 
			if(PntX % 2 == 0)
			{
				Uoffset =  PntY * ImgW * 2 + (PntX * 2 + 1);
				Voffset =  PntY * ImgW * 2 + (PntX * 2 + 3);
			}
			else
			{
				Uoffset =  PntY * ImgW * 2 + (PntX * 2 - 1);
				Voffset =  PntY * ImgW * 2 + (PntX * 2 + 1);
			}
			
            YuvBuf[Yoffset] = s_color_table[Color].Y;
			YuvBuf[Uoffset] = s_color_table[Color].U;
			YuvBuf[Voffset] = s_color_table[Color].V;
		}
		    ret = 0;
		break;
		
		case V4L2_PIX_FMT_NV12:
		{
            int Yoffset = PntY * ImgW + PntX;
			int Uoffset,Voffset;
			
		    Uoffset =  ImgH * ImgW + (PntY / 2) * ImgW + PntX / 2 * 2 ;
		    Voffset =  Uoffset + 1;
			
            YuvBuf[Yoffset] = s_color_table[Color].Y;
			YuvBuf[Uoffset] = s_color_table[Color].U;
			YuvBuf[Voffset] = s_color_table[Color].V;
		}
		    ret = 0;
		break;

		default:
		    ret = -2;
		break;
	}
    return ret;
}

static int YUY_Draw_Line(unsigned char * YuvBuf, int ImgW, int ImgH, int fmt,int SPntX, int SPntY,int EPntX, int EPntY, int Color,int lineWidth)
{
    int ret = -1;
    if(YuvBuf == NULL)
		return ret;

	if(lineWidth == 0)
		lineWidth = 1;

    SPntX = (SPntX + lineWidth >= ImgW) ? (ImgW - lineWidth) : (SPntX < 0 ? 0 : SPntX);
    EPntX = (EPntX + lineWidth >= ImgW) ? (ImgW - lineWidth) : (EPntX < 0 ? 0 : EPntX); 
    SPntY = (SPntY + lineWidth >= ImgH) ? (ImgH - lineWidth) : (SPntY < 0 ? 0 : SPntY); 
    EPntY = (EPntY + lineWidth >= ImgH) ? (ImgH - lineWidth) : (EPntY < 0 ? 0 : EPntY);  

    int dx = abs( (SPntX - EPntX));//(SPntX > EPntX) ? (SPntX - EPntX) : (EPntX - SPntX); // length 正值   abs 
    int dy = abs((EPntY - SPntY));//(SPntY > EPntY) ? (SPntY - EPntY) : (EPntY - SPntY);  // width 
    int xstep = (SPntX < EPntX) ? 1 : -1;
	int ystep = (SPntY < EPntY) ? 1 : -1; 
    int nstep = 0, eps = 0; 
	int pointX = SPntX;
	int	pointY = SPntY;
	
     if(dx > dy)
    {  
    	while(nstep <= dx) 
    	{	
    		ret = YUY_Draw_Piont(YuvBuf, ImgW, ImgH, fmt, pointX, pointY, Color);
			if(ret < 0)
				break;
			
    		eps += dy;	 
    		if( (eps << 1) >= dx ) // 计算斜率
    		{	 
    			pointY += ystep;
    			eps -= dx;	 
    		}	
    		pointX += xstep;	 
    		nstep++;  
    	} 
    }
    else 
    {  
    	while(nstep <= dy)
    	{
    		ret = YUY_Draw_Piont(YuvBuf, ImgW, ImgH, fmt, pointX, pointY, Color); 
			if(ret < 0)
				break;
			
    		eps += dx;	 
    		if( (eps << 1) >= dy ) 
    		{	 
    			pointX += xstep;	  
    			eps -= dy;	 
    		}	
    			pointY += ystep;	 
    			nstep++;
    	} 
    }
    return ret;
}


int YUV_Draw_Rect(unsigned char *YuvBuf, int ImgW, int ImgH, int fmt,RECT* Rect, int Color,int lineWidth)
{
    int ret = -1;
	if(YuvBuf == NULL)
		return ret;

	if(Rect->bottom == Rect->top || Rect->left == Rect->right)
		return ret;

	YUY_Draw_Line(YuvBuf, ImgW, ImgH,fmt, Rect->left, Rect->top , Rect->right, Rect->top, Color,lineWidth);
	YUY_Draw_Line(YuvBuf, ImgW, ImgH,fmt, Rect->right, Rect->top, Rect->right, Rect->bottom, Color,lineWidth);
	YUY_Draw_Line(YuvBuf, ImgW, ImgH,fmt, Rect->left, Rect->top , Rect->left,  Rect->bottom, Color,lineWidth);
	YUY_Draw_Line(YuvBuf, ImgW, ImgH,fmt, Rect->left,  Rect->bottom, Rect->right, Rect->bottom, Color,lineWidth);
	
    return 0;
}

