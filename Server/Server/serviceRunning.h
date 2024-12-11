#include <windows.h>
#include <iostream>
#include <fstream>
#include <psapi.h>
#include <string>
#include <vector>
#include <tchar.h>
#include <iomanip>

using namespace std;

bool getServiceRunning(const std::string& filename);
bool startService(const std::string& serviceName);
bool endService(const std::string& serviceName);