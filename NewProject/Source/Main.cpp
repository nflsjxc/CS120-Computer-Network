/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "sender.h"
#include "receiver.h"
#include "MACframe.h"
#include "Selftune.hpp"
#include "MAC.h"
#include <filesystem>
#include "Message_Listener.h"
using namespace juce;
using namespace std;

//static int nbpf = 144; //50 byte
//static int nspb = 3; // 5 samples per bit
//static int frame_len = 144; //Len is in bit
//
//void send(AudioDeviceManager* dev_manager)
//{
//    unique_ptr<Sender> se;
//    se.reset(new Sender(nbpf, nspb));
//    dev_manager->addAudioCallback(se.get());
//    Array<int8_t> data;
//    data.add(1); 
//    data.add(1);
//    for (int i = 2; i < 40; i++)
//    {
//        //data.add(0);
//        data.add((i+1)%2);
//    }
//    for (int i = 40; i < frame_len; i+=8)
//    {
//        int num = i / 8;
//        for (int j = 7; j >=0; j--)
//        {
//            if ((1 << j) & num)data.add(1);
//            else data.add(0);
//        }
//    }
//    cout << data.size();
//    getchar();
//    //se->gendebug(frame_len, data);
//    while (1)
//    {
//        cout << "Sender: "<<"Send one packet\n";
//        se->sendOnePacket(frame_len, data);
//        //getchar();
//        this_thread::sleep_for(chrono::milliseconds(100));
//    }
//    dev_manager->removeAudioCallback(se.get());
//}
//
//void receive(AudioDeviceManager* dev_manager)
//{
//    unique_ptr<Receiver> re;
//    re.reset(new Receiver(nbpf, nspb));
//    dev_manager->addAudioCallback(re.get());
//    re->startRecording();
//    while (1)
//    {
//        auto fdata=re->getData();
//        cout << "Receiver: " << fdata.size()<<'\n';
//        for (int i = 0; i < fdata.size(); i++)
//        {
//            cout << (int)fdata[i] << ' ';
//        }
//        cout << '\n';
//    }
//    dev_manager->removeAudioCallback(re.get()); 
//}

void mac_init(MAC* test)
{
    test->main_thread();
}

//==============================================================================
int main (int argc, char* argv[])
{
    //ios::sync_with_stdio(0);

    AudioDeviceManager dev_manager;
    dev_manager.initialiseWithDefaultDevices(1, 1);
    AudioDeviceManager::AudioDeviceSetup dev_info;
    dev_info = dev_manager.getAudioDeviceSetup();
    dev_info.sampleRate = 48000; // Setup sample rate to 48000 Hz
    dev_info.bufferSize = 512;
    dev_manager.setAudioDeviceSetup(dev_info, false);

    string datap = R"(C:\Users\Cabbage\Desktop\Network-debug\Data\input_t.txt)";
    string okp= R"(C:\Users\Cabbage\Desktop\Network-debug\Data\ok.txt)";
    //filesystem::path datapath(datap);
    //dataloader load(datapath.string());

    //CriticalSection audiolock;
    //Selftune::self_tune(&dev_manager);
    getchar();

    dataloader datal;
    datareceiver datar;
    
    Message_Listener msgl(&datal);
    msgl.addData(okp, datap);
    MAC test(&dev_manager);
    test.dl = &datal; test.dr = &datar;

    thread mac_thread(mac_init, &test);
    for (;;)
    {
        //this_thread::sleep_for(chrono::milliseconds(50));
        //cout << datal.isfinish() << '\n';
        if (datal.isfinish())
        {
            test.update_status(1);
            break;
        }
    }
    mac_thread.join();

    //for (int i = 0; i < datar.data.size(); i++)
    //{
    //    for (int j = 0; j < datar.data[i].size(); j++)
    //    {
    //        cout << datar.data[i][j] << ' ';
    //    }
    //    cout << '\n';
    //}
    

    /*thread send_thread(&send, &dev_manager);
    thread receive_thread(&receive, &dev_manager);
    

    send_thread.join();
    receive_thread.join();*/
    
    //Array<int8_t> datatmp;
    //for (int i = 0; i < 13; i++)
    //{
    //    datatmp.add((int8_t)i);
    //}
    //MACframe dataframe(DST_ADDR, SRC_ADDR, datatmp);
    //cout << dataframe.data.size();

    DeletedAtShutdown::deleteAll();
    juce::MessageManager::deleteInstance();
    return 0;
}
