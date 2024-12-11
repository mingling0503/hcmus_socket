#include <windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
using namespace std;

cv::Mat getPicture();
int screenshot(const string& filename);