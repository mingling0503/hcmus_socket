#include "screenshot.h"

bool SaveBitmapToFile(HBITMAP hBitmap, HDC hdc, const char* filePath) {
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD cClrBits;
    HANDLE hf;
    BITMAPFILEHEADER hdr;
    PBITMAPINFOHEADER pbih;
    LPBYTE lpBits;
    DWORD dwTotal;
    DWORD cb;
    BYTE* hp;
    DWORD dwTmp;

    if (!GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bmp)) {
        std::cerr << "GetObject failed!" << std::endl;
        return false;
    }

    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits < 24)
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits));
    else
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

    pbih = (PBITMAPINFOHEADER)pbmi;
    pbih->biSize = sizeof(BITMAPINFOHEADER);
    pbih->biWidth = bmp.bmWidth;
    pbih->biHeight = bmp.bmHeight;
    pbih->biPlanes = bmp.bmPlanes;
    pbih->biBitCount = bmp.bmBitsPixel;
    pbih->biCompression = BI_RGB;
    pbih->biSizeImage = ((pbih->biWidth * cClrBits + 31) & ~31) / 8 * pbih->biHeight;

    lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
    if (!lpBits) {
        std::cerr << "GlobalAlloc failed!" << std::endl;
        return false;
    }

    if (!GetDIBits(hdc, hBitmap, 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) {
        std::cerr << "GetDIBits failed!" << std::endl;
        GlobalFree((HGLOBAL)lpBits);
        return false;
    }

    hf = CreateFileA(filePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateFile failed! Path: " << filePath << " Error: " << GetLastError() << std::endl;
        GlobalFree((HGLOBAL)lpBits);
        return false;
    }

    hdr.bfType = 0x4D42;
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;
    hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + pbih->biSize;

    if (!WriteFile(hf, &hdr, sizeof(BITMAPFILEHEADER), &dwTmp, NULL)) {
        std::cerr << "WriteFile (header) failed!" << std::endl;
        CloseHandle(hf);
        GlobalFree((HGLOBAL)lpBits);
        return false;
    }

    if (!WriteFile(hf, pbih, sizeof(BITMAPINFOHEADER), &dwTmp, NULL)) {
        std::cerr << "WriteFile (info header) failed!" << std::endl;
        CloseHandle(hf);
        GlobalFree((HGLOBAL)lpBits);
        return false;
    }

    dwTotal = pbih->biSizeImage;
    hp = lpBits;

    if (!WriteFile(hf, hp, dwTotal, &cb, NULL)) {
        std::cerr << "WriteFile (data) failed!" << std::endl;
        CloseHandle(hf);
        GlobalFree((HGLOBAL)lpBits);
        return false;
    }

    CloseHandle(hf);
    GlobalFree((HGLOBAL)lpBits);
    LocalFree(pbmi);

    return true;
}

bool screenshot(const string& filename) {
    HDC hScreenDC = GetDC(nullptr);              // Get the screen device context
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);  // Create a memory device context
    int width = GetDeviceCaps(hScreenDC, HORZRES);  // Screen width
    int height = GetDeviceCaps(hScreenDC, VERTRES); // Screen height
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height); // Create a bitmap

    // Select the new bitmap into the memory DC
    HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC, hBitmap));

    // Bit-block transfer from screen DC to memory DC
    BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

    // Restore the old bitmap
    hBitmap = static_cast<HBITMAP>(SelectObject(hMemoryDC, hOldBitmap));

    // Save the bitmap to a file
    if (SaveBitmapToFile(hBitmap, hMemoryDC, filename.c_str())) {
        std::cout << "Screenshot saved successfully!" << std::endl;
    }
    else {
        std::cout << "Failed to save screenshot." << std::endl;
        return false;
    }

    // Clean up
    DeleteDC(hMemoryDC);
    ReleaseDC(nullptr, hScreenDC);
    DeleteObject(hBitmap);
    return true;

}