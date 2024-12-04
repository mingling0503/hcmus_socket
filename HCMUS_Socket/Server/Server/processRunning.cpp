#include "processRunning.h"

bool getProcessRunning(const string& filename) {
    // Check if the output file is open
    ofstream fileResult(filename);
    if (!fileResult.is_open()) {
        std::cout << "Failed to open output file for writing.\n";
        return false;
    }

    // Define the command to execute
    const char* command = "tasklist";

    // Open a pipe to read the output of the command
    FILE* pipe = _popen(command, "r");
    if (!pipe) {
        std::cout << "Error opening pipe for tasklist command.\n";
        return false;
    }

    // Read the output of the command line by line
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        fileResult << buffer; // Write each line into the file
    }

    // Close the pipe and check for errors
    int returnCode = _pclose(pipe);
    if (returnCode != 0) {
        std::cout << "Tasklist command failed with return code: " << returnCode << "\n";
        return false;
    }

    std::cout << "Process list successfully saved to the output file.\n";
    fileResult.close();
    return true;
}
