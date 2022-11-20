#pragma once

#include <JuceHeader.h>
#include <vector>
#include<fstream>
#include "Settings.h"
using namespace juce;



class Receiver : public AudioIODeviceCallback, private HighResolutionTimer
{
public:
    //Receiver();


    Receiver(int packlen, int bitlen);
    ~Receiver();

    void GenerateCarrierWave();

    void GenerateHeader();

    void audioDeviceAboutToStart(AudioIODevice* device) override;

    void audioDeviceStopped() override {}

    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
        float** outputChannelData, int numOutputChannels, int numSamples) override;

    void startRecording();

    void stopRecording();

    Array<int8_t> Int2Byte(Array<int>& int_data);

    int Demodulate(float sample);

    Array<int8_t> getData();

    void clearFrameData() { frame_data.clear(); }

    bool isRecording;

    void hiResTimerCallback() override;

    float getChannelPower() { return power_; }

    float getMaxPower() { return max_power; }

    bool timeout_flag;
private:

    Array<float> processingHeader;
    Array<float> processingData;
    Array<float> syncHeader;
    Array<float> carrierWave;
    Array<int8_t> frame_data;

    Array<float> recordedSound;
    Array<float> demodulate_buffer;

    CriticalSection lock;

    int recordedSampleNum = -1;
    int bitLen; //the length of one bit
    int packLen; // how many bits per frame
    int headerLength;
    int sampleRate;
    float syncPower_localMax;
    float power_ = 0.0f;
    int state; // processing state
    int data_state;
    std::ofstream fout;
    std::ofstream of;
    std::ofstream powerf;
    bool _ifheadercheck = false;
    Array<float> tempBuffer;
    Array<int> int_data;
    float max_power = 0.0f;

    bool is_short_packet = false;
};