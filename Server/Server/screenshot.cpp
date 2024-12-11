#include "screenshot.h"

// Function to capture the screen and store it in a cv::Mat
cv::Mat getPicture() {
    // Get the device context of the screen
    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    // Get the screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create a compatible bitmap
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenWidth, screenHeight);
    SelectObject(hMemoryDC, hBitmap);

    // Copy the screen to the bitmap
    BitBlt(hMemoryDC, 0, 0, screenWidth, screenHeight, hScreenDC, 0, 0, SRCCOPY);

    // Create a BITMAPINFO structure
    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = screenWidth;
    bi.biHeight = -screenHeight; // Negative to indicate top-down bitmap
    bi.biPlanes = 1;
    bi.biBitCount = 24; // 24-bit RGB
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // Create a Mat to store the screen capture
    cv::Mat mat(screenHeight, screenWidth, CV_8UC3);

    // Get the data from the bitmap into the Mat
    GetDIBits(hMemoryDC, hBitmap, 0, screenHeight, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Clean up
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    return mat;
}

int screenshot(const string& filename) {
    cv::Mat screen = getPicture();

    if (!screen.empty()) {

        // Save the captured screen to a file
        if (cv::imwrite(filename, screen)) {
            std::cout << "Screen capture saved as screenCapture.jpg" << std::endl;
        }
        else {
            std::cerr << "Failed to save screen capture." << std::endl;
        }

    }
    else {
        std::cerr << "Failed to capture screen." << std::endl;
    }

    return 0;
}

