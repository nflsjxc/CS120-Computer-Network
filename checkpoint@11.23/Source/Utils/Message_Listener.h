#ifndef MESSAGE_LISTENER_H
#define MESSAGE_LISTENER_H

#include <iostream>
#include <fstream>
#include "Datareceiver.h"
#include <JuceHeader.h>
#include "Dataloader.h"
#include <map>

class Message_Listener {
public:
	Message_Listener(dataloader* Dataloader);

	void addData(std::string okPath, std::string dataPath);

	void addIcmpRequest();

	void addEndRequest();

	bool isExist(std::string filePath);

private:
	dataloader* Dataloader;
	juce::Array<int8_t> allData;
	std::map<std::string, int> visPath;
};




#endif