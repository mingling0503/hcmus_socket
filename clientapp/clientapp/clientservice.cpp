// clientservice.cpp
#include "clientservice.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>

#include <vector>
#include <QMutexLocker>


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


// Note: Simulating core functions from original client.cpp
// You'll need to implement actual socket and email processing logic

ClientService::ClientService(QObject* parent) : QObject(parent) {
    serviceTimer = new QTimer(this);
    connect(serviceTimer, &QTimer::timeout, this, &ClientService::processEmail);
}

void ClientService::startService() {
    emit logMessage("Remote Control Service Started "
        + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    serviceTimer->start(5000);  // Check for emails every 5 seconds
}

void ClientService::processEmail() {
    emit logMessage("Checking for new emails...");
    auto registeredEmails = loadRegisteredEmails();

    if (receiveEmail()) {
        std::string emailFilePath = "../x64/Debug/inbox/email.txt";
        std::vector<std::string> requests = {};
        
        std::vector<std::string> emailInfo = parseEmail(emailFilePath, requests);
        if (emailInfo.size() == 0) {
            emit logMessage("Could not parse email!");
            return;
        }

        if (emailInfo.size() < 2) {
            emit logMessage("Invalid email format. Skipping...");
            return;
        }

        string senderEmail = emailInfo[0];
        if (registeredEmails.find(senderEmail) == registeredEmails.end()) {
            emit logMessage("Sender email not registered. Skipping...");
            return;
        }
        
        emit logMessage(QString::fromStdString("-----------------PROCESSING FOR USER: " + senderEmail + "--------------------"));

        string serverIP = emailInfo[1];
        //string passKey = emailInfo[2];

        
        for (string request : requests) {
            string resultFilePath, messageResult = "";
            emit logMessage(QString::fromStdString("\nProcessing " + request));
            if (request == LISTAPP || request == LISTSERVICE)
                resultFilePath = "result.txt";

            else if (request.rfind(GETFILE_PREFIX, 0) == 0) {
                resultFilePath = request.substr(GETFILE_PREFIX.length());
            }
            
            else resultFilePath = "result.jpg";

            int result = executeRequest(serverIP, request, messageResult, resultFilePath);
            if (result == 0) {
                // Send result email
                if (messageResult != "") {
                    if (sendEmailWith(senderEmail, "RESULT: " + request, messageResult)) {
                        emit logMessage(QString::fromStdString("Result message of " + request + " to user sent!\n"));
                    }
                }
                else {
                    if (sendEmailWith(senderEmail, "RESULT: " + request, messageResult, resultFilePath)) {
                        emit logMessage(QString::fromStdString("Result file of " + request + " to user sent!\n"));
                    }
                }
            }
            else {
                emit logMessage("Executed failed!");
            }
        }
        emit logMessage("---------------------------------------------------------------------------------------\n");
    }
}

bool ClientService::receiveEmail(){
    const int MailServerPop3 = 0;
    const int MailServerImap4 = 1;
    const int MailServerEWS = 2;
    const int MailServerDAV = 3;
    const int MailServerMsGraph = 4;

    bool received = false;

    // Initialize COM environment
    ::CoInitialize(NULL);

    // Create a folder named "inbox" under current exe file directory
    // to save the emails retrieved.

    TCHAR szPath[MAX_PATH + 1];
    _getCurrentPath(szPath, MAX_PATH);

    TCHAR szMailBox[MAX_PATH + 1];
    wsprintf(szMailBox, _T("%s\\inbox"), szPath);

    TCHAR szNewFile[MAX_PATH + 1];
    wsprintf(szNewFile, _T("%s\\email.txt"), szMailBox);

    if (GetFileAttributes(szNewFile) != INVALID_FILE_ATTRIBUTES) {
        if (!DeleteFile(szNewFile)) {
            _tprintf(_T("Failed to delete existing email.txt. Error: %d\n"), GetLastError());
            return false;
        }
    }

    // Create a folder to store emails
    ::CreateDirectory(szMailBox, NULL);

    try
    {
        IMailServerPtr oServer = NULL;
        oServer.CreateInstance(__uuidof(EAGetMailObjLib::MailServer));

        // Gmail IMAP4 server address
        oServer->Server = _T("imap.gmail.com");
        oServer->User = _T("almondsrabbit@gmail.com");

        // Create app password in Google account
        oServer->Password = _T("slqi fvvq obfs giik");
        oServer->Protocol = MailServerImap4;

        // Enable SSL connection
        oServer->SSLConnection = VARIANT_TRUE;

        // Set 993 IMAP4 SSL port
        oServer->Port = 993;

        IMailClientPtr oClient = NULL;
        oClient.CreateInstance(__uuidof(EAGetMailObjLib::MailClient));
        oClient->LicenseCode = _T("TryIt");

        if (oClient->Connect(oServer)) {
            _tprintf(_T("Connected\r\n"));
        }

        IMailInfoCollectionPtr infos = oClient->GetMailInfoList();
        if (infos->Count > 0)
            emit logMessage("Email received!");

        for (long i = 0; i < infos->Count; i++)
        {
            IMailInfoPtr pInfo = infos->GetItem(i);

            TCHAR szFile[MAX_PATH + 1];
            // Generate a random file name by current local datetime,
            // You can use your method to generate the filename if you do not like it
            SYSTEMTIME curtm;
            ::GetLocalTime(&curtm);
            ::wsprintf(szFile, _T("%s\\%04d%02d%02d%02d%02d%02d%03d%d.txt"),
                szMailBox,
                curtm.wYear,
                curtm.wMonth,
                curtm.wDay,
                curtm.wHour,
                curtm.wMinute,
                curtm.wSecond,
                curtm.wMilliseconds,
                i);

            // Receive email from POP3 server
            EAGetMailObjLib::IMailPtr oMail = oClient->GetMail(pInfo);
            received = true;

            // Save email to local disk
            oMail->SaveAs(szFile, VARIANT_TRUE);
            // Rename the saved file to "email.txt"

            TCHAR szNewFile[MAX_PATH + 1];
            wsprintf(szNewFile, _T("%s\\email.txt"), szMailBox);

            if (!MoveFileEx(szFile, szNewFile, MOVEFILE_REPLACE_EXISTING)) {
                _tprintf(_T("Failed to rename file to email.txt. Error: %d\n"), GetLastError());
                return false;
            }

            // Mark email as deleted from POP3 server.
            oClient->Delete(pInfo);
        }

        // Delete method just mark the email as deleted,
        // Quit method expunge the emails from server exactly.
        oClient->Quit();
    }
    catch (_com_error& ep)
    {
        _tprintf(_T("Error: %s"), (const TCHAR*)ep.Description());
    }
    return received;
}


int ClientService::executeRequest(const string& serverIP, const string& request, string& messageResult, const string& resultFilePath) {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    if (WSAStartup(ver, &wsData) != 0) {
        emit logMessage("Error starting winsock!");
        return -1;
    }

    SOCKET clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSock == INVALID_SOCKET) {
        emit logMessage("Error creating socket!, " + WSAGetLastError());
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(55000);
    inet_pton(AF_INET, serverIP.c_str(), &hint.sin_addr);


    if(::connect(clientSock, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        emit logMessage("Error connect to server!, " + WSAGetLastError());
        closesocket(clientSock);
        WSACleanup();
        return -1;
    }

    emit logMessage(QString::fromStdString("Connected to server " + serverIP + " successfully!"));

    int byRecv = send(clientSock, request.c_str(), 100, 0);

    if (byRecv == 0 || byRecv == -1) {
        closesocket(clientSock);
        return 1;
    }

    emit logMessage(QString("Executing request: %1 on server %2")
        .arg(QString::fromStdString(request),
            QString::fromStdString(serverIP)));

    if (request == STOPWEBCAM ||					// message-returned requests
        request.rfind(START_APP_PREFIX, 0) == 0 ||
        request.rfind(END_APP_PREFIX, 0) == 0 ||
        request.rfind(START_SERVICE_PREFIX, 0) == 0 ||
        request.rfind(END_SERVICE_PREFIX, 0) == 0 ||
        request.rfind(DEL_FILE_PREFIX, 0) == 0
        ) {
        const int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);

        int byRecv2 = recv(clientSock, buffer, sizeof(buffer) - 1, 0);

        if (byRecv2 > 0) {
            buffer[byRecv2] = '\0';
            messageResult = buffer;
            emit logMessage(QString::fromStdString("Received result message of " + request));
            return 0;
        }
        else {
            emit logMessage(QString::fromStdString("Could not receive result message of " + request));
            return 1;

        }
    }
    // file-returned requests
    else {
        if (getFileFromServer(clientSock, resultFilePath)) {
            emit logMessage(QString::fromStdString("Received result file of " + request));
            return 0;
        }
        else {
            emit logMessage(QString::fromStdString("Could not receive result file of " + request));
            return 1; // not receive code
        }
    }

    closesocket(clientSock);
    WSACleanup();
}

bool ClientService::sendEmailWith(std::string Recipient, std::string Subject, std::string& BodyText, const std::string& AttachmentPath)
{
    ::CoInitialize(NULL);

    const int ConnectNormal = 0;
    const int ConnectSSLAuto = 1;
    const int ConnectSTARTTLS = 2;
    const int ConnectDirectSSL = 3;
    const int ConnectTryTLS = 4;

    EASendMailObjLib::IMailPtr oSmtp = NULL;
    oSmtp.CreateInstance(__uuidof(EASendMailObjLib::Mail));

    try {
        oSmtp->LicenseCode = _T("TryIt");
    }
    catch (const _com_error& e) {
        _tprintf(_T("COM Error: %s\n"), (const TCHAR*)e.Description());
        _tprintf(_T("Error Code: 0x%08lx\n"), e.Error());
    }

    // Set email address
    oSmtp->ServerAddr = _T("smtp.gmail.com");
    oSmtp->FromAddr = _T("almondsrabbit@gmail.com");
    oSmtp->AddRecipientEx(Recipient.c_str(), 0);

    // User and password for ESMTP authentication
    oSmtp->UserName = _T("almondsrabbit@gmail.com");
    oSmtp->Password = _T("slqi fvvq obfs giik");

    oSmtp->Subject = Subject.c_str();
    oSmtp->BodyText = BodyText.c_str();

    oSmtp->ConnectType = ConnectTryTLS;
    oSmtp->ServerPort = 587;

    // Add attachment
    oSmtp->AddAttachment(AttachmentPath.c_str());


    emit logMessage("Start to send email ...");

    if (oSmtp->SendMail() == 0)
    {
        emit logMessage("Email was sent successfully!");
    }
    else
    {
        emit logMessage("Failed to send email with the following error: %s"),
            (const TCHAR*)oSmtp->GetLastErrDescription();
        return false;
    }
    ::CoUninitialize();
    return true;
}

std::vector<std::string> ClientService::parseEmail(const string& filePath, vector<string>& requests) {
    ifstream file(filePath);
    if (!file.is_open()) {
        emit logMessage(QString::fromStdString("Could not open file: " + filePath));
        return {};
    }

    string line;
    string senderEmail;
    string contentPlainText;
    string boundary;
    string partContent;

    // Regular expressions for parsing email components
    regex senderRegex(R"(From:.*<([\w\.-]+@[\w\.-]+)>)");
    regex boundaryRegex(R"(boundary=\"?(.+?)\"?)");
    smatch match;

    // First pass: Extract sender email and boundary
    while (getline(file, line)) {
        if (regex_search(line, match, senderRegex)) {
            senderEmail = match[1];
        }
        if (regex_search(line, match, boundaryRegex)) {
            boundary = "--" + match[1].str();
        }
    }

    // Reset file stream
    file.clear();
    file.seekg(0, ios::beg);

    bool isPlainText = false;

    // Extract plain text content
    while (getline(file, line)) {
        if (!boundary.empty() && line.find(boundary) != string::npos) {
            if (isPlainText) {
                contentPlainText = partContent;
                break;
            }
            partContent.clear();
            isPlainText = false;
        }

        if (line.find("Content-Type: text/plain") != string::npos) {
            isPlainText = true;
            file.ignore();
        }
        else if (isPlainText) {
            partContent += line + "\n"; // Preserve readability
        }
    }

    file.close();

    // Split plain text content into individual lines
    vector<string> info = split(contentPlainText, '\n');
    if (info.size() < 3) {
        emit logMessage("Email content is incomplete or improperly formatted.");
        return {};
    }

    // Extract sender email, IP address, passKey, and requests
    string ipAddr = info[0];
    //string passKey = info[1];
    for (size_t i = 1; i < info.size(); ++i) {
        if (!info[i].empty()) {
            requests.push_back(info[i]);
        }
    }

    //for (auto request : requests) {
    //    if (request.find('-') != string::npos) {
    //        vector <string> req = split(request, '-');
    //        request = req[0];
    //        nameAdd = req[1]; // name of file or service
    //    }
    //}
    return { senderEmail, ipAddr };
}

void ClientService::stopService() {
    // Stop the timer that checks for emails
    if (serviceTimer) {
        serviceTimer->stop();
        emit logMessage("Service stopped");
    }
}
