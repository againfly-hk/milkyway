#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <vector>

using namespace cv;
using namespace std;

vector<Vec3f> detect_green_circles(const Mat& img) {
    vector<Vec3f> circles;
    if (img.empty()) return circles;

    // 转换为HSV色彩空间进行颜色分割
    Mat hsv, mask;
    cvtColor(img, hsv, COLOR_BGR2HSV);
    
    // 定义绿色范围 (H:35-85, S>50, V>50)
    Scalar lower_green(35, 50, 50);
    Scalar upper_green(85, 255, 255);
    inRange(hsv, lower_green, upper_green, mask);

    // 形态学优化（可选）
    // Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
    // morphologyEx(mask, mask, MORPH_OPEN, kernel);

    // 高斯模糊降噪
    Mat blurred;
    GaussianBlur(mask, blurred, Size(9,9), 2, 2);

    // 霍夫圆检测
    HoughCircles(blurred, circles, HOUGH_GRADIENT, 
                 1,   // 图像尺度
                 30,  // 圆心最小间距
                 100, // Canny高阈值
                 20,  // 累加器阈值
                 5,   // 最小半径
                 50   // 最大半径
    );

    return circles;
}

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "error" << std::endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);   
    cv::Mat frame;
    
    int frameCount = 0;
    double fps = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastPrintTime = startTime;
    
    // 跳过前几帧让摄像头稳定
    for (int i = 0; i < 5; ++i) {
        cap >> frame;
    }
    
    while (true) {
        cap >> frame;
        if (frame.empty()) break;
        
        // 检测绿色圆形
        auto startDetect = std::chrono::high_resolution_clock::now();
        vector<Vec3f> green_circles = detect_green_circles(frame);
        auto endDetect = std::chrono::high_resolution_clock::now();
        
        // 绘制检测结果
        for (const Vec3f& c : green_circles) {
            Point center(cvRound(c[0]), cvRound(c[1]));
            int radius = cvRound(c[2]);
            // 绘制圆形轮廓
            circle(frame, center, radius, Scalar(0,255,0), 3);
            // 绘制圆心
            circle(frame, center, 3, Scalar(0,0,255), -1);
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
        
        // 显示FPS和检测时间
        std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
        std::string detectText = "Detect: " + std::to_string(detectTime) + "ms";
        cv::putText(frame, fpsText, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        cv::putText(frame, detectText, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);
        
        // 显示检测到的圆形数量
        std::string countText = "Circles: " + std::to_string(green_circles.size());
        cv::putText(frame, countText, cv::Point(10, 90), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 0, 0), 2);
        
        cv::imshow("Green Circle Detection", frame);
        
        // 按ESC退出
        if (cv::waitKey(10) == 27) break; 
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}