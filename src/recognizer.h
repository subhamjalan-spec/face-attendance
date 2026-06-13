#pragma once
#include <string>
#include <map>
#include <opencv2/opencv.hpp>
#include "logger.h"

class Recognizer {
public:
    // modelPath: trained LBPH model
    // logPath: attendance CSV file
    // confidenceThreshold: lower = stricter (LBPH: <70 is good match)
    Recognizer(const std::string& modelPath,
               const std::string& logPath,
               double confidenceThreshold = 120.0);

    // Start live webcam recognition loop
    void run();

private:
    std::string modelPath_;
    double confidenceThreshold_;
    std::map<int, std::string> labelMap_;
    Logger logger_;
    cv::CascadeClassifier faceCascade_;

    // Load label map from file saved during training
    bool loadLabelMap();

    // Draw bounding box and name on frame
    void drawResult(cv::Mat& frame, const cv::Rect& faceRect,
                    const std::string& name, double confidence, bool known);
};