#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

bool getFileFromServer(SOCKET clientSock, string filename);