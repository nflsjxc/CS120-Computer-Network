#include "MAC.h"

MAC::MAC(AudioDeviceManager* dev_manager)
{
    nbpf = 144; //50 byte
    nspb = 3; // 5 samples per bit
    frame_len = 144; //Len is in bit
	this->dev_manager = dev_manager;
    se.reset(new Sender(nbpf, nspb));
    dev_manager->addAudioCallback(se.get());
    re.reset(new Receiver(nbpf, nspb));
    dev_manager->addAudioCallback(re.get());
    stop_flag = false;
}

MAC::~MAC() 
{
    dev_manager->removeAudioCallback(se.get());
    dev_manager->removeAudioCallback(re.get());
}

void MAC::main_thread()
{
    //MACframe dataframe(DST_ADDR, SRC_ADDR, );

    re->startRecording();
	send_thread=thread(&MAC::send,this);
	receive_thread=thread(&MAC::receive,this);

    bool receive_flag;
    MACframe frame;
    int current_sending = -1;
    int success_sending = -1;
    int idx;
    int srcadd, dstadd;
    Array<int8_t> payload;
    std::chrono::system_clock::time_point lstsendtime;
    for (;;)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        receive_flag = false;
        {
            const ScopedLock sl(rblock);
            if (receive_buffer.size() > 0)
            {
                receive_flag = true;
                frame = receive_buffer[0];
                receive_buffer.removeRange(0, 1);
            }
        }
        if (receive_flag)// Rx mode
        {
            //cout << "Rx\n";
            switch (frame.getType())
            {
            case TYPE_ACK:
            {
                idx = frame.getFrame_id();//For received frame this is ack
                cout << "Receive ACK frame! " << idx << "\n";
                auto fdata = frame.getData();
                for (int i = 0; i < fdata.size(); i++)
                {
                    cout << (int)fdata[i] << ' ';
                }
                cout << endl;

                if (success_sending == idx - 1)
                {
                    success_sending = idx;
                }
                break;
            }
                
            case TYPE_DATA:
            {
                idx = frame.getFrame_id();
                if (frame.isBadCRC()) {
                    // Receive a wrong frame. Go back to Frame_Detection
                    cout << "Receive BAD DATA frame! "<<idx<<'\n';
                    auto fdata = frame.getData();
                    for (int i = 0; i < fdata.size(); i++)
                    {
                        cout << (int)fdata[i] << ' ';
                    }
                    cout << endl;
                    break;
                }
                else
                {
                    cout << "Receive corret DATA frame! " <<idx<< "\n";
                    auto fdata = frame.getData();
                    for (int i = 0; i < fdata.size(); i++)
                    {
                        cout << (int)fdata[i] << ' ';
                    }
                    cout << endl;

                    //Reply ACK (TxACK)
                    MACframe ack(frame.getSrcAddr(), frame.getDstAddr(), frame.getFrame_id());
                    /*ack.setFrameId(frame.getFrame_id());*/
                    {
                        cout << "Reply ACK " << (int)frame.getFrame_id() << '\n';
                        const ScopedLock sl(sblock);
                        if (send_buffer.size() <= MAX_BUFFER_SIZE)
                        {
                            send_buffer.add(ack);
                        }
                    }
                    break;
                }
                break;
            }
                
            }
            continue;
        }
        else // Tx mode
        {
            //cout << "Tx\n";
            if (success_sending == current_sending)
            {
                current_sending++;
                
                payload.clear();
                for (int i = 1; i <= 13; i++)
                {
                    payload.add(current_sending*13+i);
                }
                for (int i = 0; i < payload.size(); i++)
                {
                    cout << (int)payload[i] << ' ';
                }
                cout << endl;
                MACframe fpayload(DST_ADDR, SRC_ADDR, payload);
                cout << "Sending DATA " << (int)fpayload.getFrame_id() << '\n';
                cout << (int)fpayload.crc_int8 << '\n';
                fpayload.setFrameId(current_sending);
                {
                    const ScopedLock sl(sblock);
                    if (send_buffer.size() <= MAX_BUFFER_SIZE)
                    {
                        send_buffer.add(fpayload);
                        lstsendtime = std::chrono::system_clock::now();
                    }
                }
            }
            else
            {
                std::chrono::duration<double, std::milli> diff = std::chrono::system_clock::now() - lstsendtime;
                if (diff.count() > MAX_WAITING_TIME)// Timeout, resend
                {
                    payload.clear();
                    for (int i = 1; i <= 13; i++)
                    {
                        payload.add(current_sending * 13 + i);
                    }

                    MACframe fpayload(DST_ADDR, SRC_ADDR, payload);
                    fpayload.setFrameId(current_sending);
                    cout << "TIMEOUT resend DATA " << current_sending << '\n';
                    cout <<(int) fpayload.crc_int8<<'\n';
                    {
                        const ScopedLock sl(sblock);
                        if (send_buffer.size() <= MAX_BUFFER_SIZE)
                        {
                            send_buffer.add(fpayload);
                            lstsendtime = std::chrono::system_clock::now();
                        }
                    }
                }
            }
        }
    }

    send_thread.join();
    receive_thread.join();
}

void MAC::send() //size data should be the frame_len (144 0/1)
{
    Array<int8_t>data;
    for (;;)
    {
        if (stop_flag)break;
        if (send_buffer.size() == 0)continue;
        MACframe frame;
        {
            const ScopedLock sl(sblock);
            frame = send_buffer[0];
            send_buffer.removeRange(0, 1);
        }

        //cout << "Sender: " << "Send one packet, size: " << frame.getFrame_size() + FRAME_OFFSET << "\n";
        se->sendOnePacket(frame.getFrame_size() + FRAME_OFFSET, frame.toBitStream());
        //se->sendOnePacket(frame_len, data);
        std::this_thread::sleep_for(chrono::milliseconds(5));// Is it necessary?
    }

}

void MAC::receive()// Receive 144 bit, 18 byte, 4 byte + 14 byte payload ?
{
    for (;;)
    {
        auto fdata = re->getData();
        //cout << "Receiver, size: " << fdata.size() << '\n';
        if (!fdata.size())continue;
        MACframe frame(fdata);
        {
            const ScopedLock sl(rblock);
            if (receive_buffer.size() > MAX_BUFFER_SIZE) continue;
            receive_buffer.add(frame);
        }
    }
}

/*
* Example of MacFrame
* MACFrame structure:
* TYPE | FRAME_ID | DST_ADDRESS | SRC_ADDRESS | DATA
* Firstly construct the different type MacFrame.
* Then call get_FrameSize() to get DATA's length.
* The sum length of Type and FrameID and DstAddress and SrcAddress is a constant number (32).
* Next call toBitStream() to get the full MACFrame.
*/