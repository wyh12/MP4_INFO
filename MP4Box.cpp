#include "MP4Box.h"
#include <vector>

MP4Box::~MP4Box()
{
	if (fin)
		fin.close();
}

int MP4Box::MP4_Open(const std::string& path)
{

	std::vector<MP4BOX> boxer;
	
	MP4BOX box_mp4;
	fin.open(path,std::ios::in | std::ios::binary);
	if (!fin.good()) {
		std::cout << "open error\n";
		return -1;
	}

	while (1) {
		MP4BOX box;
		int64_t size = 0;
		if (boxHead(box_mp4) == 0) {
			MP4_Parse_ByHeader(box_mp4);
		}
		else {
			std::cout << "error parse ...\n";
		}
	}

	//moov
	//MP4BOX box_moov;
	//boxHead(box_moov);	
	//size = 0;
	//char_to_int64(box_moov.bhdr.boxSize, box_moov.bhdr.fullBox ? 8 : 4, size);
	//std::cout << "box type: " << (char)box_moov.bhdr.boxType[0] << (char)box_moov.bhdr.boxType[1]
	//	<< (char)box_moov.bhdr.boxType[2] << (char)box_moov.bhdr.boxType[3]
	//	<< " size: " << size << std::endl;

	////mvhd https://www.cnblogs.com/ranson7zop/p/7889272.html
	//MP4BOX box_mvhd;
	//boxHead(box_mvhd);
	//size = 0;
	//char_to_int64(box_mvhd.bhdr.boxSize, box_mvhd.bhdr.fullBox ? 8 : 4, size);
	//std::cout << "box type: " << (char)box_mvhd.bhdr.boxType[0] << (char)box_mvhd.bhdr.boxType[1]
	//	<< (char)box_mvhd.bhdr.boxType[2] << (char)box_mvhd.bhdr.boxType[3]
	//	<< " size: " << size << std::endl;

	return 0;
}

int MP4Box::MP4_Parse_ByHeader(MP4BOX& box)
{
	int64_t readLen = box.bhdr.boxSize - (box.bhdr.fullBox ? (4 + 4 + 8) : (4 + 4));
	if (box.bhdr.boxType == FTYP) {
		std::cout << "box type " << "FTYP" << " size " << box.bhdr.boxSize << std::endl;
		ftypeBox(box.data.ftype, readLen);
	}
	else if (box.bhdr.boxType == MOOV) {
		std::cout << "box type " << "MOOV" << " size " << box.bhdr.boxSize << std::endl;
		MP4BOX local_box;
		// 递归moov中的box
		while (boxHead(local_box) == 0) {
			MP4_Parse_ByHeader(local_box);
		}
	}
	else if (box.bhdr.boxType == MVHD) {
		std::cout << "box type " << "MVHD" << " size " << box.bhdr.boxSize << std::endl;
		mvhdBox(box.data.mvhd, readLen);
	}
	else {
		std::cout << "unknow box type " << std::hex << box.bhdr.boxType << std::endl;
		return -1;
	}
	
	return 0;
}



int64_t MP4Box::getSize(std::ifstream& fp)
{
	int64_t boxSize = 0;
	std::streampos position, head, end;
	position= fp.tellg();
	
	fp.seekg(0,std::ios_base::end);
	boxSize = fp.tellg();
	fp.seekg(position);
	return boxSize;
}


int MP4Box::ftypeBox(FTYPEBOX& box,int64_t length)
{
	//get compatible_brands
	fin.read(box.major_brand, 4);
	fin.read(box.minor_version, 4);
	fin.read(box.compatible_brand, length - 8);

	std::cout << "	major_brand: " 
		<< (char)box.major_brand[0]
		<< (char)box.major_brand[1]
		<< (char)box.major_brand[2]
		<< (char)box.major_brand[3] << " ";
	
	
	std::cout << "minor_version:"
		<< (int)box.minor_version[0]
		<< (int)box.minor_version[1]
		<< (int)box.minor_version[2]
		<< (int)box.minor_version[3] << " ";
	

	std::cout << "compatible_brand: ";
	for (int i(0); i < length - 8; i++) {
		std::cout << (char)box.compatible_brand[i];
	}
	std::cout << std::endl;

	return 0;
}

int MP4Box::mvhdBox(MVHDBOX& box, int64_t length)
{
	char* data = new char[length];
	fin.read(data, length);
//	fin.flags & std::ios::failbit
	int offset = 0;
	box.version = data[offset];
	if (box.version == 0) {
		offset += 4; // flag标志位略过
		// create time
		char_to_int64(data + offset, 4, box.media.mv_0.create_time);
		offset += 4;

		// modfiy time
		char_to_int64(data + offset, 4, box.media.mv_0.modify_time);
		offset += 4;

		// time scale 时间尺度
		char_to_int64(data + offset, 4, box.media.mv_0.timescale);
		offset += 4;

		// track 时长
		char_to_int64(data + offset, 4, box.media.mv_0.duration);
		offset += 4;
	}
	else {
		//未实现，，，
	}


	// rate 播放速率
	int64_t rate;
	char_to_int64(data + offset, 2, rate);
	offset += 2;
	box.rate = rate;
	char_to_int64(data + offset, 2, rate);
	box.rate += rate / 100000.0;
	offset += 2;


	//volume
	box.volume = *(data + offset);
	box.volume += *(data + offset + 1) / 1000.0;
	offset += 2;

	// 保留位
	memcpy(box.reserved, data + offset, 10);
	offset += 10;

	//视频变换矩阵
	memcpy(box.matrix, data + offset, 36);
	offset += 36;

	// prd-defined 24 跳过
	offset += 24;

	// next track id
	char_to_int64(data + offset, 4, box.next_tid);

	return 0;
}



int MP4Box::boxHead(MP4BOX& box)
{
	int ret = 0;
	char boxSize[8] = {};
	char type[8] = {};
	
	// get box size 
	fin.read(boxSize, 4);
	if (!fin.good()) {
		std::cout << __LINE__ << "read err\n";
		ret = -1;
		goto end;
	}

	// get box type
	fin.read(type, 4);
	
	char_to_int64(type, 4,(int64_t&)(box.bhdr.boxType));
	char_to_int64(boxSize,4,box.bhdr.boxSize);


	if (box.bhdr.boxSize == 1) { // full box
		memset(boxSize, 0, 8);
		fin.read(boxSize, 8);
		if (!fin.good()) {
			std::cout << __LINE__ << "read err\n";
			ret = -1;
			goto end;
		}
		char_to_int64(boxSize, 4, box.bhdr.boxSize);

	}
	else if (box.bhdr.boxSize == 0) { 
	
	}
	
end:
	return ret;
}
