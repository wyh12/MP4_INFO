#include "common.h"

bool char_to_int64(const char str[], int size,int64_t& number)
{
	if (str && size <= 0) {
		return false;
	}
	number = 0;
	// ´ó¶Ë´æ´¢·½Ê½
	for (int i(size); i > 0; i--) {
		uint8_t value = str[size - i];
		number += (int64_t)value << (i - 1) * 8;
	}
	return true;
}
