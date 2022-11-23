#pragma once

#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include "Dataloader.h"
#include "Datareceiver.h"
using namespace std;

const int IN_PORT = 22111;
const int OUT_PORT = 22112;
const string IP_ADDR = "127.0.0.1";

int myserver(dataloader* dl);
int myclient(datareceiver* dr);

int myserver_test();