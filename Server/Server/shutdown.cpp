#include "shutdown.h"

bool ShutDownPC() {
    // Adjust privileges to enable shutdown
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    // Get a token for this process
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        std::cerr << "Failed to get process token.\n";
        return false;
    }
    // Get the LUID for the shutdown privilege
    if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) {
        std::cerr << "Failed to lookup shutdown privilege.\n";
        CloseHandle(hToken);
        return false;
    }
    tkp.PrivilegeCount = 1;  // Set one privilege
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    // Adjust the token privileges to enable shutdown
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0)) {
        std::cerr << "Failed to adjust token privileges.\n";
        CloseHandle(hToken);
        return false;
    }
    // Check for any errors from AdjustTokenPrivileges
    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        std::cerr << "The token does not have the required privileges.\n";
        CloseHandle(hToken);
        return false;
    }
    // Shut down the system
    if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_FLAG_PLANNED)) {
        std::cerr << "Failed to shutdown the system.\n";
        CloseHandle(hToken);
        return false;
    }
    CloseHandle(hToken);
    return true;
}