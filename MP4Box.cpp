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
		//MP4BOX local_box;
		//// 递归moov中的box
		//while (boxHead(local_box) == 0) {
		//	MP4_Parse_ByHeader(local_box);
		//}
	}
	else if (box.bhdr.boxType == MVHD) {
		std::cout << "box type " << "MVHD" << " size " << box.bhdr.boxSize << std::endl;
		mvhdBox(box.data.mvhd, readLen);
	}
	else if (box.bhdr.boxType == TRAK) {
		std::cout << "box type " << "TRAK" << " size " << box.bhdr.boxSize << std::endl;
	}
	else if (box.bhdr.boxType == TKHD) {
		std::cout << "box type " << "TKHD" << " size " << box.bhdr.boxSize << std::endl;
		tkhdBox(box.data.tkhd,readLen);
	}
	else if (box.bhdr.boxType == EDTS) {
		std::cout << "box type " << "EDTS" << " size " << box.bhdr.boxSize << std::endl;
		edtsBox(box.data.edts, readLen);
	}
	else if (box.bhdr.boxType == MDIA) {
		std::cout << "box type " << "MDIA" << " size " << box.bhdr.boxSize << std::endl;
	}
	else if (box.bhdr.boxType == MDHD) {
		std::cout << "box type " << "MDHD" << " size " << box.bhdr.boxSize << std::endl;
		mdhdBox(box.data.mdhd, readLen);
	}
	else if (box.bhdr.boxType == HDLR) {
		std::cout << "box type " << "HDLR" << " size " << box.bhdr.boxSize << std::endl;
		hdlrBox(box.data.hdlr, readLen);
	}
	else if (box.bhdr.boxType == MINF) {
		std::cout << "box type " << "MINF" << " size " << box.bhdr.boxSize << std::endl;
	}
	else if (box.bhdr.boxType == VMHD) {
		std::cout << "box type " << "VMHD" << " size " << box.bhdr.boxSize << std::endl;
		vmhdBox(box.data.vmhd, readLen);
	}
	else if (box.bhdr.boxType == DINF) {
		std::cout << "box type " << "DINF" << " size " << box.bhdr.boxSize << std::endl;
		drefBox(box.data.dref, readLen);
	}
	else if (box.bhdr.boxType == STBL) {
		std::cout << "box type " << "STBL" << " size " << box.bhdr.boxSize << std::endl;
		
	}
	else if (box.bhdr.boxType == STSD) {
		std::cout << "box type " << "STSD" << " size " << box.bhdr.boxSize << std::endl;
		avc1Box(box.data.avc1, readLen);
	}
	else if (box.bhdr.boxType == STTS) {
		std::cout << "box type " << "STTS" << " size " << box.bhdr.boxSize << std::endl;
		sttsBox(box.data.stts, readLen);
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
	delete[] data;
	return 0;
}

int MP4Box::tkhdBox(TKHDBOX& box, int64_t length)
{
	char* data = new char[length];
	fin.read(data, length);

	int offset = 0;
	box.version = data[offset];
	if (box.version == 0) {

		// flag标志位 
		memcpy(box.flag, data + 1, 3);
		offset += 4; 

		// create time
		char_to_int64(data + offset, 4, box.media.mv_0.create_time);
		offset += 4;

		// modfiy time
		char_to_int64(data + offset, 4, box.media.mv_0.modify_time);
		offset += 4;

		// trakid id不能重复且不为0
		char_to_int64(data + offset, 4, box.trackid);
		offset += 4;

		// reserved  保留位 4 Byte
		offset += 4;

		// track 时长
		char_to_int64(data + offset, 4, box.media.mv_0.duration);
		offset += 4;

		// reserved  保留位 8 Byte
		offset += 8;
	}
	else {
		//未实现，，，
	}

	// layer 视频层，默认为0，值小的在上层
	int64_t temp;
	char_to_int64(data + offset, 2, temp);
	box.layer = temp;
	offset += 2;

	// track分组信息，默认为0表示该track未与其他track有群组关系
	char_to_int64(data + offset, 2, temp);
	box.alternate_group = temp;
	offset += 2;

	// volume
	box.volume = *(data + offset);
	box.volume += *(data + offset + 1) / 1000.0;
	offset += 2;

	// 保留位 2 byte
	offset += 2;

	//matrix 视频变换矩阵
	offset += 36;

	// w,h
	char_to_int64(data + offset, 2, temp);
	offset += 2;
	box.width = temp;
	char_to_int64(data + offset, 2, temp);
	box.width += temp / 100000.0;
	offset += 2;

	char_to_int64(data + offset, 2, temp);
	offset += 2;
	box.height = temp;
	char_to_int64(data + offset, 2, temp);
	box.height += temp / 100000.0;
	offset += 2;

	
	delete[] data;
	return 0;
}

int MP4Box::mdhdBox(MDHDBOX& box, int64_t length)
{
	char* data = new char[length];
	fin.read(data, length);

	int offset = 0;
	box.version = data[offset];
	if (box.version == 0) {
		// flag标志位 
		memcpy(box.flag, data + 1, 3);
		offset += 4;

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
		
		// 语言码 ISO_639-2 首位为0，剩余的5位一组，从字母顺序表取值。https://baike.baidu.com/item/ISO_639-2/9935792?fr=aladdin
		box.language_code[0] = *(data + offset);
		box.language_code[1] = *(data + offset + 1);
		offset += 2;

		// prd_defined
		box.pre_defined[0] = *(data + offset);
		box.pre_defined[1] = *(data + offset + 1);
	}
	else {
		std::cout << "error not support box version " << box.version << std::endl;
	}

	delete[] data;
	return 0;
}

