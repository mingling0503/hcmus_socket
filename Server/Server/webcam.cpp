#include "webcam.h"
#include "screenshot.h"
using namespace std;

cv::VideoCapture camera;
cv::Mat frame;
bool startWebcam(const std::string& filename) {

    if (!camera.isOpened()) {
        camera.open(0);
    }

    camera >> frame;

    if (screenshot(filename)) {
        std::cout << "Webcam started!\n";
        return true;
    }

    return false;
}

bool stopWebcam() {
    if (camera.isOpened()) {
        camera.release();  // Release the webcam
        cv::destroyAllWindows();  // Close all OpenCV windows
    }
    if (camera.isOpened()) return false;
    return true;
}

//void stopWebcam(const std::string& filename) {
//    std::ofstream f(filename); 
//
//    if (!f.is_open()) {
//        std::cerr << "Error: Unable to open the file " << filename << " for writing.\n";
//        return;
//    }
//
//    if (camera.isOpened()) {
//        camera.release(); 
//        cv::destroyAllWindows(); 
//        f << "Webcam stopped successfully.\n";
//    }
//    else {
//        f << "Webcam is not running.\n";
//    }
//
//    f.close();
//}

