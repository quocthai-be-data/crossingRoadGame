#include "CSaveManager.h"
#include "Utils.h"
using namespace std;

CSaveManager::CSaveManager() {}
CSaveManager::CSaveManager(const string& indexPath, const string& saveDir)
    : mIndexPath(indexPath), mSaveDir(saveDir) {}

void CSaveManager::loadIndex() {
    mEntries.clear();
    ifstream f(mIndexPath);
    if (!f.is_open()) return;

    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        SaveEntry entry;
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        if (p1 == string::npos || p2 == string::npos ||
            p3 == string::npos || p4 == string::npos) continue;

        entry.playCount    = stoi(line.substr(0, p1));
        entry.playerName   = line.substr(p1 + 1, p2 - p1 - 1);
        entry.level        = stoi(line.substr(p2 + 1, p3 - p2 - 1));
        entry.score        = stoi(line.substr(p3 + 1, p4 - p3 - 1));
        entry.saveFileName = line.substr(p4 + 1);
        mEntries.push_back(entry);
    }
    f.close();
}

void CSaveManager::saveIndex() {
    ofstream f(mIndexPath);
    if (!f.is_open()) return;
    for (const auto& e : mEntries) {
        f << e.playCount << '|' << e.playerName << '|'
          << e.level << '|' << e.score << '|' << e.saveFileName << '\n';
    }
    f.close();
}

string CSaveManager::prepareSave(const SaveEntry& entry) {
    mEntries.push_back(entry);
    saveIndex();
    return mSaveDir + entry.saveFileName;
}

vector<SaveEntry> CSaveManager::getByPlayer(const string& name) const {
    if (name.empty()) return mEntries;  // return all if name is empty
    vector<SaveEntry> result;
    for (const auto& e : mEntries) {
        if (e.playerName == name) result.push_back(e);
    }
    return result;
}

int CSaveManager::displaySaveList(const string& playerName) {
    system("cls");
    DrawScreenBorder();
    DrawBox(10, 3, 60, 22);

    GotoXY(13, 5);
    SetColor(14); cout << "LOAD GAME"; ResetColor();

    vector<SaveEntry> filtered = getByPlayer(playerName);

    if (filtered.empty()) {
        GotoXY(13, 8);
        SetColor(8); cout << "No saved games."; ResetColor();
        GotoXY(13, 22);
        SetColor(10); cout << "[N] Want to start a new game?"; ResetColor();
        GotoXY(13, 23);
        SetColor(7); cout << "[B] Back to menu"; ResetColor();
        while (true) {
            char key = toupper(_getch());
            if (key == 'B' || key == 27) return -1;
            if (key == 'N') return -2;  // Start new game
        }
    }

    int selected = 0;
    int maxShow = min((int)filtered.size(), 12);

    while (true) {
        // Draw list
        for (int i = 0; i < maxShow; i++) {
            GotoXY(13, 8 + i);
            if (i == selected) {
                SetColor(11); cout << "> ";
            } else {
                SetColor(7); cout << "  ";
            }
            cout << (i + 1) << ". " << filtered[i].playerName
                 << " - Level " << filtered[i].level
                 << " - " << filtered[i].score << "p   ";
            ResetColor();
        }

        GotoXY(13, 22);
        SetColor(8); cout << "Arrows + Enter to select"; ResetColor();
        GotoXY(13, 23);
        SetColor(7); cout << "[B] Back to menu"; ResetColor();

        int key = _getch();
        if (key == 224 || key == 0) {
            int arrow = _getch();
            if (arrow == 72 && selected > 0) selected--;          // UP
            if (arrow == 80 && selected < maxShow - 1) selected++; // DOWN
        }
        else if (key == 13) { // Enter
            return selected;
        }
        else if (toupper(key) == 'B' || key == 27) {
            return -1;
        }
    }
}

string CSaveManager::selectAndGetPath(const string& playerName) {
    vector<SaveEntry> filtered = getByPlayer(playerName);
    int index = displaySaveList(playerName);
    if (index == -2) {
        return "NEW:" + playerName;  // Signal to start new game
    }
    if (index >= 0 && index < (int)filtered.size()) {
        return mSaveDir + filtered[index].saveFileName;
    }
    return "";
}
