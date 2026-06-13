#include "logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

Logger::Logger(const std::string& logPath) : logPath_(logPath) {
    // Generate a unique filename for this session
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&t);
    std::ostringstream oss;
    oss << logPath << "_" << std::put_time(tm_info, "%Y-%m-%d_%H-%M-%S") << ".csv";
    logPath_ = oss.str();

    // Create new CSV with header
    std::ofstream file(logPath_);
    if (file.is_open()) {
        file << "Name,Date,Time\n";
    }
    std::cout << "[INFO] New log file: " << logPath_ << "\n";
}

void Logger::markAttendance(const std::string& name) {
    if (isMarked(name)) {
        std::cout << "[INFO] " << name << " already marked present today.\n";
        return;
    }

    std::ofstream file(logPath_, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open log file: " << logPath_ << "\n";
        return;
    }

    file << name << "," << getCurrentDate() << "," << getCurrentTimestamp() << "\n";
    markedToday_.insert(name);
    std::cout << "[ATTENDANCE] Marked: " << name << " at " << getCurrentTimestamp() << "\n";
}

bool Logger::isMarked(const std::string& name) const {
    return markedToday_.count(name) > 0;
}

void Logger::printSummary() const {
    std::cout << "\n=== Attendance Summary ===\n";
    if (markedToday_.empty()) {
        std::cout << "No attendance marked this session.\n";
        return;
    }
    for (const auto& name : markedToday_) {
        std::cout << "  ✓ " << name << "\n";
    }
    std::cout << "Total: " << markedToday_.size() << " person(s)\n";
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm_info, "%H:%M:%S");
    return oss.str();
}

std::string Logger::getCurrentDate() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm_info, "%Y-%m-%d");
    return oss.str();
}