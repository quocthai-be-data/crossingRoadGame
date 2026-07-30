#pragma once
#include "CPEOPLE.h"
#include "CVEHICLE.h"
#include "CANIMAL.h"
#include "CTRAFFICLIGHT.h"
#include "CSoundManager.h"
#include "CRecordManager.h"
#include <vector>
#include <fstream>
#include <string>

class CGAME {
    std::vector<CCAR>      mCars;
    std::vector<CTRUCK>    mTrucks;
    std::vector<CBIRD>     mBirds;
    std::vector<CDINAUSOR> mDinos;
    CPEOPLE                mPlayer;

    CTRAFFICLIGHT mLightCar;
    CTRAFFICLIGHT mLightTruck;

    int mLevel;
    int mFrameCount;

    int mLevelScore;
    int mTotalScore;
    int mDeathCount;

    std::string mPlayerName;
    int         mPlayCount;

    CSoundManager* mSound;

    int vehicleCount() const;

public:
    CGAME();

    void initObjects();
    void drawLayout();
    void startGame();
    void startNewLevel();
    void resetLevel();
    void resetFullGame();

    void updatePosVehicle();
    void updatePosAnimal();
    void updatePosPeople(char dir);

    bool checkCollision();
    void handleCollision();
    void handleFinish();

    bool saveGame(const std::string& path);
    bool loadGame(const std::string& path);

    bool playerDead()   const;
    bool playerFinish() const;
    int  getLevel()     const;

    int  getLevelScore()       const;
    int  getTotalScore()       const;
    int  getCurrentFullScore() const;
    bool isEndGame()           const;
    bool isWinGame()           const;
    void drawScoreBoard();

    void        setPlayerInfo(const std::string& name, int playCount);
    std::string getPlayerName() const;
    int         getPlayCount()  const;

    Record buildRecord() const;

    void setSoundManager(CSoundManager* sm);

    void setLevel(int level);
    void setTotalScore(int score);

    void redrawAll();
};
