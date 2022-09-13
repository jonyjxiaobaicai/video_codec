/*
 * author: francis.fan@rock-chip.com
 * date:   2019-8-2
 */


// Y = 0.299R + 0.587G + 0.114B
// U = -0.147R - 0.289G + 0.436B
// V = 0.615R - 0.515G - 0.100B
//Y=0.30R+0.59G+0.11B ， U=0.493(B － Y) ， V=0.877(R － Y)
// R = Y + 1.14V

// G = Y - 0.39U - 0.58V

// B = Y + 2.03U



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "com.h"
typedef struct _overlayParam {
	unsigned char *bm_buff; //base map buff
	int bm_w; //base map width
	int bm_h; //base map height

	unsigned char *top_buff; //top overlay buff
	int top_w;
	int top_h;

	int pos_x; //position x [0, bm_w-top_w]
	int pos_y;
}overlayParam;

void dump_overlayparam(overlayParam *param)
{
	printf("=== overlayParam ===\n");
	printf("\tbm_buff:%p\n", param->bm_buff);
	printf("\tbm_w:%d\n", param->bm_w);
	printf("\tbm_h:%d\n\n", param->bm_h);
	printf("\ttop_buff:%p\n", param->top_buff);
	printf("\ttop_w:%d\n", param->top_w);
	printf("\ttop_h:%d\n\n", param->top_h);
	printf("\tpos_x:%d\n", param->pos_x);
	printf("\tpos_y:%d\n", param->pos_y);
}

/* 功能：nv12图像叠加 */
int pixel_format_nv12_overlay(overlayParam *param)
{
	unsigned char *rect_y = NULL;
	unsigned char *rect_u = NULL;
	unsigned char *rect_v = NULL;
	unsigned char *rect_tmp_y = NULL;
	unsigned char *rect_tmp_u = NULL;
	unsigned char *rect_tmp_v = NULL;
	unsigned  char * top_y = NULL;
	unsigned  char * top_u =NULL;
	unsigned  char * top_v=NULL;
	int i, j;
	static int steps=0;
	static int line_widgth=20;
	dump_overlayparam(param);

	if (!param || !(param->bm_buff) ||
		((param->bm_w * param->bm_h) <= 0) || ((param->top_w * param->top_h) <= 0) ||
		(param->pos_x < 0) || (param->pos_y < 0)) {
		printf("ERROR: %s input args invalid!\n", __func__);
		return -EINVAL;
	}

	/* Position align */
	param->pos_x = (param->pos_x / 2) * 2;
	param->pos_y = (param->pos_y / 2) * 2;

	if (((param->pos_x + param->top_w) >= param->bm_w) ||
		(((param->pos_y + param->top_h) >= param->bm_h))) {
		printf("ERROR: %s overlay img size invalid!\n", __func__);
		return -EINVAL;
	}

	/*
	 * 以(pos_x, pos_y)为起始点分别向右（ｘ轴）和向下（ｙ轴）画一个矩形框（图片左上角为原点），
	　* 矩形框的款高就是顶图的宽高。该矩形框便是需要替换成顶图的区域。
	 */
	rect_y = param->bm_buff;// + param->pos_y * param->bm_w + param->pos_x;
	rect_u = param->bm_buff + param->bm_w * param->bm_h + param->pos_y * param->bm_w / 2 + param->pos_x;
	rect_v = rect_u + 1;
	top_y = param->top_buff;
	top_u = param->top_buff + param->top_w * param->top_h;
	top_v = top_u + 1;

	#if 1
	for (i = 0; i < param->top_h; i++) {
		/* 在底图中的需要overlay的矩形框内 */
		rect_tmp_y = rect_y + i * param->bm_w;
		if (i % 2 == 0) {
			rect_tmp_u = rect_u + i * param->bm_w / 2;
			rect_tmp_v = rect_tmp_u + 1;
		}

		for (j = 0; j < param->top_w; j++) {
			/* Replace y value */
			*(rect_tmp_y + j) = *(top_y + i * param->top_w + j);

			if ((i % 2 == 0) && (j % 2 == 0)) {
				*rect_tmp_u = *top_u;
				top_u += 2;
				rect_tmp_u += 2;
			} else if ((i % 2 == 1) && (j % 2 == 1)) {
				*rect_tmp_v = *top_v;
				top_v += 2;
				rect_tmp_v += 2;
			}
		}
	}
	#elif 0
		for (i = 0; i < param->top_h; i++) {// 第一个像素开始遍历
		/* 在底图中的需要overlay的矩形框内 */
		rect_tmp_y = rect_y + i * param->bm_w; // i*base_pic_w 
		if (i % 2 == 0) {
			rect_tmp_u = rect_u + i * param->bm_w / 2;
			rect_tmp_v = rect_tmp_u + 1;
		}


		for (j = 0; j < param->top_w; j++) {


			if(i<=(param->top_h-line_widgth)&&i>=line_widgth)
			{
				if(j<param->top_w-line_widgth&&j>line_widgth&&steps==0){
					j+=param->top_w-line_widgth*2-1;
					steps=1;//once times 
				}
				else {
					steps=0;
				}
				
				//j+=param->top_w-10;
			}
			// printf("%d - ", j);	

			/* Replace y value */
			*(rect_tmp_y + j) = 0;//*(top_y + i * param->top_w + j);

			if ((i % 2 == 0) && (j % 2 == 0)) {
				*rect_tmp_u = 0;//*top_u;
				/// printf("%d - ", j);	
				rect_tmp_u += 2;
			} else if ((i % 2 == 1) && (j % 2 == 1)) {
				*rect_tmp_v =0;// *top_v;
				// printf("%d - ", j);	
				rect_tmp_v += 2;
			}
		}
	}
	#elif 0
		//1 画y 
		rect_tmp_y=rect_y;
		for(i=param->pos_y;i<param->pos_y+param->top_h;i++){

			for(j=param->pos_x;j<param->pos_x+param->top_w;j++){
				*(rect_tmp_y+i*param->bm_h+j)=0;
				printf("(%d , %d ) - ", j , i );
			}
			printf("\n");
		}

		//2 画 uv 


	#endif
	return 0;
}

