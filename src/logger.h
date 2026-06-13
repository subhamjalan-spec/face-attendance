#pragma once
#include <string>
#include <set>

class Logger {
public:
    explicit Logger(const std::string& logPath);

    // Mark attendance for a person (only once per session)
    void markAttendance(const std::string& name);

    // Check if already marked in this session
    bool isMarked(const std::string& name) const;

    // Print today's attendance to console
    void printSummary() const;

private:
    std::string logPath_;
    std::set<std::string> markedToday_; // prevents duplicate entries per session

    std::string getCurrentTimestamp() const;
    std::string getCurrentDate() const;
};