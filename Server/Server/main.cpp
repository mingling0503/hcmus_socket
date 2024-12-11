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
#include "serviceRunning.h"
//#include "processRunning.h"

#include "screenshot.h"
#include "webcam.h"
#include "shutdown.h"
#include "fileHandle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//const string LISTPROCESS = "ListProcessRunning";
//const string GETFILE = "GetFile";
const string SCREENSHOT = "ScreenShot";
const string LISTAPP = "GetAppRunning";
const string LISTSERVICE = "GetServiceRunning";
const string STARTWEBCAM = "StartWebcam";
const string STOPWEBCAM = "StopWebcam";
const string SHUTDOWN = "Shutdown";

const string GETFILE_PREFIX = "GetFile-";
const string DEL_FILE_PREFIX = "DeleteFile-";
const string START_APP_PREFIX = "StartApp-";
const string END_APP_PREFIX = "EndApp-";
const string START_SERVICE_PREFIX = "StartService-";
const string END_SERVICE_PREFIX = "EndService-";

CWinApp theApp;
using namespace std;

bool checkCmd(string request, string &fileResult) {	// execute request, update resultfile
	if (request == LISTAPP) {
		if (getAppRunning(fileResult)) {
			return true;
		}
	}
	else if (request == LISTSERVICE) {
		if (getServiceRunning(fileResult)) {
			return true;
		}
	}
    else if (request == SCREENSHOT) {
		if (screenshot(fileResult)) {
			return true;
		}
    }
    else if (request == STARTWEBCAM) {
        startWebcam(fileResult);
		return true;

    }
    else if (request == STOPWEBCAM) {
		return stopWebcam();
    }
	else if (request == SHUTDOWN) {
		ShutDownPC();
	}

	else if (request.rfind(START_APP_PREFIX, 0) == 0) {
		std::string appName = request.substr(START_APP_PREFIX.length());
		return startApp(appName);
	}

	else if (request.rfind(END_APP_PREFIX, 0) == 0) {
		std::string appName = request.substr(END_APP_PREFIX.length());
		return endApp(appName);
	}
	else if (request.rfind(START_SERVICE_PREFIX, 0) == 0) {
		std::string appName = request.substr(START_SERVICE_PREFIX.length());
		return startService(appName);
	}
	else if (request.rfind(END_SERVICE_PREFIX, 0) == 0) {
		std::string appName = request.substr(END_SERVICE_PREFIX.length());
		return endService(appName);
	}

	else if (request.rfind(GETFILE_PREFIX, 0) == 0) {
		std::string fileAddr = request.substr(GETFILE_PREFIX.length());
		ifstream f(fileAddr);
		if (f) {
			return true;
		}
		else return false;
	}

	else if (request.rfind(DEL_FILE_PREFIX, 0) == 0) {
		std::string fileAddr = request.substr(DEL_FILE_PREFIX.length());
		return deleteFile(fileAddr);
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
		string fileResult, messageResult;
		if (request == LISTAPP || request == LISTSERVICE)
			fileResult = "result.txt";
		else if (request.rfind(GETFILE_PREFIX, 0) == 0) {
			fileResult = request.substr(GETFILE_PREFIX.length());
		}
		else fileResult = "result.jpg";

		
		if (checkCmd(request, fileResult)) {	// executed successfully
			if (request == STOPWEBCAM ||					// message-returned requests
				request.rfind(START_APP_PREFIX, 0) == 0 ||
				request.rfind(END_APP_PREFIX, 0) == 0 ||
				request.rfind(START_SERVICE_PREFIX, 0) == 0 ||
				request.rfind(END_SERVICE_PREFIX, 0) == 0 ||
				request.rfind(DEL_FILE_PREFIX, 0) == 0
				) {
				fileResult = "";
				messageResult = "Executed " + request + " successfully";

				int byRecv = send(clientSock, messageResult.c_str(), 100, 0);

				if (byRecv == 0 || byRecv == -1) {
					cout << "Error sending message result.\n";
				}
				else cout << "Message result sent.\n";
				// return 0;

				closesocket(clientSock);
				WSACleanup();
				
			}
			
			else { // file-returned requests

				if (sendFileRequested(clientSock, fileResult)) {
					cout << "Result file sent!\n";
					closesocket(clientSock);
					WSACleanup();
				}
				else cout << "Failed to send file to client!\n";

			}
		}
		// executed failed
		else {
			cout << "Failed to execute request " << request << '\n';

			/*messageResult = "Failed to execute request " + request + " on server\n";
			int byRecv = send(clientSock, messageResult.c_str(), 100, 0);

			if (byRecv == 0 || byRecv == -1) {
				cout << "Error sending message result.\n";
			}
			else cout << "Message result to client sent!\n";*/

			
			closesocket(clientSock);
			WSACleanup();
			//return 1;
		}

		cout << "\nWaiting for new request...\n";
	}
    return 0;
}