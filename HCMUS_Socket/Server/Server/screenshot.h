#include <windows.h>
#include <wingdi.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

bool SaveBitmapToFile(HBITMAP hBitmap, HDC hdc, const char* filePath);
bool screenshot(const string& filename);