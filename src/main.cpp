#include <iostream>
#include <string>
#include <filesystem>
#include "trainer.h"
#include "recognizer.h"

namespace fs = std::filesystem;

void printUsage(const std::string& programName) {
    std::cout << "\n=== Face Attendance System ===\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " train      — Train model on known faces\n";
    std::cout << "  " << programName << " recognize  — Start live attendance tracking\n\n";
    std::cout << "Setup:\n";
    std::cout << "  1. Add face images to: data/known_faces/<person_name>/\n";
    std::cout << "     e.g. data/known_faces/rahul/photo1.jpg\n";
    std::cout << "  2. Run: ./face_attendance train\n";
    std::cout << "  3. Run: ./face_attendance recognize\n\n";
    std::cout << "Attendance log: logs/attendance.csv\n\n";
}

int main(int argc, char* argv[]) {
    // Paths (relative to build directory)
    const std::string dataDir    = "../data/known_faces";
    const std::string modelPath  = "../models/trained_model.yml";
    const std::string logPath    = "../logs/attendance.csv";

    // Ensure output directories exist
    fs::create_directories("../models");
    fs::create_directories("../logs");

    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "train") {
        std::cout << "\n[MODE] Training\n";
        Trainer trainer(dataDir, modelPath);
        if (!trainer.train()) {
            std::cerr << "[FAIL] Training failed.\n";
            return 1;
        }
        std::cout << "[DONE] Training complete. Run './face_attendance recognize' to start.\n";

    } else if (mode == "recognize") {
        std::cout << "\n[MODE] Recognition\n";
        if (!fs::exists(modelPath)) {
            std::cerr << "[ERROR] No trained model found at: " << modelPath << "\n";
            std::cerr << "        Run training first: ./face_attendance train\n";
            return 1;
        }
        Recognizer recognizer(modelPath, logPath);
        recognizer.run();

    } else {
        std::cerr << "[ERROR] Unknown mode: " << mode << "\n";
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}