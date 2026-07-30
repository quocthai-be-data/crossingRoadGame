#include "CANIMAL.h"
using namespace std;

CANIMAL::CANIMAL() : mX(0), mY(0), mSpeed(1), mDir(1), mSymbol(' ') {}
CANIMAL::CANIMAL(int x, int y, int speed, int dir, char symbol)
    : mX(x), mY(y), mSpeed(speed), mDir(dir), mSymbol(symbol) {}

void CANIMAL::update() {
    mX += mDir * mSpeed;
    if (mX >= SCREEN_WIDTH) mX -= SCREEN_WIDTH;
    if (mX < 0) mX += SCREEN_WIDTH;
}
void CANIMAL::erase() { RestoreCell(mX, mY); }
void CANIMAL::draw()  { GotoXY(mX, mY); cout << mSymbol; }
void CANIMAL::save(ofstream& f) const { f << mX << ' ' << mY << ' ' << mSpeed << ' ' << mDir << ' ' << mSymbol << '\n'; }
void CANIMAL::load(ifstream& f) { f >> mX >> mY >> mSpeed >> mDir >> mSymbol; }
int  CANIMAL::getX() const { return mX; }
int  CANIMAL::getY() const { return mY; }
void CANIMAL::setPos(int x, int y) { mX = x; mY = y; }

CBIRD::CBIRD() : CANIMAL() { mSymbol = 'B'; }
CBIRD::CBIRD(int x, int y, int dir) : CANIMAL(x, y, 2, dir, 'B') {}
void CBIRD::tell() { thread([]() { Beep(1200, 30); }).detach(); }

CDINAUSOR::CDINAUSOR() : CANIMAL() { mSymbol = 'D'; }
CDINAUSOR::CDINAUSOR(int x, int y, int dir) : CANIMAL(x, y, 1, dir, 'D') {}
void CDINAUSOR::tell() { thread([]() { Beep(300, 50); }).detach(); }
