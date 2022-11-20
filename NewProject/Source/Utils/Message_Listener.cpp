#include "Message_Listener.h"

Message_Listener::Message_Listener(dataloader* Dataloader) {
	this->Dataloader = Dataloader;
}


bool Message_Listener::isExist(std::string filePath) {
	ifstream f(filePath.c_str());
	return f.good();
}

void Message_Listener::addData(std::string okPath, std::string dataPath)
{
	allData.clear();
	if (!isExist(okPath)) {
		return;
	}
	ifstream f(dataPath.c_str());
	while (!f.eof()) {
		char x;
		f >> x;
		allData.add(x);
	}
	int n = allData.size();
	cout << "data size: " << n << '\n';
	if (n % 13 != 0) {
		for (int i = 0; i < n % 13; ++i) {
			allData.add(0);
		}
	}
	juce::Array<int8_t> tmp;
	for (int i = 0; i < allData.size(); ++i) {
		if (i % 13 == 0) {
			tmp.clear();
		}
		tmp.add(allData[i]);
		if ((i + 1) % 13 == 0) {
			Dataloader->add13bytes(tmp);
		}
	}
}