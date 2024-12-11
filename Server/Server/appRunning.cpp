#include "appRunning.h"

#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <psapi.h>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>

using namespace std;

TCHAR lpstring[500];
HWND g_HWND = NULL;

// Function to convert wstring to string
std::string wstringToString(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}

std::wstring stringToWstring(const std::string& str) {
    return std::wstring(str.begin(), str.end());
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
void saveProcessNameAndID(DWORD processID, ofstream& outfile) {
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
            outfile << std::setw(50) << std::left << wstringToString(processName)  // Process Name, width 30
                << std::setw(10) << std::left << processID                     // PID, width 10
                << std::setw(50) << std::left << wstringToString(windowTitle)   // Window Title, width 50
                << "\n";  // Newline at the end
        }

        CloseHandle(hProcess);
    }
}

// Main function to get running applications
bool getAppRunning(const std::string& filename) {
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    // Open a file to save the results
    ofstream outfile(filename);
    if (!outfile.is_open()) {
        cerr << "Failed to open file for writing." << endl;
        return false; // Exit if file can't be opened
    }

    // Get the list of process identifiers
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        outfile.close();
        cerr << "Failed to enumerate processes." << endl; // Log error
        return false;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    // Iterate through the process identifiers and print their information
    for (i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            saveProcessNameAndID(aProcesses[i], outfile);
        }
    }

    // Close the output file
    outfile.close();
    return true;
}

bool tryStartApp(std::string& appName) {
    cout << "Start " << appName << "...\n";
    std::wstring wideAppName(appName.begin(), appName.end());

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    BOOL success = CreateProcess(
        NULL,                               // Application name (use NULL to use command line)
        const_cast<LPWSTR>(wideAppName.c_str()), // Command line (wide string)
        NULL,                               // Process attributes
        NULL,                               // Thread attributes
        FALSE,                              // Inherit handles
        0,                                  // Creation flags
        NULL,                               // Environment variables
        NULL,                               // Current directory
        &si,                                // Startup info
        &pi                                 // Process information
    );

    if (!success) {
        std::cerr << "Failed to start application: " << appName << std::endl;
        return false;
    }

    // Optionally, wait for the process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}


bool startApp(std::string& appName) {
    std::cout << "Starting " << appName << "...\n";
    std::wstring appNameW = stringToWstring(appName);
    // Start the application asynchronously without waiting for it to finish
    HINSTANCE result = ShellExecuteW(NULL, L"open", appNameW.c_str(), NULL, NULL, SW_SHOWNORMAL);
    if (result <= (HINSTANCE)32) {
        std::cerr << "Failed to start the application.\n";
        return false;
    }

    return true;
}


// Structure to hold process information
struct ProcessInfo {
    DWORD processId;
    std::wstring processName;
};


// Function to get the process name from a PID
std::wstring getProcessName(DWORD processID) {
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    if (NULL != hProcess) {
        HMODULE hMod;
        DWORD cbNeeded;

        // Get the process name
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
        }
        CloseHandle(hProcess);
    }

    return std::wstring(szProcessName);
}

// Function to list processes by name
std::vector<ProcessInfo> listProcessesByName(const std::wstring& targetProcessName) {
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    std::vector<ProcessInfo> matchingProcesses;

    // Get the list of process identifiers
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        cerr << "Failed to enumerate processes." << endl;
        return matchingProcesses;
    }

    // Calculate the number of processes
    cProcesses = cbNeeded / sizeof(DWORD);

    // Iterate through the process list
    for (unsigned int i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            std::wstring processName = getProcessName(aProcesses[i]);

            // Check if the process name matches
            if (_wcsicmp(processName.c_str(), targetProcessName.c_str()) == 0) {
                matchingProcesses.push_back(ProcessInfo{ aProcesses[i], processName });
            }
        }
    }

    return matchingProcesses;
}

// Function to terminate a process by PID
bool terminateProcessById(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess) {
        if (TerminateProcess(hProcess, 0)) {
            CloseHandle(hProcess);
            return true;
        }
        CloseHandle(hProcess);
    }
    return false;
}

// Function to terminate processes by name or PID
bool endApp(std::string& appName, DWORD pid) {
    std::wstring wideAppName(appName.begin(), appName.end());
    std::vector<ProcessInfo> matchingProcesses = listProcessesByName(wideAppName);

    if (matchingProcesses.empty()) {
        cout << "No processes found with the name: " << appName << endl;
        return false;
    }

    // Display matching processes
    cout << "Matching processes:" << endl;
    for (size_t i = 0; i < matchingProcesses.size(); ++i) {
        cout << std::setw(3) << (i + 1) << ". "
            << "PID: " << matchingProcesses[i].processId
            << ", Name: " << wstringToString(matchingProcesses[i].processName) << endl;
    }


    if (pid == -1) {
        // Terminate all matching processes
        for (const auto& proc : matchingProcesses) {
            if (terminateProcessById(proc.processId)) {
                cout << "Terminated process PID: " << proc.processId << endl;
            }
            //else {
            //    cout << "Failed to terminate process PID: " << proc.processId << endl;
            //}
        }
    }
    else {
        // Terminate the specific process by PID
        bool terminated = false;
        for (const auto& proc : matchingProcesses) {
            if (proc.processId == pid) {
                terminated = terminateProcessById(proc.processId);
                if (terminated) {
                    cout << "Successfully terminated process PID: " << proc.processId << endl;
                    return true;
                }
                else {
                    cout << "Failed to terminate process PID: " << proc.processId << endl;
                    return false;
                }
                break;
            }
        }

        if (!terminated) {
            cout << "No matching process found with PID: " << pid << endl;
            return false;
        }
    }
}

