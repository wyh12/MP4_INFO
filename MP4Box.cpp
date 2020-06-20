#include "MP4Box.h"

MP4Box::~MP4Box()
{
	if (fin)
		fin.close();
}

int MP4Box::MP4_Open(const std::string path)
{
	mp4Box box_mp4;
	fin.open(path,std::ios::in | std::ios::binary);
	if (!fin.good()) {
		std::cout << "open error\n";
		return -1;
	}

//	while (1) {
		if (boxHead(box_mp4) == 0) {
			int64_t size = 0;
			char_to_int64(box_mp4.boxSize, 4, size);
			std::cout << "box type: " << (char)box_mp4.boxType[0] << (char)box_mp4.boxType[1] 
									  << (char)box_mp4.boxType[2] << (char)box_mp4.boxType[3]	
									  << " size: "<< size << std::endl;
			
			int readLen = size - (box_mp4.fullBox ? (4 + 4 + 8) : (4 + 4));			
			ftypeBox(box_mp4.data.ftype, readLen);

		}
//	}

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

int MP4Box::ftypeBox(FTYPE& box,int64_t length)
{
	//get compatible_brands
	fin.read(box.major_brand, 4);
	fin.read(box.minor_version, 4);
	fin.read(box.compatible_brand, length - 8);

	std::cout << "major_brand: " 
		<< (char)box.major_brand[0]
		<< (char)box.major_brand[1]
		<< (char)box.major_brand[2]
		<< (char)box.major_brand[3] << std::endl;
	
	
	std::cout << "minor_version:"
		<< (int)box.minor_version[0]
		<< (int)box.minor_version[1]
		<< (int)box.minor_version[2]
		<< (int)box.minor_version[3] << std::endl;
	

	std::cout << "compatible_brand: ";
	for (int i(0); i < length - 8; i++) {
		std::cout << (char)box.compatible_brand[i];
	}
	std::cout << std::endl;

	return 0;
}

int MP4Box::boxHead(mp4Box& box)
{
	int ret = 0;
	int64_t size = 0;
	// get box size 
	fin.read(box.boxSize, 4);
	if (!fin.good()) {
		std::cout << __LINE__ << "read err\n";
		ret = -1;
		goto end;
	}

	// get box type
	fin.read(box.boxType, 4);

	if (atoi(box.boxSize) == 1) {
		memset(box.boxSize, 0, 8);
		fin.read(box.boxSize, 8);		
	}
	
end:
	return 0;
}
