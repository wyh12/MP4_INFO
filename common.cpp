#include "common.h"

bool char_to_int64(const char str[], int size,int64_t& number)
{
	if (str && size <= 0) {
		return false;
	}

	// ��˴洢��ʽ
	for (int i(size); i > 0; i--) {
		number += (int64_t)str[size - i] << (i - 1) * 8;
	}
	return true;
}
