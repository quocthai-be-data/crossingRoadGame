#pragma once
#include "Utils.h"
#include "CAccount.h"
#include "CRecordManager.h"
#include "CSaveManager.h"
#include "CSoundManager.h"
#include <string>
#include <vector>

enum GameState {
    STATE_MENU,
    STATE_NEW_GAME,
    STATE_LOAD,
    STATE_SETTINGS,
    STATE_ABOUT,
    STATE_QUIT,
    STATE_PLAYING
};

GameState runMainMenu();
std::string runNewGameScreen(std::vector<CAccount>& accounts, const std::string& playersFile);
void runSettingsScreen(CSoundManager& sound);
void runAboutScreen();
bool runQuitConfirm();
int  showExitConfirm();  // 0=cancel(No), 1=quit no save, 2=quit with save
bool showSaveDialog();
bool showPlayAgainDialog();
void showEndGameScreen(bool isWin, int score, int level);
// Death panel in bottom-right: returns 0=menu, 1=new game same name, 2=new game new name
int showDeathPanel(int score, int level, const std::string& playerName);
