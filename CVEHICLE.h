#pragma once
#include "Utils.h"
#include <fstream>

class CVEHICLE {
protected:
    int  mX, mY;
    int  mSpeed;
    int  mDir;
    char mSymbol;
    bool mStopped;
public:
    CVEHICLE();
    CVEHICLE(int x, int y, int speed, int dir, char symbol);
    virtual void update();
    virtual void erase();
    virtual void draw();
    virtual void save(std::ofstream& f) const;
    virtual void load(std::ifstream& f);
    void stop();
    void resume();
    bool isStopped() const;
    int  getX() const;
    int  getY() const;
    void setPos(int x, int y);
    virtual ~CVEHICLE() = default;
};

class CCAR : public CVEHICLE {
public:
    CCAR();
    CCAR(int x, int y, int dir);
};

class CTRUCK : public CVEHICLE {
public:
    CTRUCK();
    CTRUCK(int x, int y, int dir);
};
