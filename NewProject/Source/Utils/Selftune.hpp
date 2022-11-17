#pragma once
#include <JuceHeader.h>
#include <thread>
#include "sender.h"
#include"receiver.h"
using namespace std;
using namespace juce;

namespace Selftune
{
    static int nbpf = 144; //50 byte
    static int nspb = 3; // 5 samples per bit
    static int frame_len = 144; //Len is in bit

    void send(AudioDeviceManager* dev_manager, CriticalSection* lock)
    {
        unique_ptr<Sender> se;
        se.reset(new Sender(nbpf, nspb));
        lock->enter();
        dev_manager->addAudioCallback(se.get());
        lock->exit();
        Array<int8_t> data;
        data.add(1);
        data.add(1);
        for (int i = 2; i < 40; i++)
        {
            //data.add(0);
            data.add((i + 1) % 2);
        }
        for (int i = 40; i < frame_len; i += 8)
        {
            int num = i / 8;
            for (int j = 7; j >= 0; j--)
            {
                if ((1 << j) & num)data.add(1);
                else data.add(0);
            }
        }
        cout << data.size();
        getchar();
        //se->gendebug(frame_len, data);
        while (1)
        {
            cout << "Sender: " << "Send one packet\n";
            se->sendOnePacket(frame_len, data);
            //getchar();
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        lock->enter(); dev_manager->removeAudioCallback(se.get()); lock->exit();
    }

    void receive(AudioDeviceManager* dev_manager, CriticalSection* lock)
    {
        unique_ptr<Receiver> re;
        re.reset(new Receiver(nbpf, nspb));
        lock->enter();
        dev_manager->addAudioCallback(re.get());
        lock->exit();
        re->startRecording();
        while (1)
        {
            auto fdata = re->getData();
            cout << "Receiver: " << fdata.size() << '\n';
            for (int i = 0; i < fdata.size(); i++)
            {
                cout << (int)fdata[i] << ' ';
            }
            cout << '\n';
        }
        lock->enter(); dev_manager->removeAudioCallback(re.get()); lock->exit();
    }

    void self_tune(AudioDeviceManager* dev_manager)
    {
        CriticalSection audiolock;
        thread send_thread(&send, dev_manager, &audiolock);
        thread receive_thread(&receive, dev_manager, &audiolock);


        send_thread.join();
        receive_thread.join();
    }
};
