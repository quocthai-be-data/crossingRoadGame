#pragma once
#include <string>
#include <vector>
#include <fstream>

class CAccount {
    std::string mName;
    int         mPlayCount;
public:
    CAccount();
    CAccount(const std::string& name, int playCount = 0);
    std::string getName() const;
    int         getPlayCount() const;
    void        incrementPlayCount();
    static std::vector<CAccount> loadAll(const std::string& path);
    static void saveAll(const std::vector<CAccount>& accounts, const std::string& path);
    static CAccount* findByName(std::vector<CAccount>& accounts, const std::string& name);
};
