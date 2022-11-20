#pragma once
#include <JuceHeader.h>
#include "Settings.h"
#include "sender.h"
#include "receiver.h"
#include "MACframe.h"
#include "Dataloader.h"
#include "Datareceiver.h"

using namespace juce;
using namespace std;
//extern dataloader;

class MAC
{
public:
	MAC(AudioDeviceManager* dev_manager);
	~MAC();
	void main_thread();
	void update_status(bool newstatus);
	void send();
	void receive();
	dataloader* dl;
	datareceiver* dr;
private:
	int nbpf = 144; //50 byte
	int nspb = 3; // 5 samples per bit
	int frame_len = 144; //Len is in bit
	AudioDeviceManager* dev_manager;
	CriticalSection rslock,sblock,rblock;
	unique_ptr<Sender> se;
	unique_ptr<Receiver> re;
	Array<MACframe>send_buffer,receive_buffer;
	thread send_thread, receive_thread;
	bool stop_flag;
	bool main_flag;
};
