#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
    
struct Record {
    int         playCount;
    std::string playerName;
    int         highestLevel;
    int         totalScore;
};

class CRecordManager {
    std::vector<Record> mRecords;
    std::string mFilePath;
public:
    CRecordManager();
    CRecordManager(const std::string& path);
    void load();
    void save();
    void addRecord(const Record& record);
    std::vector<Record> getByPlayer(const std::string& name) const;
    void displayRecords(const std::string& playerName);
};
