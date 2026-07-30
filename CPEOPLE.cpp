#include "CPEOPLE.h"
using namespace std;

CPEOPLE::CPEOPLE() : mX(PLAYER_START_X), mY(PLAYER_START_Y), mAlive(true) {}

void CPEOPLE::reset() {
    erase();
    mX = PLAYER_START_X;
    mY = PLAYER_START_Y;
    mAlive = true;
}

void CPEOPLE::moveDir(char dir) {
    erase();
    switch (toupper(dir)) {
    case 'W': if (mY > FINISH_Y)        mY--; break;
    case 'S': if (mY < SCREEN_HEIGHT-2) mY++; break;
    case 'A': if (mX > 0)               mX--; break;
    case 'D': if (mX < SCREEN_WIDTH-2)  mX++; break;
    }
    draw();
}

bool CPEOPLE::isImpact(CVEHICLE* v) const {
    if (mY != v->getY()) return false;
    int dx = abs(mX - v->getX());
    if (dx > SCREEN_WIDTH / 2) dx = SCREEN_WIDTH - dx;
    return dx <= 1;
}
bool CPEOPLE::isImpact(CANIMAL* a) const {
    if (mY != a->getY()) return false;
    int dx = abs(mX - a->getX());
    if (dx > SCREEN_WIDTH / 2) dx = SCREEN_WIDTH - dx;
    return dx <= 1;
}
bool CPEOPLE::isFinish() const { return mY <= FINISH_Y; }
bool CPEOPLE::isDead()   const { return !mAlive; }

void CPEOPLE::setDead() {
    mAlive = false;
    for (int i = 0; i < 2; i++) {
        GotoXY(mX, mY);
        SetColor(12); cout << 'X'; ResetColor();
        Sleep(100);
        GotoXY(mX, mY); cout << ' ';
        Sleep(100);
    }
}

// KEY FIX: Restore background character instead of clearing to space
void CPEOPLE::erase() { RestoreCell(mX, mY); }

void CPEOPLE::draw() {
    GotoXY(mX, mY);
    SetColor(14); cout << 'Y'; ResetColor();
}

void CPEOPLE::save(ofstream& f) const { f << mX << ' ' << mY << ' ' << mAlive << '\n'; }
void CPEOPLE::load(ifstream& f) { f >> mX >> mY >> mAlive; }
int CPEOPLE::getX() const { return mX; }
int CPEOPLE::getY() const { return mY; }
