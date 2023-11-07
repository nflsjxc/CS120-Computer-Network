#include "Dataloader.h"

dataloader::dataloader()
{
	const ScopedLock sl(lock);
	finish = false;
	for (int i = 0; i < 13; i++)zero13.add(0);
	//data.add(zero13);
}

void dataloader::status_update(bool isfinish)
{
	finish = isfinish;
}

bool dataloader::isfinish()
{
	const ScopedLock sl(lock);
	return finish;
}

bool dataloader::isempty()
{
	const ScopedLock sl(lock);
	return data.size() == 0;
}

Array<int8_t> dataloader::getcur13bytes()
{
	const ScopedLock sl(lock);
	if (data.size() > 0)return data[0];
	else
	{
		return zero13;
	}
}

void dataloader::add13bytes(Array<int8_t> newdata)
{
	const ScopedLock sl(lock);
	data.add(newdata);
}

void dataloader::pop13bytes()
{
	const ScopedLock sl(lock);
	if (data.size() == 0)
	{
		finish = true;
		return;
	}
	data.removeRange(0, 1);
	if (data.size() == 0)
	{
		//cout << "Zeroed\n";
		finish = true;
		return;
	}
}