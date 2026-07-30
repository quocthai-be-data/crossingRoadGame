#pragma once
#include <string>
#include <Windows.h>

class CSoundManager {
    bool mBGMEnabled;
    bool mSFXEnabled;
    // TODO: Assign your audio file paths here
    std::string mBGMFile;
    std::string mMoveSound;
    std::string mCollisionSound;
    std::string mLoseSound;
    std::string mWinSound;
public:
    CSoundManager();
    void playBGM();
    void stopBGM();
    void playMove();
    void playCollision();
    void playLose();
    void playWin();
    void toggleBGM();
    void toggleSFX();
    bool isBGMEnabled() const;
    bool isSFXEnabled() const;
};
