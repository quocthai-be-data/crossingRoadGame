#pragma once
#include "Utils.h"
#include "CVEHICLE.h"
#include "CANIMAL.h"
#include <fstream>

class CPEOPLE {
    int  mX, mY;
    bool mAlive;
public:
    CPEOPLE();
    void reset();
    void moveDir(char dir);
    bool isImpact(CVEHICLE* v) const;
    bool isImpact(CANIMAL*  a) const;
    bool isFinish() const;
    bool isDead()   const;
    void setDead();
    void erase();
    void draw();
    void save(std::ofstream& f) const;
    void load(std::ifstream& f);
    int getX() const;
    int getY() const;
};
