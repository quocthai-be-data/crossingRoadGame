#include "CAccount.h"
#include <sstream>
using namespace std;

CAccount::CAccount() : mPlayCount(0) {}
CAccount::CAccount(const string& name, int playCount) : mName(name), mPlayCount(playCount) {}

string CAccount::getName()      const { return mName; }
int    CAccount::getPlayCount() const { return mPlayCount; }
void   CAccount::incrementPlayCount()  { mPlayCount++; }

vector<CAccount> CAccount::loadAll(const string& path) {
    vector<CAccount> accounts;
    ifstream f(path);
    if (!f.is_open()) return accounts;
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        size_t pos = line.rfind('|');
        if (pos != string::npos) {
            // New format: name|count
            string name = line.substr(0, pos);
            int count = stoi(line.substr(pos + 1));
            accounts.emplace_back(name, count);
        } else {
            // Legacy format: name count (only works for names without spaces)
            istringstream iss(line);
            string name; int count;
            if (iss >> name >> count) {
                accounts.emplace_back(name, count);
            }
        }
    }
    f.close();
    return accounts;
}

void CAccount::saveAll(const vector<CAccount>& accounts, const string& path) {
    ofstream f(path);
    if (!f.is_open()) return;
    for (const auto& acc : accounts) {
        f << acc.getName() << '|' << acc.getPlayCount() << '\n';
    }
    f.close();
}

CAccount* CAccount::findByName(vector<CAccount>& accounts, const string& name) {
    for (auto& acc : accounts) {
        if (acc.getName() == name) return &acc;
    }
    return nullptr;
}

