//#pragma once

#ifndef DATALOADER_H
#define DATALOADER_H

#include <JuceHeader.h>
#include <string>
#include <fstream>
using namespace juce;

class dataloader
{
public:
	//dataloader(string path);
	dataloader();
	~dataloader() {};
	Array<int8_t> getcur13bytes();
	void pop13bytes();
	void dataloader::add13bytes(Array<int8_t> newdata);
	bool isfinish();
	bool isempty();
	void status_update(bool x);
private:
	CriticalSection lock;
	Array<Array<int8_t> > data;
	Array<int8_t> zero13;
	int pos;
	bool finish;
};

#endif