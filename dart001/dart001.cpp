#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

const uchar GREEN_THRESHOLD = 200;   // 绿色通道阈值 (0-255)
const uchar MIN_RB_DIFF = 100;       // G与R/B的最小差值
const int MIN_AREA = 1;             // 最小区域面积 (像素)

int main() {
    raspicam::RaspiCam_Cv camera;
    cv::Mat frame;

    camera.set(cv::CAP_PROP_FORMAT, CV_8UC3);    // 设置图像格式为BGR
    camera.set(cv::CAP_PROP_FRAME_WIDTH, 320);   // 宽度
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, 240);  // 高度
    camera.set(cv::CAP_PROP_FPS, 120);            // 帧率

    camera.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);    // 手动曝光 (1=手动, 0=自动)
    camera.set(cv::CAP_PROP_EXPOSURE, 10);        // 曝光值 (根据环境调整)

    camera.set(cv::CAP_PROP_AUTO_WB, 0);           // 关闭自动白平衡
    camera.set(cv::CAP_PROP_WB_TEMPERATURE, 4000); // 白平衡温度 (K)

    camera.set(cv::CAP_PROP_BRIGHTNESS, 50);      // 亮度 (0-100)
    camera.set(cv::CAP_PROP_CONTRAST, 70);        // 对比度 (0-100)
    camera.set(cv::CAP_PROP_SATURATION, 80);      // 饱和度 (0-100)
    camera.set(cv::CAP_PROP_GAIN, 10);            // 增益 (0-100)
    camera.set(cv::CAP_PROP_SHARPNESS, 20);        // 锐度 (0-100)

    if (!camera.open()) {
        std::cerr << "无法打开树莓派摄像头!" << std::endl;
        return -1;
    }

    std::cout << "当前摄像头参数设置:" << std::endl;
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
    
    cv::Mat channels[3];
    cv::Mat mask(240, 320, CV_8UC1);
    std::vector<std::vector<cv::Point>> contours;

    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
        
        camera.grab();
        camera.retrieve(frame);
        if (frame.empty()) {
            std::cerr << "获取帧失败!" << std::endl;
            break;
        }
        
        cv::split(frame, channels);
        uchar *ptrB, *ptrG, *ptrR, *ptrMask;

        for (int y = 0; y < frame.rows; y++) {
            ptrB = channels[0].ptr<uchar>(y);
            ptrG = channels[1].ptr<uchar>(y);
            ptrR = channels[2].ptr<uchar>(y);
            ptrMask = mask.ptr<uchar>(y);

            for (int x = 0; x < frame.cols; x++) {
                uchar g = ptrG[x], r = ptrR[x], b = ptrB[x];
                ptrMask[x] = (g > GREEN_THRESHOLD &&
                              g - r > MIN_RB_DIFF &&
                              g - b > MIN_RB_DIFF) ? 255 : 0;
            }
        }

        contours.clear();
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (const auto& contour : contours) {
            double area = cv::contourArea(contour);
            if (area < MIN_AREA) continue;

            cv::Moments mu = cv::moments(contour);
            if (mu.m00 != 0) {
                int cx = static_cast<int>(mu.m10 / mu.m00);
                int cy = static_cast<int>(mu.m01 / mu.m00);
                cv::circle(frame, cv::Point(cx, cy), 3, cv::Scalar(0, 0, 255), -1);
                std::cout << "连通域中心: (" << cx << ", " << cy << ")" << std::endl;
            }
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