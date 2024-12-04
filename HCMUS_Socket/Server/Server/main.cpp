#include "afxwin.h"  // For MFC core and standard components
#include "afxsock.h" // For MFC socket classes (optional)
#include <iostream>
#include <cstdio>
#include <string>

#include <fstream>
#include <ws2tcpip.h>
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

#include "fileHandle.h"
#include "appRunning.h"
#include "processRunning.h"

#include "screenshot.h"
#include "webcam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const string LISTPROCESS = "ListProcessRunning";
//const string GETFILE = "GetFile";
const string SCREENSHOT = "ScreenShot";
const string LISTAPP = "GetAppRunning";
const string STARTWEBCAM = "StartWebcam";
const string STOPWEBCAM = "StopWebcam";
const string SHUTDOWN = "Shutdown";

CWinApp theApp;
using namespace std;

bool checkCmd(string request, const string &filename) {
	if (request == LISTAPP) {
		if (getAppRunning(filename)) {
			return true;
		}
		cout << "failed to execute!\n";
	}
	else if (request == LISTPROCESS) {
		if (getProcessRunning(filename)) {
			return true;
		}
		cout << "failed to execute!\n";
	}
    else if (request == SCREENSHOT) {
		if (screenshot(filename)) {
			return true;
		}
		cout << "failed to execute!\n";
    }
    else if (request == STARTWEBCAM) {
        startWebcam(filename);
    }
    else if (request == STOPWEBCAM) {
        stopWebcam();
    }
	else {
		cout << "Request is not valid." << endl;
		return false;
	}
}

int main() {
	cout << "Waiting for request..." << endl;
	while (true) {
		WSADATA wsData;
		WORD ver = MAKEWORD(2, 2);

		if (WSAStartup(ver, &wsData) != 0) {
			std::cerr << "Error starting winsock!" << std::endl;
			return -1;
		}

		SOCKET listenerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (listenerSock == INVALID_SOCKET) {
			std::cerr << "Error creating listener socket! " << WSAGetLastError() << std::endl;
			WSACleanup();
			return -1;
		}
		std::cout << "Server is listenning..." << std::endl;

		sockaddr_in listenerHint;
		listenerHint.sin_family = AF_INET;
		listenerHint.sin_port = htons(55000);
		listenerHint.sin_addr.S_un.S_addr = INADDR_ANY;

		::bind(listenerSock, (sockaddr*)&listenerHint, sizeof(listenerHint));
		if (listen(listenerSock, SOMAXCONN) == SOCKET_ERROR) {
			std::cerr << "Error listening on socket! " << WSAGetLastError() << std::endl;
			closesocket(listenerSock);
			WSACleanup();
			return -1;
		}

		sockaddr_in clientHint;
		int clientSize = sizeof(clientHint);

		SOCKET clientSock = accept(listenerSock, (sockaddr*)&clientHint, &clientSize);

		if (clientSock == SOCKET_ERROR) {
			std::cerr << "Error accept socket! " << WSAGetLastError() << std::endl;
			closesocket(listenerSock);
			WSACleanup();
			return -1;
		}

		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];

		if (getnameinfo((sockaddr*)&clientHint, sizeof(clientHint), host, NI_MAXHOST, serv, NI_MAXSERV, 0) == 0) {
			std::cout << "Host: " << host << " connected on port: " << serv << std::endl;
		}
		else {
			inet_ntop(AF_INET, &clientHint.sin_addr, host, NI_MAXHOST);
			std::cout << "Host: " << host << " connected on port: " << ntohs(clientHint.sin_port) << std::endl;
		}

		closesocket(listenerSock);

		const int fileAvailable = 200;
		const int fileNotfound = 404;
		const int BUFFER_SIZE = 1024;
		char bufferFile[BUFFER_SIZE];
		 // Tạo buffer đủ lớn để nhận dữ liệu
		memset(bufferFile, 0, BUFFER_SIZE);
		int byRecv = recv(clientSock, bufferFile, sizeof(bufferFile) - 1, 0);  // Giảm đi 1 byte để dành cho ký tự kết thúc chuỗi '\0'

		if (byRecv == 0 || byRecv == -1) {
			continue;
		}

		bufferFile[byRecv] = '\0'; // Đảm bảo chuỗi kết thúc bằng ký tự null

		cout << "Request received: " << bufferFile << endl;

		string request(bufferFile);
		string fileResult = "result.txt";
		if (request == SCREENSHOT || request == STARTWEBCAM || request == STOPWEBCAM)
			fileResult = "result.jpg";


		if (checkCmd(request, fileResult)) {
			cout << "Execute request successfully.\n";
			if (sendFileRequested(clientSock, fileResult)) {
				cout << "Result file sent!\n";
				closesocket(clientSock);
				WSACleanup();
			}
		}
		else {
			cout << "FAILED.";
			closesocket(clientSock);
			WSACleanup();
		}
		cout << "\nWaiting for new request...\n";
	}
    return 0;
}