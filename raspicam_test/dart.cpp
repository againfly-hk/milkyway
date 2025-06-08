#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    // 使用raspicam接口
    raspicam::RaspiCam_Cv cap;
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);   // 320x240可进一步提高帧率
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 120);           // 尝试设置30FPS
    cap.set(cv::CAP_PROP_FORMAT, CV_8UC3);    // BGR格式
    
    if (!cap.open()) {
        std::cerr << "无法打开树莓派摄像头!" << std::endl;
        return -1;
    }
    std::cout << "摄像头已打开，分辨率: " 
              << cap.get(cv::CAP_PROP_FRAME_WIDTH) << "x" 
              << cap.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;
    
    cv::Mat frame;
    int frameCount = 0;
    double fps = 0;
    auto lastPrintTime = std::chrono::high_resolution_clock::now();
    
    // 预热摄像头（跳过前几帧）
    for (int i = 0; i < 10; ++i) {
        cap.grab();
        cap.retrieve(frame);
    }
    
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
        
        // 捕获帧
        cap.grab();
        cap.retrieve(frame);
        if (frame.empty()) {
            std::cerr << "获取帧失败!" << std::endl;
            break;
        }
        
        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - start).count();
        auto detectTime = std::chrono::duration_cast<std::chrono::milliseconds>(detectEnd - detectStart).count();
        
        // 每秒更新FPS
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastPrintTime).count() > 1000) {
            fps = frameCount;
            frameCount = 0;
            lastPrintTime = currentTime;
            
            // 输出性能信息
            std::cout << "FPS: " << fps << std::endl;
        }

        cv::imshow("RPi Green Circle Detection", frame);
        
        // 按ESC退出
        if (cv::waitKey(1) == 27) break; 
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}