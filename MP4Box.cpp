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
		// µÝ¹émoovÖÐµÄbox
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
