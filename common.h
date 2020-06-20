#pragma once
#ifndef __MP4_COMMON_H__
#define __MP4_COMMON_H__
#include <iostream>
typedef struct {
	/*	在ISO注册的4个字符。http://www.ftyps.com/	*/
	char major_brand[4];
	/*	4 bytes: version # of above (for reference and diagnostic purposes only)*/
	char minor_version[4];
	/*	兼容协议 */
	char compatible_brand[16];
}FTYPE;

typedef struct mp4Box {
	mp4Box() {
		fullBox = false;
		memset(boxSize, 0, 8);
		memset(boxType, 0, 4);
		memset(&data, 0, sizeof(boxData));
	}

	bool fullBox;
	char boxSize[8];
	char boxType[4];
	// box
	typedef union
	{
		FTYPE ftype;
	}boxData;
	boxData data;
}MP4BOX;


bool char_to_int64(const char str[], int size,int64_t& number);
#endif // !__MP4_COMMON_H__
