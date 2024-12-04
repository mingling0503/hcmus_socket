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
    }

    closesocket(clientSock);
    return true;
}

//int deleteFile(string searchfilename, const string resultFile) {
//
//    ifstream fileread(searchfilename.c_str()); // Open file to check existence
//
//    if (!fileread) {
//        cout << "\nERROR: Either you entered an invalid date or there is no diary entry for this date.\n\n";
//    }
//    else {
//        fileread.close();
//
//        if (remove(searchfilename.c_str()) == 0) { // Delete the file
//            cout << "\nINFO: Diary entry deleted successfully!\n";
//            if (screenshot(resultFile)) {
//                return 0;
//            }
//        }
//        else {
//            std::cerr << "ERROR: Unable to delete file. Reason: " << std::strerror(errno) << "\n";
//        }
//
//        system("pause");
//    }
//    return 1;
//}