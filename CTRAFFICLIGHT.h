#pragma once
#include "Utils.h"
#include "CVEHICLE.h"
#include <vector>
#include <fstream>

inline const int GREEN_DURATION = 20;
inline const int RED_DURATION   = 15;

class CTRAFFICLIGHT {
    int  mX, mY;
    int  mTimer;
    bool mIsGreen;
public:
    CTRAFFICLIGHT(int x, int y);
    bool update();
    bool isGreen() const;
    void draw();
    void applyToLane(std::vector<CVEHICLE*>& lane);
    void save(std::ofstream& f) const;
    void load(std::ifstream& f);
};
