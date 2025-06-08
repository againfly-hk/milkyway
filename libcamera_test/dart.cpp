#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

int main() {
    cv::VideoCapture cap(0); // 0 表示默认摄像头（CSI 摄像头）
    if (!cap.isOpened()) {
        std::cerr << "摄像头打开失败！检查驱动是否加载（bcm2835-v4l2）" << std::endl;
        return -1;
    }
    
    // 关键优化：降低分辨率至 640x480，大幅减少计算量
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 120);
    
    cv::Mat frame;
    
    // 帧率计算相关变量
    int frameCount = 0;
    double fps = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastPrintTime = startTime;
    
    // 跳过前几帧（摄像头预热）
    for (int i = 0; i < 5; ++i) {
        cap >> frame;
    }
    
    while (true) {
        cap >> frame; // 捕获一帧
        if (frame.empty()) break;
        
        frameCount++;
        
        // 计算当前时间
        auto currentTime = std::chrono::high_resolution_clock::now();
        
        // 每秒更新一次帧率显示
        auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastPrintTime).count();
        if (timeDiff > 1000) { // 每1秒更新一次
            fps = frameCount * 1000.0 / timeDiff;
            std::cout << "当前帧率: " << fps << " FPS" << std::endl;
            
            // 重置计数器
            frameCount = 0;
            lastPrintTime = currentTime;
        }
        
        // 在画面上显示帧率
        std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
        std::cout << "FPS: " << fps << endl;        
        // 按 ESC 退出
        if (cv::waitKey(10) == 27) break; 
    }
    
    // 计算并打印平均帧率
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    double averageFps = frameCount * 1000.0 / totalDuration;
    std::cout << "\n程序运行结束\n";
    std::cout << "总帧数: " << frameCount << std::endl;
    std::cout << "总时间: " << totalDuration << " 毫秒" << std::endl;
    std::cout << "平均帧率: " << averageFps << " FPS" << std::endl;
    
    cap.release();
    cv::destroyAllWindows();
    return 0;
}