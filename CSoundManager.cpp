#include "CSoundManager.h"
using namespace std;
#pragma comment(lib, "winmm.lib")

CSoundManager::CSoundManager()
    : mBGMEnabled(true), mSFXEnabled(true),
      mBGMFile(""), mMoveSound(""), mCollisionSound(""),
      mLoseSound(""), mWinSound("") {}

void CSoundManager::playBGM() {
    if (!mBGMEnabled || mBGMFile.empty()) return;
    // TODO: Uncomment and set your BGM file path
    // PlaySound(TEXT(mBGMFile.c_str()), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void CSoundManager::stopBGM() {
    // TODO: Uncomment when BGM is implemented
    // PlaySound(NULL, NULL, 0);
}

void CSoundManager::playMove() {
    if (!mSFXEnabled) return;
    if (!mMoveSound.empty()) {
        // TODO: PlaySound(TEXT(mMoveSound.c_str()), NULL, SND_FILENAME | SND_ASYNC);
    } else {
        Beep(800, 50);  // Temporary placeholder
    }
}

void CSoundManager::playCollision() {
    if (!mSFXEnabled) return;
    if (!mCollisionSound.empty()) {
        // TODO: PlaySound(TEXT(mCollisionSound.c_str()), NULL, SND_FILENAME | SND_ASYNC);
    } else {
        Beep(200, 300);  // Temporary placeholder
    }
}

void CSoundManager::playLose() {
    if (!mSFXEnabled) return;
    if (!mLoseSound.empty()) {
        // TODO: PlaySound(TEXT(mLoseSound.c_str()), NULL, SND_FILENAME | SND_ASYNC);
    } else {
        Beep(150, 500);  // Temporary placeholder
    }
}

void CSoundManager::playWin() {
    if (!mSFXEnabled) return;
    if (!mWinSound.empty()) {
        // TODO: PlaySound(TEXT(mWinSound.c_str()), NULL, SND_FILENAME | SND_ASYNC);
    } else {
        Beep(1000, 100); Beep(1200, 100); Beep(1500, 200);  // Temporary placeholder
    }
}

void CSoundManager::toggleBGM() {
    mBGMEnabled = !mBGMEnabled;
    if (!mBGMEnabled) stopBGM();
    else playBGM();
}

void CSoundManager::toggleSFX() {
    mSFXEnabled = !mSFXEnabled;
}

bool CSoundManager::isBGMEnabled() const { return mBGMEnabled; }
bool CSoundManager::isSFXEnabled() const { return mSFXEnabled; }
