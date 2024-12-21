#include "fileHandle.h"

bool sendFileRequested(SOCKET clientSock, string filename) {
    const int fileAvailable = 200;
    const int fileNotfound = 404;
    const int BUFFER_SIZE = 1024;
    char bufferFile[BUFFER_SIZE];

    // Attempt to open the file in the current directory in binary mode
    ifstream fileread(filename, ios::binary);  // Open file to check existence

    if (fileread.is_open()) {
        // File is available
        int bySendinfo = send(clientSock, (char*)&fileAvailable, sizeof(int), 0);
        if (bySendinfo == SOCKET_ERROR) {
            cout << "Error sending data: " << WSAGetLastError() << endl;
            return false;
        }

        // Get file size
        fileread.seekg(0, ios::end);
        long fileSize = fileread.tellg();
        fileread.seekg(0, ios::beg);

        // Send file size to client
        bySendinfo = send(clientSock, (char*)&fileSize, sizeof(long), 0);
        if (bySendinfo == SOCKET_ERROR) {
            cout << "Error sending data: " << WSAGetLastError() << endl;
            return false;
        }

        // Read file and send in chunks
        do {
            // Read and send part of the file to the client
            fileread.read(bufferFile, BUFFER_SIZE);
            if (fileread.gcount() > 0) {
                bySendinfo = send(clientSock, bufferFile, fileread.gcount(), 0);
            }

            if (bySendinfo == SOCKET_ERROR) {
                cout << "Error sending data: " << WSAGetLastError() << endl;
                break;
            }
        } while (fileread.gcount() > 0);

        fileread.close();
    }
    else {
        // File cannot be opened or does not exist
        int bySendCode = send(clientSock, (char*)&fileNotfound, sizeof(int), 0);
        if (bySendCode == SOCKET_ERROR) {
            cout << "Error sending data: " << WSAGetLastError() << endl;
            return false;
        }
        cout << "File not found: " << filename << endl;
        return false;
    }

    //closesocket(clientSock);
    return true;
}

bool deleteFile(string searchfilename) {

    ifstream fileread(searchfilename.c_str()); // Open file to check existence

    if (!fileread) {
        cout << "\nERROR: File does not exist.\n\n";
        return false;
    }
    else {
        fileread.close();

        if (remove(searchfilename.c_str()) == 0) { // Delete the file
            cout << "\nFile deleted successfully!\n";
            return true;
        }
        else {
            std::cerr << "ERROR: Unable to delete file.";
            return false;
        }
    }
}

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

string normalizePath(const std::string& path) {
    std::string normalizedPath = path;
    std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');
    return normalizedPath;
}