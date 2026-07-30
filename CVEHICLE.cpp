#include "CVEHICLE.h"
using namespace std;

CVEHICLE::CVEHICLE() : mX(0), mY(0), mSpeed(1), mDir(1), mSymbol(' '), mStopped(false) {}
CVEHICLE::CVEHICLE(int x, int y, int speed, int dir, char symbol)
    : mX(x), mY(y), mSpeed(speed), mDir(dir), mSymbol(symbol), mStopped(false) {}

void CVEHICLE::update() {
    if (mStopped) return;
    mX += mDir * mSpeed;
    if (mX >= SCREEN_WIDTH) mX -= SCREEN_WIDTH;
    if (mX < 0) mX += SCREEN_WIDTH;
}
void CVEHICLE::erase() { RestoreCell(mX, mY); }
void CVEHICLE::draw()  { GotoXY(mX, mY); cout << mSymbol; }
void CVEHICLE::save(ofstream& f) const { f << mX << ' ' << mY << ' ' << mSpeed << ' ' << mDir << ' ' << mSymbol << '\n'; }
void CVEHICLE::load(ifstream& f) { f >> mX >> mY >> mSpeed >> mDir >> mSymbol; }
void CVEHICLE::stop()    { mStopped = true; }
void CVEHICLE::resume()  { mStopped = false; }
bool CVEHICLE::isStopped() const { return mStopped; }
int  CVEHICLE::getX() const { return mX; }
int  CVEHICLE::getY() const { return mY; }
void CVEHICLE::setPos(int x, int y) { mX = x; mY = y; }

CCAR::CCAR() : CVEHICLE() { mSymbol = 'C'; }
CCAR::CCAR(int x, int y, int dir) : CVEHICLE(x, y, 2, dir, 'C') {}
CTRUCK::CTRUCK() : CVEHICLE() { mSymbol = 'T'; }
CTRUCK::CTRUCK(int x, int y, int dir) : CVEHICLE(x, y, 1, dir, 'T') {}
