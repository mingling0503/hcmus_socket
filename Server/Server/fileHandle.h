#include <iostream>
#include <fstream>
#include <ws2tcpip.h>
#include <WinSock2.h>
#include <string>
#include <cstdio>  // For remove()
#include "screenshot.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")

bool sendFileRequested(SOCKET clientSock, string filename);
bool deleteFile(string searchfilename);