#pragma once
#ifndef __MP4_COMMON_H__
#define __MP4_COMMON_H__
#include <iostream>

//c11 �������ԣ���ʾ����enum��С
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
	NONETYPE = 0x00,
};

typedef struct  {
	int64_t	create_time;
	int64_t	modify_time;
	int64_t	timescale;
	int64_t	duration;
}media_version_0;


// �д��о�
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




typedef struct boxtkhd {
	int version;
/*	
	0x000001 track_enabled�������track�������ţ�
	0x000002 track_in_movie����ʾ��track�ڲ����б����ã�
	0x000004 track_in_preview����ʾ��track��Ԥ��ʱ�����á�	
	һ���ֵΪ7�����һ��ý������track��δ����track_in_movie��track_in_preview���������Ϊ����track�������������
	����hint track����ֵΪ0��
*/
	char flag[3];
	Media media;
	int64_t trackid;
	int16_t alternate_group;
	int16_t layer;
	double volume;
	double width;
	double height;
	
}TKHDBOX;



typedef struct boxedts {
	
}EDTSBOX;


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
	int8_t nalu_len; // û��
	int8_t sps_num; // ��5λ��Ч
	int16_t sps_size; // Ӧ�����ܳ���
	char sps[64];  //�󲿷�Ӧ�ù�����

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
	/*	��ISOע���4���ַ���http://www.ftyps.com/	*/
	char major_brand[4];
	/*	4 bytes: version # of above (for reference and diagnostic purposes only)*/
	char minor_version[4];
	/*	����Э�� */
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
}BOXER;


typedef struct mp4Box {
	mp4Box() {}
	BOXHEAD bhdr;
	// box  http://mp4ra.org/atoms.html ����ע���box
	
	BOXER data;
}MP4BOX;



// ��˴洢��ʽ
bool char_to_int64(const char str[], int size,int64_t& number);
#endif // !__MP4_COMMON_H__