int MP4Box::hdlrBox(HDLRBOX& box, int64_t length)
{
	char* data = new char[length];
	fin.read(data, length);

	int offset = 0;
	box.version = data[offset];
	if (box.version == 0) {
		// flag标志位 
		memcpy(box.flag, data + 1, 3);
		offset += 4;

		// pre-defined 4 byte
		offset += 4;

		//handler type
		memcpy(box.handle_type, data + offset, 4);
		offset += 4;

		// reserved 12
		offset += 12;

		// name 
		std::cout << "name " << (data + offset) << std::endl;

	}
	else {
		std::cout << "error not support box version " << box.version << std::endl;
	}

	delete[] data;
	return 0;
}

int MP4Box::vmhdBox(VMHDBOX& box, int64_t length)
{
	// 暂时解析，跳过。
	std::streampos pos = fin.tellg();
	pos += length;
	fin.seekg(pos);
	return 0;
}

int MP4Box::drefBox(DREFBOX& box, int64_t length)
{
	// 百度结果 包含有url字段，绿色为box flag，值为1，说明“url”中的字符串为空，表示track数据已包含在文件中。
	// 猜测url字段也为 version ，flag结构，flag = 1。
	char* data = new char[length];
	fin.read(data, length);

	int offset = 0;
	box.version = data[offset];
	if (box.version == 0) {	
		// flag标志位 
		memcpy(box.flag, data + 1, 3);
		offset += 4;

		// 
		char_to_int64(data + offset, 4, box.url_count);
		offset += 4;

		// 具体url暂不解析
	}

	delete[] data;
	return 0;
}

int MP4Box::edtsBox(EDTSBOX& box, int64_t length)
{
	// 暂时不用解析，跳过。内部还包含有elst box
	std::streampos pos = fin.tellg();
	pos += length;
	fin.seekg(pos);
	return 0;
}

int MP4Box::avc1Box(AVC1BOX& box, int64_t length)
{
	char* data = new char[length];
	fin.read(data, length);

	int offset = 16;
	
	//reserved 6 byte
	offset += 6;

	// data_reference_index
	int64_t temp;
	char_to_int64(data + offset, 2, temp);
	box.data_reference_index = temp;
	offset += 2;

	// pred_defined
	offset += 2;

	//reserved
	offset += 2;

	// pre_defined 3*4
	memcpy(box.predefined, data + offset, 12);
	offset += 12;

	//width
	char_to_int64(data + offset, 2, temp);
	box.width = temp;
	offset += 2;

	//height	
	char_to_int64(data + offset, 2, temp);
	box.height = temp;
	offset += 2;

	// horiz_res
	char_to_int64(data + offset, 4, box.horiz_res);
	offset += 4;

	// vert_res
	char_to_int64(data + offset, 4, box.vert_res);
	offset += 4;

	// reserver 4 byte
	offset += 4;

	// frame count 
	char_to_int64(data + offset, 2, temp);
	box.frame_count = temp;
	offset += 2;

	// compressr_name
	memcpy(box.compress_name, data + offset, 32);
	offset += 32;

	// bit_depth
	char_to_int64(data + offset, 2, temp);
	box.bit_depth = temp;
	offset += 2;

	// pre_define
	char_to_int64(data + offset, 2, temp);
	box.pre_define = temp;
	offset += 2;

	// 跳过avcC头
	offset += 8;

	// version
	box.avcC.version				= *(data + offset);
	box.avcC.avcProfileIndication	= *(data + offset + 1);
	box.avcC.profile_compatibility	= *(data + offset + 2);
	box.avcC.avcLevelIndication		= *(data + offset + 3);
	box.avcC.nalu_len				= *(data + offset + 4);
	box.avcC.sps_num				= (*(data + offset + 5)) & 0x1F;
	offset += 6;

	// sps size
	char_to_int64(data + offset, 2, temp);
	box.avcC.sps_size = temp;
	offset += 2;
	// copy sps
	memcpy(box.avcC.sps, data + offset, box.avcC.sps_size);
	offset += box.avcC.sps_size;



	// pps num 
	box.avcC.pps_num = (*(data + offset)) & 0x1F;
	offset += 1;
	// pps size 
	char_to_int64(data + offset, 2, temp);
	box.avcC.pps_size = temp;
	offset += 2;

	// copy pps
	memcpy(box.avcC.pps, data + offset, box.avcC.pps_size);
	offset += box.avcC.pps_size;

	delete[] data;
	return 0;
}

int MP4Box::sttsBox(STTSBOX& box, int64_t length)
{
	char* data = new char[length];
	fin.read(data, length);
	int offset = 0;
	box.version = data[offset];
	if (box.version == 0) {
		// flag标志位 
		memcpy(box.flag, data + 1, 3);
		offset += 4;

		// sample count 
		char_to_int64(data + offset, 4, box.time_to_sample);
		offset += 4;

		box.ts = new std::vector<boxts>;
		for (int i = 0; i < box.time_to_sample; i++) {
			boxts stamp;
			char_to_int64(data + offset, 4, stamp.sample_count);
			offset += 4;
			char_to_int64(data + offset, 4, stamp.sample_duration);
			offset += 4;
			box.ts->push_back(stamp);
		}		
	}
	else {
		
	}

	delete[] data;
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