/* 用法介绍：./main baseMap.nv12 topMap.nv12 outputMap.nv12 */
int main(int argc, char *argv[])
{
	FILE *r_fd = NULL, *w_fd = NULL;
	char *bm_buff = NULL, *top_buff = NULL;
	int w, h;
	int ret = 0;
	overlayParam param;

	if (argc != 4) {
		printf("[Usage]:\n\t./main baseMap.nv12 topMap.nv12 outputMap.nv12\n");
		return -EINVAL;
	}

	printf("#InputBaseMapFile:%s\n", argv[1]);
	printf("#InputTopMapFile:%s\n", argv[2]);
	printf("#OutputFile:%s\n", argv[3]);

	memset(&param, 0, sizeof(param));

	/*************************************************
	 * 底图载入buff，并设置overlayParam
	 *************************************************/
	/* 底图宽高 */
    w = 1920;
    h = 1080;

    r_fd = fopen(argv[1], "r");
    if (r_fd == NULL) {
        printf("open %s failed\n", argv[1]);
        goto OUT;
    }

	bm_buff = (char *) malloc(w * h * 3 / 2);
	if (!bm_buff) {
		printf("ERROR: No space left for basemap!\n");
        goto OUT;
	}

	/* 图像载入buff */
    ret = fread(bm_buff, 1, w * h * 3 / 2, r_fd);
	if (ret < (w * h * 3 / 2)) {
    	printf("ERROR: Read %s error. read byte: %d(Expecting:%d)\n",
			argv[1], ret, w * h * 3 / 2);
		goto OUT;
	}

	fclose(r_fd);
	r_fd = NULL;

	/* overlayParam参数设置 */
	param.bm_w = w;
	param.bm_h = h;
	param.bm_buff = bm_buff;

	/*************************************************
	 * 顶图载入buff，并设置overlayParam
	 *************************************************/
	/* 顶图宽高 */
    w = 50;
    h = 50;

	// r_fd = fopen(argv[2], "r");
    // if (r_fd == NULL) {
    //     printf("open %s failed\n", argv[2]);
    //     goto OUT;
    // }

	// top_buff = (char *) malloc(w * h * 3 / 2);
	// if (!top_buff) {
	// 	printf("ERROR: No space left for topmap!\n");
	// 	goto OUT;
	// }

	// /* 载入顶图到buff */
    // ret = fread(top_buff, 1, w * h * 3 / 2, r_fd);
	// if (ret < (w * h * 3 / 2)) {
    // 	printf("ERROR: Read %s error. read byte: %d(Expecting:%d)\n",
	// 		argv[2], ret, w * h * 3 / 2);
	// 	goto OUT;
	// }

	// fclose(r_fd);
	// r_fd = NULL;

	/* overlayParam参数设置 */
	param.top_w = w;
	param.top_h = h;
	param.top_buff = NULL;
	/* 设置顶图插入底图的位置 */	
	param.pos_x = 1920 - 144 - 500;
	param.pos_y = 500;
RECT Rect;	
	Rect.left=50;
	Rect.top=50;
	Rect.right=1000;
	Rect.bottom=1000;
	
	YUV_Draw_Rect(param.bm_buff, 1920,1080, V4L2_PIX_FMT_NV12,&Rect,0,5);


	// ret = pixel_format_nv12_overlay(&param);
	// if (ret < 0) {
	// 	printf("ERROR: add overlay(%s) to %s failed!\n", argv[2], argv[1]);
	// 	goto OUT;
	// }

	w_fd = fopen(argv[3], "w+");
    if (w_fd == NULL) {
        printf("open %s failed\n", argv[3]);
        goto OUT;
    }

	ret = fwrite(param.bm_buff, 1, param.bm_w * param.bm_h * 3 / 2, w_fd);
	if (ret != (param.bm_w * param.bm_h * 3 / 2))
		printf("ERROR: write %s failed!\n", argv[3]);

OUT:
	if (r_fd)
		fclose(r_fd);
	if (w_fd)
		fclose(w_fd);
	if (bm_buff)
		free(bm_buff);
	// if (top_buff)
	// 	free(top_buff);

	return 0;
}
