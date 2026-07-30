#include "CTRAFFICLIGHT.h"
using namespace std;

CTRAFFICLIGHT::CTRAFFICLIGHT(int x, int y)
    : mX(x), mY(y), mTimer(0), mIsGreen(true) {}

bool CTRAFFICLIGHT::update() {
    mTimer++;
    int limit = mIsGreen ? GREEN_DURATION : RED_DURATION;
    if (mTimer >= limit) {
        mTimer = 0;
        mIsGreen = !mIsGreen;
        return true;
    }
    return false;
}
bool CTRAFFICLIGHT::isGreen() const { return mIsGreen; }
void CTRAFFICLIGHT::draw() {
    GotoXY(mX, mY);
    if (mIsGreen) { SetColor(10); cout << 'G'; }
    else          { SetColor(12); cout << 'R'; }
    ResetColor();
}
void CTRAFFICLIGHT::applyToLane(vector<CVEHICLE*>& lane) {
    for (auto v : lane) {
        if (mIsGreen) v->resume();
        else          v->stop();
    }
}
void CTRAFFICLIGHT::save(ofstream& f) const { f << mTimer << ' ' << mIsGreen << '\n'; }
void CTRAFFICLIGHT::load(ifstream& f) { f >> mTimer >> mIsGreen; }
