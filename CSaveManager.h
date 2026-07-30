#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

struct SaveEntry {
    int         playCount;
    std::string playerName;
    int         level;
    int         score;
    std::string saveFileName;
};

class CSaveManager {
    std::vector<SaveEntry> mEntries;
    std::string mIndexPath;
    std::string mSaveDir;
public:
    CSaveManager();
    CSaveManager(const std::string& indexPath, const std::string& saveDir);
    void loadIndex();
    void saveIndex();
    std::string prepareSave(const SaveEntry& entry);
    std::string selectAndGetPath(const std::string& playerName);
    std::vector<SaveEntry> getByPlayer(const std::string& name) const;
    int displaySaveList(const std::string& playerName);
};
