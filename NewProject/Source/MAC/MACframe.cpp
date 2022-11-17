#include "MACframe.h"

MACframe::MACframe()
{
    ;
}

MACframe::MACframe(const MACframe& f)
{
    type = f.type;
    frame_id = f.frame_id;
    dst_address = f.dst_address;
    src_address = f.src_address;
    ack_id = f.ack_id;
    data=f.data;
    ip_port = f.ip_port;
    ip_address=f.ip_address;
    port = f.port;
    send_time=f.send_time;
    receive_time=f.receive_time;
    frame_status=f.frame_status;
    resend_times=f.resend_times;
    crc=f.crc;
    bad_crc = f.bad_crc;
    crc_int8=f.crc_int8;
}

// constructor for receive frame
MACframe::MACframe(Array<int8_t> all_data) : crc() {
    this->receive_time = std::chrono::system_clock::now();
    type = all_data[0];
    frame_id = all_data[1];
    dst_address = all_data[2];
    src_address = all_data[3];
    if (type == TYPE_DATA) {
        int8_t frame_crc = all_data[all_data.size() - 1];
        for (int i = 4; i < all_data.size() - 1; i++) {
            /*if (i < 10)
                ip_port.add(all_data[i]);
            else*/
            int8_t x = all_data[i];
                data.add(all_data[i]);
            crc.updateCRC(all_data[i]);
        }
        crc_int8 = crc.getCRC();
        //translateAddrPort();
        if (frame_crc != crc.getCRC())
        {
            std::cout << (int)frame_crc << ' ' << (int)crc.getCRC()<<'\n';
            bad_crc = 1;
        }
            
    }
    else if (type == TYPE_ACK || type == TYPE_MACPING_REPLY) {
        for (int i = 4; i < all_data.size(); i++)
            data.add(all_data[i]);
    }
    resend_times = 0;
}

// constructor for ack frame
MACframe::MACframe(int8_t dst_address, int8_t src_address, int8_t ack_id) {
    type = (int8_t)TYPE_ACK;
    this->dst_address = dst_address;
    this->src_address = src_address;
    this->ack_id = ack_id;
    this->frame_id = ack_id;
    for (int i = 0; i < 8; i++)
        data.insert(0, (int8_t)((ack_id >> i) & 1));
    frame_status = Status_Waiting;
    resend_times = 0;
}

// constructor for data frame
MACframe::MACframe(int8_t dst_address, int8_t src_address, Array<int8_t> frame_data) : crc() {
    type = (int8_t)TYPE_DATA;
    this->dst_address = dst_address;
    this->src_address = src_address;
    for (int i = 0; i < frame_data.size(); i++) {
        int8_t x = frame_data[i];
        crc.updateCRC(frame_data[i]);
        for (int k = 7; k >= 0; k--)
            data.add((int8_t)((frame_data[i] >> k) & 1));
    }
    auto tmp = crc.getCRC();
    this->crc_int8 = tmp;
    for (int k = 7; k >= 0; k--)
        data.add((int8_t)(tmp >> k) & 1);
    frame_status = Status_Waiting;
    resend_times = 0;
}

//construct a MACPING frame
MACframe::MACframe(int8_t type, int8_t reply_id, int8_t dst_address, int8_t src_address)
{
    this->dst_address = dst_address;
    this->src_address = src_address;
    this->type = (int8_t)type;
    this->ack_id = (int8_t)reply_id;
    for (int i = 0; i < 8; i++)
        data.insert(0, (int8_t)((reply_id >> i) & 1));
    frame_status = Status_Waiting;
    resend_times = 0;
}

// constructor for macperf frame
MACframe::MACframe(int8_t dst_address, int8_t src_address, int frame_bit_num) {
    type = (int8_t)TYPE_DATA;
    this->dst_address = dst_address;
    this->src_address = src_address;
    for (int i = 0; i < frame_bit_num; i++)
        data.add(rand() % 2);
    frame_status = Status_Waiting;
    resend_times = 0;
}

void
MACframe::setSendTime() {
    send_time = std::chrono::system_clock::now();
}
void
MACframe::setReceiveTime()
{
    receive_time = std::chrono::system_clock::now();
}

double
MACframe::getRTT()
{
    return (receive_time - send_time).count();
}

double
MACframe::getTimeDuration() {
    std::chrono::duration<double, std::milli> diff = std::chrono::system_clock::now() - send_time;
    return diff.count();
}

Array<int8_t>
MACframe::toBitStream() {
    Array<int8_t> ret_array = Array<int8_t>(data);
    for (int i = 0; i < 8; i++)
        ret_array.insert(0, (int8_t)((src_address >> i) & 1));
    for (int i = 0; i < 8; i++)
        ret_array.insert(0, (int8_t)((dst_address >> i) & 1));
    for (int i = 0; i < 8; i++)
        ret_array.insert(0, (int8_t)((frame_id >> i) & 1));
    for (int i = 0; i < 8; i++)
        ret_array.insert(0, (int8_t)((type >> i) & 1));
    /*byteToBits(ret_array, frame_id);
    byteToBits(ret_array, type);*/
    return ret_array;
}

void
MACframe::translateAddrPort() {
    ip_address = "";
    for (int i = 0; i < 4; i++) {
        ip_address += std::to_string((int)(unsigned char)ip_port[i]);
        if (i != 3)
            ip_address += ".";
    }
    port = (int)(unsigned char)ip_port[4] * 16 * 16 + (int)(unsigned char)ip_port[5];
}

void MACframe::printFrame() {
    std::cout << "address: " << ip_address << ", port: " << port << ", data: ";
    auto raw_data = (char*)data.getRawDataPointer();
    for (int i = 0; i < 40; i++)
        std::cout << raw_data[i];
}