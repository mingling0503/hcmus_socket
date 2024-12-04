#include "afxwin.h"  
#include "afxsock.h" 
#include <iostream>
#include <cstdio>
#include <string>
#include <winsock2.h>  // For gethostname, gethostbyname
#include <ws2tcpip.h> // For InetNtop (to convert binary IP to string)

#include "receiveEmail.h"
#include "receiveFile.h"
#include "parseEmail.h"
#include "sendEmail.h"

#pragma comment(lib, "ws2_32.lib")  // Link winsock library

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const string LISTPROCESS = "ListProcessRunning";
const string GETFILE = "GetFile";
const string DELETEFILE = "DeleteFile";
const string SCREENSHOT = "ScreenShot";
const string LISTAPP = "GetAppRunning";
const string STARTWEBCAM = "StartWebcam";
const string STOPWEBCAM = "StopWebcam";
const string SHUTDOWN = "Shutdown";

CWinApp theApp;
using namespace std;

int executeRequest(const std::string& serverIP, const std::string& request, const std::string& resultFilePath) {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    if (WSAStartup(ver, &wsData) != 0) {
        std::cerr << "Error starting winsock!" << std::endl;
        return -1;
    }

    SOCKET clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSock == INVALID_SOCKET) {
        std::cerr << "Error creating socket!, " << WSAGetLastError() << std::endl;
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(55000);
    inet_pton(AF_INET, serverIP.c_str(), &hint.sin_addr);


    if (connect(clientSock, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        std::cerr << "Error connect to server!, " << WSAGetLastError() << std::endl;
        closesocket(clientSock);
        WSACleanup();
        return -1;
    }

    cout << "Connected to server " << serverIP << " successfully!" << endl;

    int byRecv = send(clientSock, request.c_str(), 100, 0);
    if (byRecv == 0 || byRecv == -1) {
        cout << "Failed to send request.";
        closesocket(clientSock);
    }
    getFileFromServer(clientSock, resultFilePath);
    closesocket(clientSock);
    WSACleanup();
    return 0;
}

int main() {
    std::cout << "Waiting for email...\n";
    while (true) {
        // Receive email
        if (!receiveEmail()) {
            continue;
        }

        // Parse email
        cout << "Parsing email..." << endl;
        const string emailFilePath = "D:\\project\\HCMUS_Socket\\Client\\x64\\Debug\\inbox\\email.txt";
        vector<string> emailInfo = parseEmail(emailFilePath);
        if (emailInfo.size() < 3) {
            std::cerr << "Invalid email format. Skipping...\n";
            break;
        }

        string userMail = emailInfo[0];
        string serverIP = emailInfo[1];
        string request = emailInfo[2];

        cout << userMail << "-" << serverIP << "-" << request << endl;

        string resultFilePath = "result.txt";
        if (request == SCREENSHOT || request == STARTWEBCAM || request == STOPWEBCAM)
            resultFilePath = "result.jpg";

        // Execute the request
        std::cout << "Executing request: " << request << " on server: " << serverIP << "\n";
        if (executeRequest(serverIP, request, resultFilePath) != 0) {
            std::cerr << "Failed to execute request for server: " << serverIP << "\n";
            break;
        }

        // Send result email
        cout << "Sending result back to user: " << userMail << "\n";
        if (!sendEmailWith(userMail, "RESULT", "Successfully processed your request.", resultFilePath)) {
            std::cerr << "Failed to send result email to user: " << userMail << "\n";
            break;
        }

        cout << "Processed request for server: " << serverIP << " successfully!\n";

        cout << "Type 'exit' to terminate the program or press Enter to continue.\n";
        string userInput;
        std::getline(std::cin, userInput);
        if (userInput == "exit") break;
        else cout << "\nWaiting for new email...\n";
    }
    return 0;
}
