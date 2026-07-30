#include "CRecordManager.h"
#include "Utils.h"
using namespace std;

CRecordManager::CRecordManager() {}
CRecordManager::CRecordManager(const string& path) : mFilePath(path) {}

void CRecordManager::load() {
    mRecords.clear();
    ifstream f(mFilePath);
    if (!f.is_open()) return;

    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        Record rec;
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        if (p1 == string::npos || p2 == string::npos || p3 == string::npos) continue;

        rec.playCount    = stoi(line.substr(0, p1));
        rec.playerName   = line.substr(p1 + 1, p2 - p1 - 1);
        rec.highestLevel = stoi(line.substr(p2 + 1, p3 - p2 - 1));
        rec.totalScore   = stoi(line.substr(p3 + 1));
        mRecords.push_back(rec);
    }
    f.close();
}

void CRecordManager::save() {
    ofstream f(mFilePath);
    if (!f.is_open()) return;
    for (const auto& rec : mRecords) {
        f << rec.playCount << '|' << rec.playerName << '|'
          << rec.highestLevel << '|' << rec.totalScore << '\n';
    }
    f.close();
}

void CRecordManager::addRecord(const Record& record) {
    mRecords.push_back(record);
    save();
}

vector<Record> CRecordManager::getByPlayer(const string& name) const {
    vector<Record> result;
    for (const auto& rec : mRecords) {
        if (rec.playerName == name) result.push_back(rec);
    }
    return result;
}

void CRecordManager::displayRecords(const string& playerName) {
    system("cls");
    DrawScreenBorder();
    DrawBox(15, 3, 50, 22);

    string title = "RECORDS - " + playerName;
    GotoXY(18, 5);
    SetColor(14); cout << title; ResetColor();

    vector<Record> recs = getByPlayer(playerName);

    if (recs.empty()) {
        GotoXY(18, 8);
        SetColor(8); cout << "No records yet."; ResetColor();
    } else {
        int y = 8;
        for (size_t i = 0; i < recs.size() && y < 23; i++) {
            GotoXY(18, y);
            SetColor(10);
            cout << (i + 1) << ". " << recs[i].playerName
                 << " - Level " << recs[i].highestLevel
                 << " - " << recs[i].totalScore << "p";
            ResetColor();
            y++;
        }
    }

    GotoXY(18, 23);
    SetColor(8); cout << "Press any key to go back..."; ResetColor();
    _getch();
}
