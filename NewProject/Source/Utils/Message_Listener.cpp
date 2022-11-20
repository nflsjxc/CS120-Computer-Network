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
	char x;
	while (f >> x) {
		allData.add(x);
		std::cout << x << " ";
	}
	int n = allData.size();
	cout << "data size: " << n << '\n';
	if (n % 13 != 0) {
		for (int i = n % 13; i < 13; ++i) {
			allData.add(0);
		}
	}
	cout << "Update data size : " << allData.size() << "\n";
	juce::Array<int8_t> tmp;
	for (int i = 0; i < allData.size(); ++i) {
		if (i % 13 == 0) {
			tmp.clear();
		}
		tmp.add(allData[i]);
		cout << allData[i] << " ";
		if ((i + 1) % 13 == 0) {
			Dataloader->add13bytes(tmp);
		}
	}
	// Add 13 zeros to represents end.
	tmp.clear();
	for (int i = 0; i < 13; ++i) {
		tmp.add(0);
	}
	Dataloader->add13bytes(tmp); 
}