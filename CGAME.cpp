#include "CGAME.h"
#include <limits>
using namespace std;

CGAME::CGAME()
    : mLightCar(SCREEN_WIDTH - 3, LANE_CAR_Y - 1),
      mLightTruck(SCREEN_WIDTH - 3, LANE_TRUCK_Y - 1),
      mLevel(1), mFrameCount(0),
      mLevelScore(MAX_SCORE_PER_LEVEL), mTotalScore(0), mDeathCount(0),
      mPlayCount(0), mSound(nullptr) {}

int CGAME::vehicleCount() const { return BASE_VEHICLE + mLevel - 1; }

void CGAME::initObjects() {
    int count = vehicleCount();
    int spacing = SCREEN_WIDTH / (count + 1);
    mCars.clear(); mTrucks.clear(); mBirds.clear(); mDinos.clear();
    for (int i = 0; i < count; i++) {
        int offset = (i % 2 == 0) ? 0 : spacing / 2;  // Stagger positions to reduce overlap
        mCars.emplace_back((i * spacing + offset) % SCREEN_WIDTH, LANE_CAR_Y, 1);
        mTrucks.emplace_back((i * spacing + spacing / 3) % SCREEN_WIDTH, LANE_TRUCK_Y, -1);
        mBirds.emplace_back((i * spacing + offset) % SCREEN_WIDTH, LANE_BIRD_Y, 1);
        mDinos.emplace_back((i * spacing + spacing / 3) % SCREEN_WIDTH, LANE_DINO_Y, -1);
    }
}

void CGAME::drawLayout() {
    // White border frame around the game area
    DrawScreenBorder();
    // Finish line
    SetColor(10);
    for (int x = 0; x < SCREEN_WIDTH; x++) { GotoXY(x, FINISH_Y); cout << '='; }
    // Lane borders
    SetColor(8);
    int lanes[] = { LANE_BIRD_Y-1, LANE_BIRD_Y+1, LANE_DINO_Y-1, LANE_DINO_Y+1,
                    LANE_CAR_Y-1, LANE_CAR_Y+1, LANE_TRUCK_Y-1, LANE_TRUCK_Y+1 };
    for (int lane : lanes)
        for (int x = 0; x < SCREEN_WIDTH; x++) { GotoXY(x, lane); cout << '-'; }
    ResetColor();
    // HUD
    GotoXY(2, 0);
    SetColor(11); cout << "LEVEL: " << mLevel; ResetColor();
    GotoXY(15, 0);
    SetColor(13); cout << "SCORE: " << getCurrentFullScore(); ResetColor();
    GotoXY(32, 0);
    SetColor(7); cout << "WASD:move ESC:quit P:pause L:save R:records"; ResetColor();
}

void CGAME::startGame() {
    mLevel = 1;
    mTotalScore = 0;
    mLevelScore = MAX_SCORE_PER_LEVEL;
    mDeathCount = 0;
    startNewLevel();
}

void CGAME::startNewLevel() {
    system("cls");
    mLevelScore = MAX_SCORE_PER_LEVEL;
    mDeathCount = 0;
    initObjects();
    drawLayout();
    mPlayer.reset();
    mPlayer.draw();
    mFrameCount = 0;
}

void CGAME::resetLevel() {
    // Retry same level after death (mLevelScore already decremented)
    system("cls");
    initObjects();
    drawLayout();
    mPlayer.reset();
    mPlayer.draw();
    mFrameCount = 0;
}

void CGAME::resetFullGame() { startGame(); }

void CGAME::updatePosVehicle() {
    // Build pointer lanes for traffic light control
    vector<CVEHICLE*> carLane, truckLane;
    for (auto& c : mCars)   carLane.push_back(&c);
    for (auto& t : mTrucks) truckLane.push_back(&t);

    // Traffic lights only affect vehicles, NOT animals
    if (mLightCar.update())   mLightCar.applyToLane(carLane);
    if (mLightTruck.update()) mLightTruck.applyToLane(truckLane);
    mLightCar.draw(); 
    mLightTruck.draw();

    // Draw-first pattern: draw at new pos, then restore old pos (no blank frame)
    for (auto& c : mCars) {
        int oldX = c.getX();
        c.update();
        if (oldX != c.getX()) { c.draw(); RestoreCell(oldX, c.getY()); }
    }
    for (auto& t : mTrucks) {
        int oldX = t.getX();
        t.update();
        if (oldX != t.getX()) { t.draw(); RestoreCell(oldX, t.getY()); }
    }
}

void CGAME::updatePosAnimal() {
    // Animals move CONTINUOUSLY - no traffic light control
    mFrameCount++;
    // Draw-first pattern: draw at new pos, then restore old pos (no blank frame)
    bool sfxOn = mSound && mSound->isSFXEnabled();
    for (auto& b : mBirds) {
        int oldX = b.getX();
        b.update();
        if (oldX != b.getX()) { b.draw(); RestoreCell(oldX, b.getY()); }
        if (sfxOn && mFrameCount % 30 == 0) b.tell();
    }
    for (auto& d : mDinos) {
        int oldX = d.getX();
        d.update();
        if (oldX != d.getX()) { d.draw(); RestoreCell(oldX, d.getY()); }
        if (sfxOn && mFrameCount % 40 == 0) d.tell();
    }
}

void CGAME::updatePosPeople(char dir) {
    if (dir != ' ') mPlayer.moveDir(dir);
}

