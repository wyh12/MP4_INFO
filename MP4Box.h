#pragma once
#include <fstream>
#include <string>
#include "common.h"


class MP4Box
{
public:
	~MP4Box();
	int MP4_Open(const std::string& path);
	int MP4_Parse_ByHeader(MP4BOX& box);
protected:
	std::ifstream fin;

private:
	int64_t getSize(std::ifstream& fp);

	int ftypeBox(FTYPEBOX& box, int64_t length);
	int mvhdBox(MVHDBOX& box, int64_t length);
	int tkhdBox(TKHDBOX& box, int64_t length);
	int edtsBox(EDTSBOX& box, int64_t length);
	int mdhdBox(MDHDBOX& box, int64_t length);
	int hdlrBox(HDLRBOX& box, int64_t length);
	int boxHead(MP4BOX& box);
};

