#include "CRC8.h"

CRC8::CRC8() {
	crc = init;
	for (int i = 0; i <= 0xff; ++i) {
		int remainder = i & 0xff;
		for (int bit = 0; bit < 8; ++bit) {
			if (remainder & 0x01) {
				remainder = (remainder >> 1) ^ poly;
			}
			else {
				remainder >>= 1;
			}
		}
		crcTable.push_back(remainder);
	}
}

void CRC8::updateCRC(int8_t data) {
	data ^= crc;
	crc = (int8_t)(crcTable[data & 0xff] ^ (crc << 8));
}