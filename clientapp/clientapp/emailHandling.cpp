#include "emailHandling.h"

//string sanitizeRequest(string request) {
//    string sanitized;
//    for (char c : request) {
//        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ') {
//            sanitized += c;
//        }
//    }
//    return sanitized;
//}

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

bool getFileFromServer(SOCKET clientSock, string filename) {

	int codeAvailable = 404;
	const int fileAvailable = 200;
	const int fileNotfound = 404;
	long fileRequestedsize = 0;
	int fileDownloaded = 0;
	const int BUFFER_SIZE = 1024;
	char bufferFile[BUFFER_SIZE];
	//char fileRequested[FILENAME_MAX];
	std::ofstream file;


	int byRecv = send(clientSock, filename.c_str(), FILENAME_MAX, 0);
	if (byRecv == 0 || byRecv == -1) {
		cout << "Failed to send file name.";
		return false;
	}

	byRecv = recv(clientSock, (char*)&codeAvailable, sizeof(int), 0);
	if (byRecv == 0 || byRecv == -1) {
		cout << "Failed to send file.";
		return false;
	}
	if (codeAvailable == 200) {
		byRecv = recv(clientSock, (char*)&fileRequestedsize, sizeof(long), 0);
		if (byRecv == 0 || byRecv == -1) {
			cout << "Failed to send file.";
			return false;
		}

		file.open(filename, std::ios::binary | std::ios::trunc);
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
	std::ifstream infile("registered_emails.txt");
	std::string email;
	while (std::getline(infile, email)) {
		registeredEmails.insert(email);
	}
	return registeredEmails;
}