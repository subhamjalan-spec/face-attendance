#pragma once
#include <string>
#include <vector>
#include <map>
#include <opencv2/opencv.hpp>

class Trainer {
public:
    // dataDir: path to known_faces/ folder (subfolders = person names)
    // modelPath: where to save the trained model
    explicit Trainer(const std::string& dataDir, const std::string& modelPath);

    // Load images from subfolders and train the LBPH model
    bool train();

    // Save label->name mapping to a file alongside the model
    bool saveLabelMap() const;

    int personCount() const { return static_cast<int>(labelMap_.size()); }

private:
    std::string dataDir_;
    std::string modelPath_;
    std::vector<cv::Mat> images_;
    std::vector<int> labels_;
    std::map<int, std::string> labelMap_;

    // Load and preprocess face images from one person's folder
    bool loadPersonImages(const std::string& folderPath, int label);

    // Detect and crop face from an image using Haar cascade
    cv::Mat extractFace(const cv::Mat& img);

    cv::CascadeClassifier faceCascade_;
};