bool CGAME::checkCollision() {
    for (auto& c : mCars)   if (mPlayer.isImpact(&c)) return true;
    for (auto& t : mTrucks) if (mPlayer.isImpact(&t)) return true;
    for (auto& b : mBirds)  if (mPlayer.isImpact(&b)) return true;
    for (auto& d : mDinos)  if (mPlayer.isImpact(&d)) return true;
    return false;
}

void CGAME::handleCollision() {
    mPlayer.setDead();
    mDeathCount++;
    mLevelScore--;
    if (mSound) mSound->playCollision();

    int attemptsLeft = mLevelScore;  // Score doubles as remaining attempts
    if (isEndGame()) {
        GotoXY(CenterX(36), SCREEN_HEIGHT / 2);
        SetColor(12); cout << "GAME OVER! No attempts remaining."; ResetColor();
    } else {
        GotoXY(CenterX(46), SCREEN_HEIGHT / 2);
        SetColor(12); cout << "YOU DIED! Deaths: " << mDeathCount << " | Attempts left: " << attemptsLeft; ResetColor();
        GotoXY(CenterX(40), SCREEN_HEIGHT / 2 + 1);
        SetColor(7); cout << "Press Y to retry, other key to quit"; ResetColor();
    }
}

void CGAME::handleFinish() {
    // Player crossed successfully!
    mTotalScore += mLevelScore;
    mLevel++;
    if (mSound) mSound->playWin();
    if (!isWinGame()) {
        startNewLevel();
    }
    // If win game, main.cpp handles the win screen
}

// SAVE FORMAT:
// line1: level
// line2: vehicleCount
// line3: mLevelScore mTotalScore mDeathCount
// line4: playerName
// line5: playCount
// then: car data, truck data, bird data, dino data, player data, light data
bool CGAME::saveGame(const string& path) {
    ofstream f(path);
    if (!f) return false;
    f << mLevel << '\n' << vehicleCount() << '\n';
    f << mLevelScore << ' ' << mTotalScore << ' ' << mDeathCount << '\n';
    f << mPlayerName << '\n';
    f << mPlayCount << '\n';
    for (auto& c : mCars)   c.save(f);
    for (auto& t : mTrucks) t.save(f);
    for (auto& b : mBirds)  b.save(f);
    for (auto& d : mDinos)  d.save(f);
    mPlayer.save(f);
    mLightCar.save(f);
    mLightTruck.save(f);
    f.close();
    return true;
}

bool CGAME::loadGame(const string& path) {
    ifstream f(path);
    if (!f) return false;
    int count;
    f >> mLevel >> count;
    f >> mLevelScore >> mTotalScore >> mDeathCount;
    f.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); // Skip to next line
    getline(f, mPlayerName);  // Handles names with spaces
    f >> mPlayCount;

    mCars.resize(count); mTrucks.resize(count);
    mBirds.resize(count); mDinos.resize(count);
    for (auto& c : mCars)   c.load(f);
    for (auto& t : mTrucks) t.load(f);
    for (auto& b : mBirds)  b.load(f);
    for (auto& d : mDinos)  d.load(f);
    mPlayer.load(f);
    mLightCar.load(f);
    mLightTruck.load(f);
    f.close();

    // Apply traffic light states to vehicles after load
    vector<CVEHICLE*> carLane, truckLane;
    for (auto& c : mCars)   carLane.push_back(&c);
    for (auto& t : mTrucks) truckLane.push_back(&t);
    mLightCar.applyToLane(carLane);
    mLightTruck.applyToLane(truckLane);

    redrawAll();
    return true;
}

bool CGAME::playerDead()   const { return mPlayer.isDead(); }
bool CGAME::playerFinish() const { return mPlayer.isFinish(); }
int  CGAME::getLevel()     const { return mLevel; }

int  CGAME::getLevelScore()       const { return mLevelScore; }
int  CGAME::getTotalScore()       const { return mTotalScore; }
int  CGAME::getCurrentFullScore() const { return mTotalScore + mLevelScore; }
bool CGAME::isEndGame()           const { return mLevelScore <= 0; }
bool CGAME::isWinGame()           const { return mLevel > MAX_LEVEL; }

void CGAME::drawScoreBoard() {
    GotoXY(15, 0);
    SetColor(13); cout << "SCORE: " << getCurrentFullScore() << "   "; ResetColor();
    GotoXY(2, 0);
    SetColor(11); cout << "LEVEL: " << mLevel << " "; ResetColor();
}

void CGAME::setPlayerInfo(const string& name, int playCount) {
    mPlayerName = name; mPlayCount = playCount;
}
string CGAME::getPlayerName() const { return mPlayerName; }
int    CGAME::getPlayCount()  const { return mPlayCount; }

Record CGAME::buildRecord() const {
    Record rec;
    rec.playCount = mPlayCount;
    rec.playerName = mPlayerName;
    rec.highestLevel = isWinGame() ? MAX_LEVEL : mLevel;  // Highest level REACHED
    rec.totalScore = mTotalScore + mLevelScore;  // Include current level score
    return rec;
}

void CGAME::setSoundManager(CSoundManager* sm) { mSound = sm; }
void CGAME::setLevel(int level) { mLevel = level; }
void CGAME::setTotalScore(int score) { mTotalScore = score; }

void CGAME::redrawAll() {
    system("cls");
    drawLayout();
    for (auto& c : mCars)   c.draw();
    for (auto& t : mTrucks) t.draw();
    for (auto& b : mBirds)  b.draw();
    for (auto& d : mDinos)  d.draw();
    mPlayer.draw();
    mLightCar.draw();
    mLightTruck.draw();
}
