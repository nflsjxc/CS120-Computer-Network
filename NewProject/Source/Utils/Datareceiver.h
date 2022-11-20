#ifndef DATARECEIVER_H
#define DATARECEIVER_H

#include <JuceHeader.h>
#include <string>
#include <fstream>
using namespace std;
using namespace juce;

class datareceiver
{
public:
	datareceiver();
	~datareceiver() {};
	CriticalSection lock;
	Array<Array<int8_t> > data;
	void add13bytes(Array<int8_t> newdata);
	Array<int8_t> get13bytes();
	void pop13bytes();
	Array<int8_t> zero13;
};

#endif