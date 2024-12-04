#include <windows.h>
#include <psapi.h>
#include <tchar.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

std::string wstringToString(const std::wstring& wstr);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM processID);
void PrintProcessNameAndID(DWORD processID, ofstream& outfile);
bool getAppRunning(const string& filename);