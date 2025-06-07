#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <vector>

using namespace cv;
using namespace std;

// 高效的颜色检测函数（针对ARM优化）
vector<Rect> detect_green_objects(const Mat& frame) {
    vector<Rect> greenRects;
    if (frame.empty()) return greenRects;

    // 创建绿色掩码（使用更高效的连续内存访问）
    Mat greenMask(frame.size(), CV_8UC1, Scalar(0));
    const int rows = frame.rows;
    const int cols = frame.cols;
    
    // 使用指针操作提高效率
    for (int y = 0; y < rows; y++) {
        const uchar* frame_ptr = frame.ptr<uchar>(y);
        uchar* mask_ptr = greenMask.ptr<uchar>(y);
        
        for (int x = 0; x < cols; x++) {
            // 直接访问像素数据（BGR顺序）
            uchar b = frame_ptr[3*x];
            uchar g = frame_ptr[3*x+1];
            uchar r = frame_ptr[3*x+2];
            
            // 优化绿色检测条件（避免浮点运算）
            bool isGreen = (g > r + 30) && (g > b + 30) && (g > 50);
            mask_ptr[x] = isGreen ? 255 : 0;
        }
    }

    // 查找连通区域
    vector<vector<Point>> contours;
    findContours(greenMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    
    // 筛选圆形区域
    for (const auto& contour : contours) {
        double area = contourArea(contour);
        if (area < 50) continue; // 忽略小区域
        
        // 使用边界框宽高比判断圆形
        Rect bbox = boundingRect(contour);
        float aspectRatio = static_cast<float>(bbox.width) / bbox.height;
        
        // 圆形度判断（0.7-1.3 宽高比认为是圆形）
        if (aspectRatio > 0.7f && aspectRatio < 1.3f) {
            greenRects.push_back(bbox);
        }
    }

    return greenRects;
}

int main() {
    // 使用raspicam接口
    raspicam::RaspiCam_Cv cap;
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);   // 320x240可进一步提高帧率
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 30);           // 尝试设置30FPS
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
        
        // 检测绿色对象
        auto detectStart = std::chrono::high_resolution_clock::now();
        vector<Rect> greenObjects = detect_green_objects(frame);
        auto detectEnd = std::chrono::high_resolution_clock::now();
        
        // 绘制检测结果
        for (const Rect& rect : greenObjects) {
            // 绘制边界框
            rectangle(frame, rect, Scalar(0, 255, 0), 2);
            
            // 绘制中心点
            Point center(rect.x + rect.width/2, rect.y + rect.height/2);
            circle(frame, center, 3, Scalar(0, 0, 255), -1);
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
            std::cout << "FPS: " << fps 
                      << " | 总耗时: " << totalTime << "ms"
                      << " | 检测耗时: " << detectTime << "ms"
                      << " | 对象数: " << greenObjects.size() << std::endl;
        }
        
        // 显示性能信息
        std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
        std::string detectText = "Detect: " + std::to_string(detectTime) + "ms";
        std::string countText = "Objects: " + std::to_string(greenObjects.size());
        
        cv::putText(frame, fpsText, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 1);
        cv::putText(frame, detectText, Point(10, 60), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 255), 1);
        cv::putText(frame, countText, Point(10, 90), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 0, 0), 1);
        
        cv::imshow("RPi Green Circle Detection", frame);
        
        // 按ESC退出
        if (cv::waitKey(1) == 27) break; 
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}