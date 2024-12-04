#include "appRunning.h"

TCHAR lpstring[500];
HWND g_HWND = NULL;

// Function to convert wstring to string
std::string wstringToString(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}

// Function to enumerate windows
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM processID) {
    DWORD lpdwProcessId;
    if (IsWindowVisible(hwnd)) {
        GetWindowThreadProcessId(hwnd, &lpdwProcessId);

        // Use the correct size for the window title buffer
        int nmax = sizeof(lpstring) / sizeof(TCHAR);
        GetWindowText(hwnd, lpstring, nmax);

        if (lpdwProcessId == processID) {
            return FALSE; // Stop enumerating windows if process is found
        }
    }
    return TRUE; // Continue enumerating windows
}

// Function to print process name and ID
void PrintProcessNameAndID(DWORD processID, ofstream& outfile) {
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    if (NULL != hProcess) {
        HMODULE hMod;
        DWORD cbNeeded;

        // Get the process name
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
        }

        // Find the window corresponding to this process
        EnumWindows(EnumWindowsProc, (LPARAM)processID);

        // Convert TCHAR to std::wstring for output
        wstring processName(szProcessName);
        wstring windowTitle(lpstring);  // Store the window title in wstring

        // Ensure that lpstring is valid before using it
        if (_tcslen(lpstring) > 0 && _tcsstr(lpstring, _T("Start")) == NULL) {
            // Output to file, converting wstring to string
            outfile << wstringToString(processName) << " | PID: " << processID << " | Window Title: " << wstringToString(windowTitle) << "\n";
        }

        CloseHandle(hProcess);
    }
}

// Main function to get running applications

bool getAppRunning(const string& filename) {
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    ofstream fileResult(filename);
    if (!fileResult.is_open()) {
        cout << "Failed to open file for writing." << endl;
        return false;
    }

    // Get the list of process identifiers
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        fileResult.close();
        cout << "Failed to enumerate processes." << endl;
        return false;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    // Iterate through the process identifiers and print their information
    for (i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            PrintProcessNameAndID(aProcesses[i], fileResult);
        }
    }

    // Close the output file
    fileResult.close();
    return true;
}
