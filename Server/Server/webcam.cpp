#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include "webcam.h"

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

// Function to delete a file
void deleteFile(const std::string& filename) {
    if (std::remove(filename.c_str()) == 0) {
        std::cout << "File '" << filename << "' deleted successfully.\n";
    }
    else {
        std::cerr << "Error: Unable to delete file '" << filename << "'\n";
    }
}

// Function to get the first available video device name
std::string getVideoDeviceName() {
    // Command to list devices and save output to a file
    const std::string command = "ffmpeg -list_devices true -f dshow -i dummy > ffmpeg_output.txt 2>&1";

    // Execute the command
    std::system(command.c_str());

    // Open the saved output file
    std::ifstream file("ffmpeg_output.txt");
    std::string line;
    std::string deviceName;

    // Parse the output to find the first video device
    while (std::getline(file, line)) {
        // Search for the video device keyword
        if (line.find("(video)") != std::string::npos) {
            // Find the first quoted string containing the device name
            size_t startPos = line.find("\"") + 1;  // Position after the first quote
            size_t endPos = line.find("\"", startPos);  // Position of the closing quote
            if (startPos != std::string::npos && endPos != std::string::npos) {
                deviceName = line.substr(startPos, endPos - startPos);
                break;  // Stop after finding the first video device
            }
        }
    }

    // Return the device name
    return deviceName;
}

void startWebcam(int durationSeconds, const std::string& filename) {
    // Check if the output file already exists and delete it
    if (fileExists(filename)) {
        std::cout << "File '" << filename << "' already exists. Deleting it...\n";
        deleteFile(filename);
    }

    // Try the default video device
    std::string deviceName = "Integrated Camera";
    bool success = false;

    // Function to build the FFmpeg command
    auto buildCommand = [&](const std::string& device) {
        return "ffmpeg -f dshow -i video=\"" + device + "\" -t 00:00:" +
            std::to_string(durationSeconds) + " -vcodec libx264 -pix_fmt yuv420p " + filename;
        };

    // Attempt to record with the default device
    std::cout << "Starting webcam recording for " << durationSeconds << " seconds...\n";
    std::string command = buildCommand(deviceName);
    if (std::system(command.c_str()) == 0) {
        success = true;
    }
    else {
        // Fallback: Get the actual device name dynamically
        std::cerr << "Error: Default device failed. Trying to detect available devices...\n";
        deviceName = getVideoDeviceName();
        if (!deviceName.empty()) {
            std::cout << "Detected video device: " << deviceName << "\n";
            command = buildCommand(deviceName);
            success = (std::system(command.c_str()) == 0);
        }
    }

    // Final status message
    if (success) {
        std::cout << "Recording finished and saved to '" << filename << "'\n";
    }
    else {
        std::cerr << "Error: Recording failed with all available devices.\n";
    }
}