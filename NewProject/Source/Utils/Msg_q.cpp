#include "Msg_q.h"

msgqueue::msgqueue(dataloader* dl)
{
	this->dl = dl;
}

void msgqueue::monitor()
{

}

void msgqueue::push(char x)
{
	const ScopedLock sl(lock);
	dataq.add((int8_t)x);
}