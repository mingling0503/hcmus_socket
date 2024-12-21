#include "emailHandling.h"
#include <iostream>
#include <fstream>

vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    size_t start = 0, end;
    while ((end = str.find(delimiter, start)) != string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

// receive emails
DWORD  _getCurrentPath(LPTSTR lpPath, DWORD nSize)
{
    DWORD dwSize = ::GetModuleFileName(NULL, lpPath, nSize);
    if (dwSize == 0 || dwSize == nSize)
    {
        return 0;
    }

    // Change file name to current full path
    LPCTSTR psz = _tcsrchr(lpPath, _T('\\'));
    if (psz != NULL)
    {
        lpPath[psz - lpPath] = _T('\0');
        return _tcslen(lpPath);
    }

    return 0;
}

bool getFileFromServer(SOCKET clientSock, string& fileResult, string& filename) {

	int codeAvailable = 404;
	const int fileAvailable = 200;
	const int fileNotfound = 404;
	long fileRequestedsize = 0;
	int fileDownloaded = 0;
	const int BUFFER_SIZE = 1024;
	char bufferFile[BUFFER_SIZE];
	//char fileRequested[FILENAME_MAX];
	std::ofstream file;
	if (filename != "") {
		int byRecv = send(clientSock, filename.c_str(), FILENAME_MAX, 0);
		if (byRecv == 0 || byRecv == -1) {
			cout << "Failed to send file name.";
			return false;
		}

		byRecv = recv(clientSock, (char*)&codeAvailable, sizeof(int), 0);
	}
	else {
		int byRecv = send(clientSock, fileResult.c_str(), FILENAME_MAX, 0);

		if (byRecv == 0 || byRecv == -1) {
			cout << "Failed to send file name.";
			return false;
		}

		byRecv = recv(clientSock, (char*)&codeAvailable, sizeof(int), 0);
		if (byRecv == 0 || byRecv == -1) {
			cout << "Failed to send file.";
			return false;
		}
	}

	if (codeAvailable == 200) {
		int byRecv = recv(clientSock, (char*)&fileRequestedsize, sizeof(long), 0);
		if (byRecv == 0 || byRecv == -1) {
			cout << "Failed to receive file.";
			return false;
		}

		file.open(fileResult, std::ios::binary | std::ios::trunc);
		do {
			memset(bufferFile, 0, BUFFER_SIZE);
			byRecv = recv(clientSock, bufferFile, BUFFER_SIZE, 0);

			if (byRecv == 0 || byRecv == -1) {
				break;
			}

			file.write(bufferFile, byRecv);
			fileDownloaded += byRecv;
		} while (fileDownloaded < fileRequestedsize);
		std::cout << "File received!" << std::endl;
		file.close();
	}
	else if (codeAvailable == 404) {
		std::cout << "Can't open file or file not found!" << std::endl;
		return false;
	}
	closesocket(clientSock);
	return true;
}

std::unordered_set<std::string> loadRegisteredEmails() {
	std::unordered_set<std::string> registeredEmails;

	QFile file("registered_users.json");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return registeredEmails; 
	}

	QByteArray data = file.readAll();
	file.close();

	QJsonDocument doc = QJsonDocument::fromJson(data);
	if (!doc.isObject()) {
		return registeredEmails; 
	}

	QJsonObject usersObj = doc.object();

	// Duyệt qua các đối tượng trong JSON, lấy key (email) và thêm vào unordered_set
	for (auto it = usersObj.begin(); it != usersObj.end(); ++it) {
		std::string email = it.key().toStdString(); 
		registeredEmails.insert(email);
	}

	return registeredEmails;
}

