#include "sender.h"
#include <fstream>
#include "Settings.h"
#include "JuceHeader.h"
using namespace juce;

Sender::~Sender()
{
    delete[]zeros;
    delete[]frame_wave;
    delete[]header_wave;
    delete[]startup_wave;
}

Sender::Sender(int nbpf, int nspb) {

    num_bits_per_frame = nbpf;
    num_samples_per_bit = nspb;
    len_warm_up = 480;
    //header_len = 120;
    header_len = HEADER_LEN;
    sample_rate = 48000;
    carrier_freq = 5000;
    carrier_phase = 0;
    carrier_amp = 1;
    //len_zeros = 20;
    len_zeros = 20;
    num_frame = 30;
    len_frame = header_len + num_samples_per_bit * num_bits_per_frame + len_zeros;
    GenerateCarrierWave();
    frame_wave = new float[num_bits_per_frame * num_samples_per_bit];
    header_wave = new float[header_len];
    zeros = new float[len_zeros];
    for (int i = 0; i < len_zeros; i++)zeros[i] = 0;
    generateHeader();
    isPlaying = false;
    startSend();
}

void Sender::setHeaderLen(int len) {
    header_len = len;
}

void Sender::setCarrierFreq(int freq) {
    carrier_freq = freq;
}

void Sender::GenerateCarrierWave() {
    for (int j = 0; j < num_samples_per_bit; j++)
    {
        carrier_wave.add(carrier_amp * sin(j * 2 * PI * ((float)carrier_freq / (float)sample_rate) + carrier_phase));
    }
}

void Sender::generateHeader() {
    int start_freq = 2000;
    int end_freq = 10000;
    float freq_step = (end_freq - start_freq) / (header_len / 2);
    float time_gap = (float)1 / (float)sample_rate;
    std::vector<float> fp;
    std::vector<float> omega;
    for (int i = 0; i < header_len; i++) {
        fp.push_back(0);
        omega.push_back(0);
    }
    fp[0] = start_freq;
    fp[header_len / 2] = end_freq;
    for (int i = 1; i < header_len / 2; i++)
        fp[i] = fp[i - 1] + freq_step;
    for (int i = header_len / 2 + 1; i < header_len; i++)
        fp[i] = fp[i - 1] - freq_step;
    for (int i = 1; i < header_len; i++)
        omega[i] = omega[i - 1] + (fp[i] + fp[i - 1]) / 2.0 * time_gap;
    for (int i = 0; i < header_len; i++)
        header_wave[i] = sin(2 * PI * omega[i]);
}

void Sender::Modulation(Array<int8_t> cur_frame_data, int frame_len) {
    for (int i = 0; i < frame_len; i++) {
        for (int j = 0; j < num_samples_per_bit; j++) {
            /*if (cur_frame_data[i] > 1) {
                cout << cur_frame_data[i] << endl;
            }*/
            //std::cout <<"Sender "<<i<<":"<< (int)cur_frame_data[i] << '\n';
            frame_wave[i * num_samples_per_bit + j] = ((int)cur_frame_data[i] * 2 - 1) * carrier_wave[j];
            //std::cout << "Sender " << i << ":" << frame_wave[i * num_samples_per_bit + j] << '\n';
        }
    }
    //std::cout << "---------------------\n";
}

void Sender::sendOnePacket(int frame_len, Array<int8_t> cur_frame_data) {
    //output_buffer.reset();
    Modulation(cur_frame_data, frame_len);
    Array<float> audio;
    audio.addArray(header_wave, header_len);
    audio.addArray(frame_wave, frame_len * num_samples_per_bit);
    audio.resize(512);
    //audio.addArray(zeros, len_zeros);

    //?
    //while (audio.size() != 512)audio.add(0);

    //{ std::cout << "Sender audio sz: " << audio.size() << '\n'; }

    {
        const ScopedLock sl(lock);
        if (buffer.size() < AUDIO_BUFFER_SIZE)buffer.add(audio);   
    }
}

int Sender::startSend() {
    //const ScopedLock sl(lock);
    //playingSampleNum = 0;
    isPlaying = true;
    //std::cout << len_frame << '\n';
    startup_wave = new float[len_warm_up];
    for (int j = 0; j < len_warm_up; j++)
        startup_wave[j] = carrier_wave[j % num_samples_per_bit];
    //buffer.write(startup_wave, 480);
    //output_buffer.write(startup_wave, 480);
    return 1;
}

void Sender::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
    float** outputChannelData, int numOutputChannels, int numSamples)
{
    
    
  /*  Array<float> Data;
    Data.resize(numSamples);*/

    //std::cout << numSamples << '\n';

    for (auto i = numOutputChannels; --i >= 0;) {
        if (outputChannelData[i] != nullptr) {
            //for (int k = 0; k < numSamples; k++)
            //{
            //    //outputChannelData[i][k] = sin(2 * PI *0.1* k);
            //    //std::cout << sin(2 * PI * 0.1 * k)<<'\n';
            //}
            
            //std::cout << "Send\n";
            /*for (int k = 0; k < 64; k++)
            {
                outputChannelData[i][k] = header_wave[k];
            }

            for (int k = 0; k < 440; k++)
            {
                outputChannelData[i][k+64] = frame_wave[k];
            }

            for (int k = 0; k < 8; k++)
            {
                outputChannelData[i][504+k] = zeros[k];
            }*/
            bool buffer_flag;
            {
                const ScopedLock sl1(lock);
                buffer_flag= buffer.size() > 0 ? 1 : 0;
            }
            
            if (buffer_flag)
            {
                const ScopedLock sl1(lock);
                //std::cout << "Success\n";
                
                memcpy(outputChannelData[i], buffer[0].data(), sizeof(float) * numSamples);
                buffer.removeRange(0, 1);
            }
            else
            {
                //std::cout << "Empty\n";
                zeromem(outputChannelData[i], numSamples * sizeof(float));
            }
        }
    }
}

void
Sender::hiResTimerCallback()
{
    /*const ScopedLock sl(lock);
    if (isPlaying && buffer.getsize() == 0)
    {
        isPlaying = false;
        stopTimer();
    }*/
}