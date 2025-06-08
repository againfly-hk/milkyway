#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

int main() {
    raspicam::RaspiCam_Cv camera;
    cv::Mat frame;

    camera.set(cv::CAP_PROP_FORMAT, CV_8UC3);    // 设置图像格式为BGR
    camera.set(cv::CAP_PROP_FRAME_WIDTH, 640);   // 宽度
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);  // 高度
    camera.set(cv::CAP_PROP_FPS, 30);            // 帧率

    camera.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);    // 手动曝光 (1=手动, 0=自动)
    camera.set(cv::CAP_PROP_EXPOSURE, 50);        // 曝光值 (根据环境调整)

    camera.set(cv::CAP_PROP_AUTO_WB, 0);           // 关闭自动白平衡
    camera.set(cv::CAP_PROP_WB_TEMPERATURE, 4000); // 白平衡温度 (K)

    camera.set(cv::CAP_PROP_BRIGHTNESS, 50);      // 亮度 (0-100)
    camera.set(cv::CAP_PROP_CONTRAST, 50);        // 对比度 (0-100)
    camera.set(cv::CAP_PROP_SATURATION, 50);      // 饱和度 (0-100)
    camera.set(cv::CAP_PROP_GAIN, 50);            // 增益 (0-100)
    camera.set(cv::CAP_PROP_SHARPNESS, 0);        // 锐度 (0-100)

    if (!camera.open()) {
        std::cerr << "无法打开树莓派摄像头!" << std::endl;
        return -1;
    }

    std::cout << "\n当前摄像头参数设置:" << std::endl;
    std::cout << "宽度: " << camera.get(cv::CAP_PROP_FRAME_WIDTH) << std::endl;
    std::cout << "高度: " << camera.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;
    std::cout << "帧率: " << camera.get(cv::CAP_PROP_FPS) << std::endl;
    std::cout << "格式: " << camera.get(cv::CAP_PROP_FORMAT) << std::endl;
    std::cout << "曝光: " << camera.get(cv::CAP_PROP_EXPOSURE) << std::endl;
    std::cout << "白平衡: " << camera.get(cv::CAP_PROP_WB_TEMPERATURE) << std::endl;
    std::cout << "亮度: " << camera.get(cv::CAP_PROP_BRIGHTNESS) << std::endl;
    std::cout << "对比度: " << camera.get(cv::CAP_PROP_CONTRAST) << std::endl;
    std::cout << "饱和度: " << camera.get(cv::CAP_PROP_SATURATION) << std::endl;
    std::cout << "增益: " << camera.get(cv::CAP_PROP_GAIN) << std::endl;
    std::cout << "锐度: " << camera.get(cv::CAP_PROP_SHARPNESS) << std::endl;

    int frameCount = 0;
    int fps = 0;
    auto lastPrintTime = std::chrono::high_resolution_clock::now();
    
    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
        
        camera.grab();
        camera.retrieve(frame);
        if (frame.empty()) {
            std::cerr << "获取帧失败!" << std::endl;
            break;
        }
        
        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastPrintTime).count() > 1000) {
            fps = frameCount;
            frameCount = 0;
            lastPrintTime = currentTime;
            
            std::cout << "FPS: " << fps << std::endl;
        }
        
        cv::imshow("RPi Camera Feed", frame);
        if (cv::waitKey(1) == 27) break; 
    }

    camera.release();
    cv::destroyAllWindows();
    return 0;
}