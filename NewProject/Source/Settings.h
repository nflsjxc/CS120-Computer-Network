#pragma once

// Constant
#define PI (3.1415926)

//Mac frame
#define TYPE_ACK 0
#define TYPE_DATA 1
#define TYPE_MACPING_REQUEST 2
#define TYPE_MACPING_REPLY 3
#define TYPE_ICMP_REQUEST 4

// Receiver mode
#define NO_HEADER -1
#define SYNC 0
#define DATA_PROCESS 1
#define DATA_RECEIVED 2

//Frame info
#define FRAME_OFFSET 32
#define CRC_LEN 8
#define IP_PORT_LEN 48

#define MAX_RESEND_TIME 100
#define MAX_WAITING_TIME 1500

//Phy frame
#define AUDIO_BUFFER_SIZE (1000) // Allow 1000 frames waiting to be played
#define HEADER_LEN (60)
#define POWER_THRES (15)
//  60 Header + 3(sample)*144(bit) data + 20zero =512 samples per frame

//Address
#define SRC_ADDR (0x01) //Address of this machine
#define DST_ADDR (0x10) //Address of target machine

//Buffer option
#define MAX_BUFFER_SIZE (1000)

//Debug option
#define DETAIL_FLAG (0)