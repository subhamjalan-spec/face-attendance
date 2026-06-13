#include "recognizer.h"
#include <opencv2/face.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

Recognizer::Recognizer(const std::string& modelPath,
                       const std::string& logPath,
                       double confidenceThreshold)
    : modelPath_(modelPath),
      confidenceThreshold_(confidenceThreshold),
      logger_(logPath) {

    // Load Haar cascade
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
        std::cerr << "[ERROR] Could not load Haar cascade.\n";
    }
}

bool Recognizer::loadLabelMap() {
    std::string mapPath = modelPath_ + ".labels";
    std::ifstream file(mapPath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Label map not found: " << mapPath << "\n";
        std::cerr << "        Run training first with: ./face_attendance train\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string idStr, name;
        if (std::getline(ss, idStr, ',') && std::getline(ss, name)) {
            labelMap_[std::stoi(idStr)] = name;
        }
    }

    std::cout << "[INFO] Loaded " << labelMap_.size() << " known person(s)\n";
    return !labelMap_.empty();
}

void Recognizer::run() {
    if (!loadLabelMap()) return;

    // Load trained model
    auto model = cv::face::LBPHFaceRecognizer::create();
    try {
        model->read(modelPath_);
    } catch (const cv::Exception& e) {
        std::cerr << "[ERROR] Could not load model: " << modelPath_ << "\n";
        std::cerr << "        " << e.what() << "\n";
        return;
    }

    // Open webcam
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "[ERROR] Could not open webcam. Check camera permissions.\n";
        return;
    }

    std::cout << "[INFO] Webcam started. Press 'q' to quit.\n";
    std::cout << "[INFO] Confidence threshold: " << confidenceThreshold_ << " (lower = stricter)\n";

    cv::Mat frame, gray;

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "[WARN] Empty frame received.\n";
            break;
        }

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);

        // Detect faces
        std::vector<cv::Rect> faces;
        faceCascade_.detectMultiScale(gray, faces, 1.1, 5, 0,
                                      cv::Size(80, 80), cv::Size(400, 400));

        for (const auto& faceRect : faces) {
            cv::Mat faceROI = gray(faceRect);
            cv::Mat resized;
            cv::resize(faceROI, resized, cv::Size(100, 100));

            int label = -1;
            double confidence = 0.0;
            model->predict(resized, label, confidence);

            // In LBPH: lower confidence = better match
            bool known = (confidence < confidenceThreshold_ && label >= 0);
            std::string name = known ? labelMap_[label] : "Unknown";

            if (known && !logger_.isMarked(name)) {
                logger_.markAttendance(name);
            }

            drawResult(frame, faceRect, name, confidence, known);
        }

        // Show instructions on frame
        cv::putText(frame, "Press 'q' to quit | 's' for summary",
                    cv::Point(10, frame.rows - 15),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(200, 200, 200), 1);

        cv::imshow("Face Attendance System", frame);

        char key = static_cast<char>(cv::waitKey(30));
        if (key == 'q' || key == 27) break;  // q or ESC
        if (key == 's') logger_.printSummary();
    }

    logger_.printSummary();
    cap.release();
    cv::destroyAllWindows();
}

void Recognizer::drawResult(cv::Mat& frame, const cv::Rect& faceRect,
                             const std::string& name, double confidence, bool known) {
    cv::Scalar color = known ? cv::Scalar(0, 200, 0) : cv::Scalar(0, 0, 220);

    // Bounding box
    cv::rectangle(frame, faceRect, color, 2);

    // Label background
    std::string label = name + " (" + std::to_string(static_cast<int>(confidence)) + ")";
    int baseLine = 0;
    cv::Size textSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.6, 1, &baseLine);
    cv::Rect labelRect(faceRect.x, faceRect.y - textSize.height - 8,
                       textSize.width + 4, textSize.height + 8);
    cv::rectangle(frame, labelRect, color, cv::FILLED);

    // Name text
    cv::putText(frame, label,
                cv::Point(faceRect.x + 2, faceRect.y - 5),
                cv::FONT_HERSHEY_SIMPLEX, 0.6,
                cv::Scalar(255, 255, 255), 1);

    // Attendance status dot
    if (known) {
        cv::circle(frame,
                   cv::Point(faceRect.x + faceRect.width - 10, faceRect.y + 10),
                   6, cv::Scalar(0, 255, 0), cv::FILLED);
    }
}