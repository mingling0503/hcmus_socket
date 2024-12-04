#include "webcam.h"
#include "screenshot.h"

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

void stopWebcam() {
    if (camera.isOpened()) {
        camera.release();  // Release the webcam
        cv::destroyAllWindows();  // Close all OpenCV windows
        std::cout << "Webcam turned off and all windows closed." << std::endl;
    }
}