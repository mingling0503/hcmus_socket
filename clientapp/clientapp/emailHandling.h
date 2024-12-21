#pragma once

#define _WINSOCKAPI_    // Ngăn không cho winsock.h được bao gồm
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <afxwin.h>    // MFC headers

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <sstream>
#include <vector>
#include <Windows.h>
#include <tchar.h>
#include <unordered_set>

#include <string>
#include <fstream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "EAGetMailobj.tlh"
#include "easendmailobj.tlh"



#pragma comment(lib, "ws2_32.lib")  // Link winsock library

using namespace EAGetMailObjLib;
using namespace EASendMailObjLib;
using namespace std;

//receive emails
DWORD  _getCurrentPath(LPTSTR lpPath, DWORD nSize);

// parse emails
//string sanitizeRequest(string request);
vector<string> split(const string& str, char delimiter);
bool getFileFromServer(SOCKET clientSock, string& fileResult, string& filename);
std::unordered_set<std::string> loadRegisteredEmails();