#pragma once
#ifndef __MP4_COMMON_H__
#define __MP4_COMMON_H__
#include <iostream>

//c11 新增特性，显示声明enum大小
enum  BoxType:int64_t{
	
	FTYP     = 0x66747970,
	MOOV	 = 0x6D6F6F76,
	MVHD	 = 0x6D766864,			//This box defines overall information which is media-independent, and relevant to the entire presentation
	NONETYPE = 0x00,
};

typedef struct  {
	int64_t	create_time;
	int64_t	modify_time;
	int64_t	timescale;
	int64_t	duration;
}media_version_0;


// 有待研究
typedef struct {
	uint64_t	create_time;
	uint64_t	modify_time;
	uint64_t	timescale;
	uint64_t	duration;
}media_version_1;

typedef struct mvhdbox {
	int version;
	union Media {
		media_version_0 mv_0;
		media_version_1 mv_1;
	};
	Media media;
	double rate;
	double volume;
	char reserved[10];
	char matrix[36];
	int64_t next_tid;

}MVHDBOX;




typedef struct boxtrak {
	int version;
	char flag[3];
	union Media {
		media_version_0 mv_0;
		media_version_1 mv_1;
	};
	int64_t trackid;

	

}TRAKBOX;

typedef struct boxHead {
	boxHead() {
		fullBox = false;
	}

	bool fullBox;
	int64_t boxSize;
	BoxType boxType;
}BOXHEAD;


typedef struct {
	/*	在ISO注册的4个字符。http://www.ftyps.com/	*/
	char major_brand[4];
	/*	4 bytes: version # of above (for reference and diagnostic purposes only)*/
	char minor_version[4];
	/*	兼容协议 */
	char compatible_brand[16];
}FTYPEBOX;





typedef struct MOOVBOX {
	BOXHEAD bhdr;
	MVHDBOX	mvhdBox;
}MOOVBOX;


typedef union __boxer
{
	__boxer() {}
	FTYPEBOX	ftype;
	MVHDBOX		mvhd;
		
}BOXER;


typedef struct mp4Box {
	mp4Box() {}
	BOXHEAD bhdr;
	// box  http://mp4ra.org/atoms.html 经过注册的box
	
	BOXER data;
}MP4BOX;



// 大端存储方式
bool char_to_int64(const char str[], int size,int64_t& number);
#endif // !__MP4_COMMON_H__
