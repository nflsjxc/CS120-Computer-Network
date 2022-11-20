#include "Datareceiver.h"

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

bool datareceiver::receiveAll() {
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