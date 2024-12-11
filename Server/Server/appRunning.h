#ifndef APP_RUNNING_H
#define APP_RUNNING_H

#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <psapi.h>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>

// Namespace usage
using namespace std;

// Structure to hold process information

// String conversion functions
std::string wstringToString(const std::wstring& wstr);
std::wstring stringToWstring(const std::string& str);

// Functions for process management
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM processID);
void saveProcessNameAndID(DWORD processID, std::ofstream& outfile);
bool getAppRunning(const std::string& filename);
bool startApp(std::string& appName);
std::wstring getProcessName(DWORD processID);

bool terminateProcessById(DWORD processId);
bool endApp(std::string& appName, DWORD pid = -1);

#endif // APP_RUNNING_H
