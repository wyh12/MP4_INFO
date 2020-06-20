#pragma once
#include <fstream>
#include <string>
#include "common.h"


class MP4Box
{
public:
	~MP4Box();
	int MP4_Open(const std::string path);
protected:
	std::ifstream fin;

private:
	int64_t getSize(std::ifstream& fp);

	int ftypeBox(FTYPE& box, int64_t length);
	int boxHead(mp4Box& box);
};

