#include "Datareceiver.h"
#include <fstream>

Array<int8_t> datareceiver::get13bytes()
{
	const ScopedLock sl(lock);
	if (data.size() == 0)return zero13;
	return data[0];
}

void datareceiver::pop13bytes()
{
	const ScopedLock sl(lock);
	if (data.size() == 0)return;
	data.removeRange(0, 1);
}
Array<int8_t> zero13;

datareceiver::datareceiver()
{
	for (int i = 0; i < 13; i++)zero13.add(0);
}

void datareceiver::add13bytes(Array<int8_t> newdata)
{
	const ScopedLock sl(lock);
	data.add(newdata);
}

bool datareceiver::isfinish() {
	// If a package received, it should end with 13 zeros.
	const ScopedLock sl(lock);
	int n = data.size() - 1;
	if (n < 0) {
		return false;
	}
	for (int i = 0; i < 13; ++i) {
		if (data[n][i] != 0) {
			return false;
		}
	}
	return true;
}

bool datareceiver::isempty()
{
	const ScopedLock sl(lock);
	return data.size() == 0;
}

void datareceiver::writeToFiles(std::string path, bool isSymbol) {
	ofstream f(path);
	if (!isSymbol) {
		for (int i = 0; i < data.size(); ++i) {
			for (int j = 0; j < data[0].size(); ++j) {
				f << ((char)data[i][j]);
			}
		}
	}
	else {
		f << "1";
	}
}