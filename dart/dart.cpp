#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "error!\r\n" << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);   
    cv::Mat frame;
    
    int frameCount = 0;
    double fps = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastPrintTime = startTime;
    
    for (int i = 0; i < 5; ++i) {
        cap >> frame;
    }
    
    while (true) {
        cap >> frame;
        if (frame.empty()) break;
        
        frameCount++;
  
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastPrintTime).count();
        if (timeDiff > 1000) {
            fps = frameCount * 1000.0 / timeDiff;
            frameCount = 0;
            lastPrintTime = currentTime;
        }
        
        std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
        cv::putText(frame, fpsText, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        
        cv::imshow("Camera Feed", frame);
        if (cv::waitKey(10) == 27) break; 
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}