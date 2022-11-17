#pragma once
#include <JuceHeader.h>

class CRC8 {
public:
	CRC8();
	int8_t getCRC() {
		return crc;
	};
	void resteCRC() {
		crc = init;
	};
	void updateCRC(int8_t data);
	~CRC8() {};
private:
	int8_t poly = 0x8c;
	int8_t init = 0x00;
	std::vector<int8_t> crcTable;
	int8_t crc;
};