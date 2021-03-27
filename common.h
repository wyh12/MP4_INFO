#pragma once
#ifndef __MP4_COMMON_H__
#define __MP4_COMMON_H__
#include <iostream>
#include <vector>

//c11 新增特性，显示声明enum大小
enum  BoxType:int64_t{
	FTYP     = 0x66747970,
	MOOV	 = 0x6D6F6F76,
	MVHD	 = 0x6D766864,			//This box defines overall information which is media-independent, and relevant to the entire presentation
	TRAK	 = 0x7472616b,
	TKHD	 = 0x746b6864,
	EDTS	 = 0x65647473,
	MDIA	 = 0x6d646961,
	MDHD	 = 0x6d646864,
	HDLR	 = 0x68646c72,
	MINF	 = 0x6d696e66,
	VMHD	 = 0x766d6864,
	DINF	 = 0x64696e66,
	DREF	 = 0x64726566,
	STBL	 = 0x7374626C,
	STSD	 = 0x73747364,
	STTS	 = 0x73747473,
	STSS	 = 0x73747373,
	CTTS	 = 0x63747473,
	STSC	 = 0x73747363,
	STSZ	 = 0x7374737a,
	STCO	 = 0x7374636F,
	MDAT	 = 0x6d646174,
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


union Media {
	media_version_0 mv_0;
	media_version_1 mv_1;
};

typedef struct mvhdbox {
	int version;
	Media media;
	double rate;
	double volume;
	char reserved[10];
	char matrix[36];
	int64_t next_tid;

}MVHDBOX;



// tkhd 描述的该track的，如果是视频会有宽、高信息、 还有文件创建时间、修改时间等。
typedef struct boxtkhd {
	int version;
/*	
	0x000001 track_enabled，否则该track不被播放；
	0x000002 track_in_movie，表示该track在播放中被引用；
	0x000004 track_in_preview，表示该track在预览时被引用。	
	一般该值为7，如果一个媒体所有track均未设置track_in_movie和track_in_preview，将被理解为所有track均设置了这两项；
	对于hint track，该值为0。
*/
	char flag[3];
	Media media;
	int64_t trackid;
	// 可替换 track 源。如果为 0 表示当前 track 没有指定的 track 源替代。
	// 非 0 的话，则表示存在多个源的 group
	int16_t alternate_group;
	int16_t layer;
	double volume;
	double width;
	double height;
	
}TKHDBOX;



typedef struct boxedts {
	
}EDTSBOX;


// 当前音/视频轨/流(trak)的总体信息, 该box中有duration字段和timescale字段，duration/timescale的值即为当前流的时长。
// 这里面也有时间基,与mvhd不知道有什么不同
typedef struct boxmdhd {
	int version;
	char flag[3];
	Media media;
	char language_code[2];
	char pre_defined[2];

}MDHDBOX;

typedef struct boxhdlr {
	int version;
	char flag[3];
	char handle_type[4];

}HDLRBOX;

typedef struct boxvmhd {
	int version;
	char flag[3];
	int64_t graphics_mode;
	
}VMHDBOX;

typedef struct boxsmhd {
	int version;
	char flag[3];
	double balance;
}SMHDBOX;


typedef struct boxstsd {
	int version;
	char flag[3];
	int64_t sample_dess;

}STSDBOX;

typedef struct boxavcC {
	int8_t version;
	int8_t avcProfileIndication;
	int8_t profile_compatibility;
	int8_t avcLevelIndication;
	int8_t nalu_len; // 没懂
	int8_t sps_num; // 低5位有效
	int16_t sps_size; // 应该是总长度
	char sps[64];  //大部分应该够用了

	int8_t pps_num;
	int16_t pps_size;
	char pps[64];

}AVCCBOX;

typedef struct boxavc1 {
	int16_t data_reference_index;
	char predefined[3*4];
	int16_t		width;
	int16_t		height;
	int64_t		horiz_res;
	int64_t		vert_res;

	int16_t		frame_count;
	char		compress_name[32];
	int16_t		bit_depth;
	int16_t		pre_define;
	AVCCBOX  avcC;
}AVC1BOX;

struct boxts {
	int64_t sample_count;
	int64_t sample_duration;
};

//stts  (time to stamp)采样时戳因映射表box
// boxts 里 sample_duration * sample_count / (mdhd)timescale = 视频时长
typedef struct boxstts {
	int version;
	char flag[3];
	int64_t time_to_sample;
	std::vector<boxts> *ts;
}STTSBOX;


// sync sample table 同步表
// 这里面存储有关键帧序号（I帧）。猜测为IDR帧
typedef struct boxstss {
	int version;
	char flag[3];
	int64_t entry_count;
	std::vector<int32_t>* sample_num;
}STSSBOX;

//Composition Time to Sample Box
//ctts 有每个sample的构成时间(Composition Time)和解码时间(DTS)之间的差值(CTTS)即图中的composition_offset。
//如果不存在ctts，则代表该流不存在B帧，那么PTS就直接等于DTS。
typedef struct boxctts {
	int version;
	char flag[3];
	int64_t entry_count;
	std::vector<boxts>* ts;

}CTTSBOX;


struct chunk {
	int64_t first_chunk;
	int64_t sample_per_chunk;
	int64_t sample_description_index;
};

// Sample To Chunk Box
// 媒体数据的样本是被打包进chunks(块)的,chunks和样本(samples)的大小不固定，该box用于说明chunks关联样本的信息。
// first_chunk 该入口第一个chunks的索引(index).
// samples_per_chunk 样本数量 / chunks
typedef struct boxstst {
	int version;
	char flag[3];
	int64_t entry_count;
	std::vector<chunk> *chunks;
}STSCBOX;

// sample size boxes 样本大小
typedef struct boxstsz {
	int version;
	char flag[3];

	int64_t sample_size_;
	int64_t sample_count;
	std::vector<int64_t> *size;
}STSZBOX;

//chunk offset boox 每个chunks相对文件的偏移量。
typedef struct stcobox {
	int version;
	char flag[3];

	int64_t entry_count;
	std::vector<int64_t>* offset;
}STCOBOX;

typedef struct boxdref {
	int version;
	char flag[3];
	int64_t url_count;
}DREFBOX;

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
	TKHDBOX tkhd;
	

}MOOVBOX;


typedef union __boxer
{
	__boxer() {}
	FTYPEBOX	ftype;
	MVHDBOX		mvhd;
	TKHDBOX		tkhd;
	EDTSBOX		edts;
	MDHDBOX		mdhd;
	HDLRBOX		hdlr;
	VMHDBOX		vmhd;
	DREFBOX		dref;
	AVC1BOX		avc1;
	STTSBOX		stts;
	STSSBOX		stss;
	CTTSBOX		ctts;
	STSCBOX		stsc;
	STSZBOX		stsz;
	STCOBOX		stco;
}BOXER;


typedef struct mp4Box {
	mp4Box() {}
	BOXHEAD bhdr;
	// box  http://mp4ra.org/atoms.html 经过注册的box
	
	BOXER data;
}MP4BOX;



// 大端存储方式
inline bool char_to_int64(const char str[], int size, int64_t& number) {
	if (str && size <= 0) {
		return false;
	}
	number = 0;
	// 大端存储方式
	for (int i(size); i > 0; i--) {
		uint8_t value = str[size - i];
		number += (int64_t)value << (i - 1) * 8;
	}
	return true;
}
#endif // !__MP4_COMMON_H__
