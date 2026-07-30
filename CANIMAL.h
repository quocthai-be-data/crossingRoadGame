#pragma once
#include "Utils.h"
#include <fstream>

class CANIMAL {
protected:
    int  mX, mY;
    int  mSpeed;
    int  mDir;
    char mSymbol;
public:
    CANIMAL();
    CANIMAL(int x, int y, int speed, int dir, char symbol);
    virtual void update();
    virtual void erase();
    virtual void draw();
    virtual void tell() = 0;
    virtual void save(std::ofstream& f) const;
    virtual void load(std::ifstream& f);
    int  getX() const;
    int  getY() const;
    void setPos(int x, int y);
    virtual ~CANIMAL() = default;
};

class CBIRD : public CANIMAL {
public:
    CBIRD();
    CBIRD(int x, int y, int dir);
    void tell() override;
};

class CDINAUSOR : public CANIMAL {
public:
    CDINAUSOR();
    CDINAUSOR(int x, int y, int dir);
    void tell() override;
};
