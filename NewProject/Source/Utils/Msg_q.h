#pragma once
#include "Dataloader.h"
#include <JuceHeader.h>
using namespace juce;
using namespace std;
class msgqueue
{
public:
	msgqueue(dataloader *dl);
	CriticalSection lock;
	void monitor();
	void push(char x);
	void flush();
	Array<int8_t> dataq;
	dataloader* dl;
	thread t;
};