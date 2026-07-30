#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <conio.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <direct.h>
#include <atomic>
#include <mutex>
#include <cmath>

// Screen
inline const int SCREEN_WIDTH  = 80;
inline const int SCREEN_HEIGHT = 30;

// Game
inline const int MAX_LEVEL     = 5;
inline const int BASE_VEHICLE  = 2;
inline const int MAX_SCORE_PER_LEVEL = 10;
inline const int MAX_ATTEMPTS_PER_LEVEL = 10;

// Layout
inline const int PLAYER_START_X = 38;
inline const int PLAYER_START_Y = 27;
inline const int FINISH_Y       = 1;

inline const int LANE_BIRD_Y  = 5;
inline const int LANE_DINO_Y  = 9;
inline const int LANE_CAR_Y   = 13;
inline const int LANE_TRUCK_Y = 17;

// Data paths (relative to exe)
inline const std::string DATA_DIR      = "data\\";
inline const std::string PLAYERS_FILE  = "data\\players.txt";
inline const std::string RECORDS_FILE  = "data\\records.txt";
inline const std::string SAVES_DIR     = "data\\saves\\";
inline const std::string SAVES_INDEX   = "data\\saves\\saves_index.txt";
inline const std::string RULE_FILE     = "rule.txt";

// Thread control (atomic for thread safety)
extern std::atomic<bool> IS_RUNNING;
extern std::atomic<bool> IS_PAUSED;
extern std::mutex GAME_MUTEX;
