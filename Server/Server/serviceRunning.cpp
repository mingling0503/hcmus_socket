#include "serviceRunning.h"

void listServicesToStart() {
    // Open the Service Control Manager
    SC_HANDLE scManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
    if (!scManager) {
        std::cerr << "Failed to open Service Control Manager. Error: " << GetLastError() << std::endl;
        return;
    }

    DWORD bytesNeeded = 0;
    DWORD servicesReturned = 0;
    DWORD resumeHandle = 0;

    // Query for required buffer size
    EnumServicesStatusEx(
        scManager,
        SC_ENUM_PROCESS_INFO,
        SERVICE_WIN32,
        SERVICE_INACTIVE, // Only inactive services
        nullptr,
        0,
        &bytesNeeded,
        &servicesReturned,
        &resumeHandle,
        nullptr
    );

    if (GetLastError() != ERROR_MORE_DATA) {
        std::cerr << "Failed to determine buffer size. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(scManager);
        return;
    }

    // Allocate the buffer
    std::vector<BYTE> buffer(bytesNeeded);
    ENUM_SERVICE_STATUS_PROCESS* services = reinterpret_cast<ENUM_SERVICE_STATUS_PROCESS*>(buffer.data());

    if (!EnumServicesStatusEx(
        scManager,
        SC_ENUM_PROCESS_INFO,
        SERVICE_WIN32,
        SERVICE_INACTIVE, // Only inactive services
        buffer.data(),
        bytesNeeded,
        &bytesNeeded,
        &servicesReturned,
        &resumeHandle,
        nullptr)) {
        std::cerr << "Failed to enumerate services. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(scManager);
        return;
    }

    // Open the output file
    std::ofstream outfile("servicesToStart.txt");
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
        CloseServiceHandle(scManager);
        return;
    }

    // Write header to the file
    outfile << std::setw(50) << std::left << "Service Name"
        << std::setw(50) << "Display Name"
        << std::endl;
    outfile << std::string(100, '-') << std::endl;

    // Process and write each service's information
    for (DWORD i = 0; i < servicesReturned; i++) {
        std::wstring serviceNameW(services[i].lpServiceName);
        std::wstring displayNameW(services[i].lpDisplayName);

        // Convert wide-character strings to standard strings
        std::string serviceName(serviceNameW.begin(), serviceNameW.end());
        std::string displayName(displayNameW.begin(), displayNameW.end());

        outfile << std::setw(50) << std::left << serviceName
            << std::setw(50) << displayName
            << std::endl;
    }

    // Close handles and file
    outfile.close();
    CloseServiceHandle(scManager);

    std::cout << "Inactive services that can be started have been saved to servicesToStart.txt." << std::endl;
}

bool getServiceRunning(const std::string& filename) {
    // Open the Service Control Manager
    listServicesToStart();
    SC_HANDLE scManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
    if (!scManager) {
        std::cerr << "Failed to open Service Control Manager. Error: " << GetLastError() << std::endl;
        return false;
    }

    DWORD bytesNeeded = 0;
    DWORD servicesReturned = 0;
    DWORD resumeHandle = 0;

    // Query for required buffer size
    EnumServicesStatusEx(
        scManager,
        SC_ENUM_PROCESS_INFO,
        SERVICE_WIN32,
        SERVICE_ACTIVE,
        nullptr,
        0,
        &bytesNeeded,
        &servicesReturned,
        &resumeHandle,
        nullptr
    );

    if (GetLastError() != ERROR_MORE_DATA) {
        std::cerr << "Failed to determine buffer size. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(scManager);
        return false;
    }

    // Allocate the buffer
    std::vector<BYTE> buffer(bytesNeeded);
    ENUM_SERVICE_STATUS_PROCESS* services = reinterpret_cast<ENUM_SERVICE_STATUS_PROCESS*>(buffer.data());

    if (!EnumServicesStatusEx(
        scManager,
        SC_ENUM_PROCESS_INFO,
        SERVICE_WIN32,
        SERVICE_ACTIVE,
        buffer.data(),
        bytesNeeded,
        &bytesNeeded,
        &servicesReturned,
        &resumeHandle,
        nullptr)) {
        std::cerr << "Failed to enumerate services. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(scManager);
        return false;
    }

    // Open the output file
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
        CloseServiceHandle(scManager);
        return false;
    }

    // Write header to the file
    outfile << std::setw(50) << std::left << "Service Name"
        << std::setw(10) << "PID"
        << std::setw(50) << "Display Name"
        << std::endl;
    outfile << std::string(110, '-') << std::endl;

    // Process and write each service's information
    for (DWORD i = 0; i < servicesReturned; i++) {
        std::wstring serviceNameW(services[i].lpServiceName);
        std::wstring displayNameW(services[i].lpDisplayName);

        // Convert wide-character strings to standard strings
        std::string serviceName(serviceNameW.begin(), serviceNameW.end());
        std::string displayName(displayNameW.begin(), displayNameW.end());

        outfile << std::setw(50) << std::left << serviceName
            << std::setw(10) << services[i].ServiceStatusProcess.dwProcessId
            << std::setw(50) << displayName
            << std::endl;
    }

    // Close handles and file
    outfile.close();
    CloseServiceHandle(scManager);
    return true;
}




bool startService(const std::string& serviceName) {
    // Convert the service name to a wide string (LPCWSTR)
    std::wstring wideServiceName(serviceName.begin(), serviceName.end());

    std::cout << "Starting service: " << serviceName << "...\n";

    // Open a handle to the Service Control Manager
    SC_HANDLE hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!hSCManager) {
        std::cerr << "Failed to open Service Control Manager. Error: "
            << GetLastError() << std::endl;
        return false;
    }

    // Open the specified service
    SC_HANDLE hService = OpenService(hSCManager, wideServiceName.c_str(), SERVICE_START);
    if (!hService) {
        std::cerr << "Failed to open service. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(hSCManager);
        return false;
    }

    // Start the service
    if (!StartService(hService, 0, nullptr)) {
        DWORD error = GetLastError();
        if (error == ERROR_SERVICE_ALREADY_RUNNING) {
            std::cout << "The service is already running." << std::endl;
        }
        else {
            std::cerr << "Failed to start service. Error: " << error << std::endl;
        }
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        return false;
    }

    std::cout << "Service started successfully." << std::endl;

    // Clean up
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return true;
}


bool endService(const std::string& serviceName) {
    std::wstring wideServiceName(serviceName.begin(), serviceName.end());

    SC_HANDLE scManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!scManager) {
        std::cerr << "Failed to open Service Control Manager. Error: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE hService = OpenService(scManager, wideServiceName.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (!hService) {
        std::cerr << "Failed to open service. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(scManager);
        return false;
    }

    SERVICE_STATUS status;
    if (!ControlService(hService, SERVICE_CONTROL_STOP, &status)) {
        std::cerr << "Failed to stop the service. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(hService);
        CloseServiceHandle(scManager);
        return false;
    }

    std::cout << "Service " << serviceName << " stopped successfully." << std::endl;

    CloseServiceHandle(hService);
    CloseServiceHandle(scManager);

    return true;
}

