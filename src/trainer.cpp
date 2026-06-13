#include "trainer.h"
#include <opencv2/face.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

Trainer::Trainer(const std::string& dataDir, const std::string& modelPath)
    : dataDir_(dataDir), modelPath_(modelPath) {

    // Load Haar cascade — try both build dir and data dir
    std::vector<std::string> cascadePaths = {
        "haarcascade_frontalface_default.xml",
        "../data/haarcascade_frontalface_default.xml",
        "/opt/homebrew/share/opencv4/haarcascades/haarcascade_frontalface_default.xml"
    };

    for (const auto& p : cascadePaths) {
        if (faceCascade_.load(p)) {
            std::cout << "[INFO] Loaded cascade from: " << p << "\n";
            break;
        }
    }

    if (faceCascade_.empty()) {
        std::cerr << "[ERROR] Could not load Haar cascade. Check data/ folder.\n";
    }
}

bool Trainer::train() {
    if (!fs::exists(dataDir_)) {
        std::cerr << "[ERROR] Known faces directory not found: " << dataDir_ << "\n";
        return false;
    }

    int label = 0;
    for (const auto& entry : fs::directory_iterator(dataDir_)) {
        if (!entry.is_directory()) continue;

        std::string personName = entry.path().filename().string();
        if (personName.empty() || personName[0] == '.') continue;

        std::cout << "[TRAIN] Loading images for: " << personName << "\n";
        labelMap_[label] = personName;

        if (!loadPersonImages(entry.path().string(), label)) {
            std::cerr << "[WARN] No usable images for: " << personName << "\n";
        }
        label++;
    }

    if (images_.empty()) {
        std::cerr << "[ERROR] No training images found. Add images to data/known_faces/<name>/\n";
        return false;
    }

    std::cout << "[TRAIN] Training LBPH model on " << images_.size()
              << " images for " << labelMap_.size() << " person(s)...\n";

    auto model = cv::face::LBPHFaceRecognizer::create();
    model->train(images_, labels_);
    model->save(modelPath_);

    std::cout << "[TRAIN] Model saved to: " << modelPath_ << "\n";
    return saveLabelMap();
}

bool Trainer::loadPersonImages(const std::string& folderPath, int label) {
    int count = 0;
    for (const auto& file : fs::directory_iterator(folderPath)) {
        if (!file.is_regular_file()) continue;

        std::string ext = file.path().extension().string();
        if (ext != ".jpg" && ext != ".jpeg" && ext != ".png") continue;

        cv::Mat img = cv::imread(file.path().string(), cv::IMREAD_GRAYSCALE);
        if (img.empty()) continue;

        cv::Mat face = extractFace(img);
        if (face.empty()) {
            std::cerr << "[WARN] No face detected in: " << file.path().filename() << "\n";
            continue;
        }

        cv::Mat resized;
        cv::resize(face, resized, cv::Size(100, 100));
        cv::equalizeHist(resized, resized);

        images_.push_back(resized);
        labels_.push_back(label);
        count++;
    }

    std::cout << "[TRAIN]   -> Loaded " << count << " face(s)\n";
    return count > 0;
}

cv::Mat Trainer::extractFace(const cv::Mat& img) {
    if (faceCascade_.empty()) return img; // fallback: use full image

    std::vector<cv::Rect> faces;
    faceCascade_.detectMultiScale(img, faces, 1.1, 5, 0,
                                  cv::Size(80, 80), cv::Size(400, 400));

    if (faces.empty()) return cv::Mat();
    return img(faces[0]); // use largest detected face
}

bool Trainer::saveLabelMap() const {
    std::string mapPath = modelPath_ + ".labels";
    std::ofstream file(mapPath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not save label map to: " << mapPath << "\n";
        return false;
    }

    for (const auto& [id, name] : labelMap_) {
        file << id << "," << name << "\n";
    }

    std::cout << "[TRAIN] Label map saved to: " << mapPath << "\n";
    return true;
}