#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <vector>

using namespace cv;
using namespace std;

vector<Rect> detect_green_objects(const Mat& frame) {
    vector<Rect> greenRects;
    if (frame.empty()) return greenRects;

    // 直接RGB颜色过滤 - 更高效
    Mat greenMask(frame.size(), CV_8UC1, Scalar(0));
    
    // 并行处理每个像素
    for (int y = 0; y < frame.rows; y++) {
        for (int x = 0; x < frame.cols; x++) {
            Vec3b pixel = frame.at<Vec3b>(y, x);
            uchar b = pixel[0], g = pixel[1], r = pixel[2];
            
            // 绿色检测条件：绿色通道占主导
            bool isGreen = (g > r * 1.3) && (g > b * 1.3) && (g > 50);
            greenMask.at<uchar>(y, x) = isGreen ? 255 : 0;
        }
    }

    // 查找连通区域
    vector<vector<Point>> contours;
    findContours(greenMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    
    // 筛选圆形区域
    for (const auto& contour : contours) {
        double area = contourArea(contour);
        if (area < 100) continue; // 忽略小区域
        
        // 使用最小外接圆判断圆形度
        Point2f center;
        float radius;
        minEnclosingCircle(contour, center, radius);
        
        double circleArea = CV_PI * radius * radius;
        double circularity = area / circleArea;
        
        // 圆形度阈值 (0.7-0.9)
        if (circularity > 0.75) {
            Rect bbox = boundingRect(contour);
            greenRects.push_back(bbox);
        }
    }

    return greenRects;
}

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "无法打开摄像头!" << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);   
    cv::Mat frame;
    
    int frameCount = 0;
    double fps = 0;
    auto lastPrintTime = std::chrono::high_resolution_clock::now();
    
    // 跳过前几帧让摄像头稳定
    for (int i = 0; i < 5; ++i) {
        cap >> frame;
    }
    
    while (true) {
        cap >> frame;
        if (frame.empty()) break;
        
        // 检测绿色圆形对象
        auto startDetect = std::chrono::high_resolution_clock::now();
        vector<Rect> greenObjects = detect_green_objects(frame);
        auto endDetect = std::chrono::high_resolution_clock::now();
        
        // 绘制检测结果
        for (const Rect& rect : greenObjects) {
            // 绘制边界框
            rectangle(frame, rect, Scalar(0, 255, 0), 2);
            
            // 绘制圆形标记
            Point center(rect.x + rect.width/2, rect.y + rect.height/2);
            int radius = min(rect.width, rect.height)/2;
            circle(frame, center, radius, Scalar(0, 0, 255), 2);
        }
        
        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastPrintTime).count();
        
        // 每秒更新FPS
        if (timeDiff > 1000) {
            fps = frameCount * 1000.0 / timeDiff;
            frameCount = 0;
            lastPrintTime = currentTime;
        }
        
        // 计算检测耗时
        auto detectTime = std::chrono::duration_cast<std::chrono::milliseconds>(endDetect - startDetect).count();
        
        // 显示性能信息
        std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
        std::string detectText = "Detect: " + std::to_string(detectTime) + "ms";
        std::string countText = "Objects: " + std::to_string(greenObjects.size());
        
        cv::putText(frame, fpsText, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
        cv::putText(frame, detectText, Point(10, 60), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 255), 2);
        cv::putText(frame, countText, Point(10, 90), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 0, 0), 2);
        
        cv::imshow("Green Circle Detection (Color Only)", frame);
        
        // 按ESC退出
        if (cv::waitKey(10) == 27) break; 
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}