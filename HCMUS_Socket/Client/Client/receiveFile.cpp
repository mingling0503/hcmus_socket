#include "receiveFile.h"

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
