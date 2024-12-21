#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include "screenshot.h"

// Function to save a bitmap to a file
bool saveBitmapToFile(HBITMAP hBitmap, HDC hDC, const std::string& filename) {
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = -bmp.bmHeight; // Negative to avoid flipping the image
    bi.biPlanes = 1;
    bi.biBitCount = 24; // 24-bit bitmap (RGB)
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((bmp.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmp.bmHeight;

    // Allocate memory for the bitmap data
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char* lpBitmap = (char*)GlobalLock(hDIB);

    // Retrieve the bitmap data
    GetDIBits(hDC, hBitmap, 0, (UINT)bmp.bmHeight, lpBitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Create and write the file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing." << std::endl;
        GlobalUnlock(hDIB);
        GlobalFree(hDIB);
        return false;
    }

    // Write the file headers
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = bmfHeader.bfOffBits + dwBmpSize;
    bmfHeader.bfType = 0x4D42; // "BM"

    file.write((char*)&bmfHeader, sizeof(BITMAPFILEHEADER));
    file.write((char*)&bi, sizeof(BITMAPINFOHEADER));
    file.write(lpBitmap, dwBmpSize);

    // Cleanup
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    file.close();

    return true;
}

// Function to capture the screen and save it to a file
bool screenshot(const std::string& filename) {
    // Get screen dimensions
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);

    // Get the device context of the entire screen
    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    // Create a compatible bitmap
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);
    SelectObject(hMemoryDC, hBitmap);

    // Copy the screen content to the bitmap
    BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY);

    // Save the bitmap to a file
    if (saveBitmapToFile(hBitmap, hMemoryDC, filename)) {
        std::cout << "Screen capture saved to '" << filename << "'\n";
        return true;
    }
    else {
        std::cerr << "Failed to save screen capture to file.\n";
        return false;
    }

    // Cleanup
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);
    return true;
}
