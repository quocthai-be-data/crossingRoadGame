#include "CGAME.h"
#include "Menu.h"
#include "CAccount.h"
#include "CRecordManager.h"
#include "CSaveManager.h"
#include "CSoundManager.h"
using namespace std;
// Global thread control variables
std::atomic<bool> IS_RUNNING{true};
std::atomic<bool> IS_PAUSED{false};
std::mutex GAME_MUTEX;

// Global game objects
CGAME           cg;
CSoundManager   soundManager;
CRecordManager  recordManager(RECORDS_FILE);
CSaveManager    saveManager(SAVES_INDEX, SAVES_DIR);
vector<CAccount> accounts;

// === SubThread: vehicle/animal update + collision loop ===
// Player movement is handled directly in main thread for instant response
void SubThread() {
    while (IS_RUNNING) {
        if (!IS_PAUSED && !cg.playerDead() && !cg.isEndGame() && !cg.isWinGame()) {
            std::lock_guard<std::mutex> lock(GAME_MUTEX);
            cg.updatePosVehicle();
            cg.updatePosAnimal();

            if (cg.checkCollision()) {
                cg.handleCollision();
            }
            else if (cg.playerFinish()) {
                cg.handleFinish();
            }

            cg.drawScoreBoard();
        }
        Sleep(80);
    }
}

// === Handle save during gameplay ===
void handleSave() {
    IS_PAUSED = true;
    Sleep(120);

    if (showSaveDialog()) {
        SaveEntry entry;
        entry.playCount = cg.getPlayCount();
        entry.playerName = cg.getPlayerName();
        entry.level = cg.getLevel();
        entry.score = cg.getTotalScore();
        entry.saveFileName = "save_" + entry.playerName + "_"
            + to_string(entry.playCount) + "_" + to_string(entry.level) + ".dat";

        string fullPath = saveManager.prepareSave(entry);
        if (cg.saveGame(fullPath)) {
            cg.redrawAll();
            GotoXY(CenterX(24), SCREEN_HEIGHT / 2 + 2);
            SetColor(10); cout << "Game saved successfully!"; ResetColor();
            Sleep(1000);
        }
    }

    // Redraw and stay paused
    cg.redrawAll();
    GotoXY(CenterX(26), SCREEN_HEIGHT / 2);
    SetColor(11); cout << "PAUSED - Press P to resume"; ResetColor();
    // IS_PAUSED stays true; player presses P to resume
}

// === Handle records view during gameplay ===
void handleRecords() {
    IS_PAUSED = true;
    Sleep(120);
    recordManager.displayRecords(cg.getPlayerName());
    cg.redrawAll();
    GotoXY(CenterX(26), SCREEN_HEIGHT / 2);
    SetColor(11); cout << "PAUSED - Press P to resume"; ResetColor();
}

// === Main entry point ===
int main() {
    FixConsoleWindow();
    HideCursor();

    // Ensure data directories exist
    EnsureDir(DATA_DIR);
    EnsureDir(SAVES_DIR);

    // Load persistent data
    accounts = CAccount::loadAll(PLAYERS_FILE);
    recordManager.load();
    saveManager.loadIndex();
    cg.setSoundManager(&soundManager);

    // === Main loop: menu -> game -> menu ===
    while (true) {
        GameState state = runMainMenu();

        if (state == STATE_QUIT) {
            if (runQuitConfirm()) break;
            continue;
        }
        if (state == STATE_SETTINGS) {
            runSettingsScreen(soundManager);
            continue;
        }
        if (state == STATE_ABOUT) {
            runAboutScreen();
            continue;
        }

        string playerName;

        if (state == STATE_NEW_GAME) {
            string result = runNewGameScreen(accounts, PLAYERS_FILE);
            if (result.empty()) continue;

            // Check if user chose "Restore saved data"
            if (result.length() > 5 && result.substr(0, 5) == "LOAD:") {
                playerName = result.substr(5);
                // Go to load screen for this player
                string savePath = saveManager.selectAndGetPath(playerName);
                if (savePath.empty()) continue;
                // Check if user chose "Start new game" from empty save list
                if (savePath.length() > 4 && savePath.substr(0, 4) == "NEW:") {
                    // Start new game for this existing player
                    CAccount* acc = CAccount::findByName(accounts, playerName);
                    acc->incrementPlayCount();
                    CAccount::saveAll(accounts, PLAYERS_FILE);
                    cg.setPlayerInfo(playerName, acc->getPlayCount());
                    cg.startGame();
                } else {
                    if (!cg.loadGame(savePath)) continue;
                }
            } else {
                playerName = result;
                CAccount* acc = CAccount::findByName(accounts, playerName);
                acc->incrementPlayCount();
                CAccount::saveAll(accounts, PLAYERS_FILE);
                cg.setPlayerInfo(playerName, acc->getPlayCount());
                cg.startGame();
            }
        }
        else if (state == STATE_LOAD) {
            // Load game - show all saves
            string savePath = saveManager.selectAndGetPath("");
            if (savePath.empty()) continue;
            if (!cg.loadGame(savePath)) continue;
            playerName = cg.getPlayerName();
        }

        if (playerName.empty()) continue;

        // === GAME LOOP ===
        IS_RUNNING = true;
        IS_PAUSED  = false;

        thread t1(SubThread);
        bool backToMenu = false;

        while (!backToMenu) {
            // === Check win condition (non-blocking) ===
            if (cg.isWinGame() && !IS_PAUSED) {
                IS_PAUSED = true;
                Sleep(200);

                Record rec = cg.buildRecord();
                recordManager.addRecord(rec);
                showEndGameScreen(true, rec.totalScore, rec.highestLevel);

                if (showPlayAgainDialog()) {
                    // Play again - new play count
                    CAccount* acc = CAccount::findByName(accounts, playerName);
                    acc->incrementPlayCount();
                    CAccount::saveAll(accounts, PLAYERS_FILE);
                    cg.setPlayerInfo(playerName, acc->getPlayCount());
                    cg.resetFullGame();
                    IS_PAUSED = false;
                } else {
                    backToMenu = true;
                }
                continue;
            }

            // === Check end game condition (non-blocking) ===
            if (cg.playerDead() && cg.isEndGame()) {
                IS_PAUSED = true;
                Sleep(200);

                if (soundManager.isSFXEnabled()) soundManager.playLose();
                Record rec = cg.buildRecord();
                recordManager.addRecord(rec);

                int deathChoice = showDeathPanel(rec.totalScore, rec.highestLevel, playerName);

                if (deathChoice == 1) {
                    // New game with same name - increment play count and restart
                    CAccount* acc = CAccount::findByName(accounts, playerName);
                    acc->incrementPlayCount();
                    CAccount::saveAll(accounts, PLAYERS_FILE);
                    cg.setPlayerInfo(playerName, acc->getPlayCount());
                    cg.resetFullGame();
                    IS_PAUSED = false;
                }
                else if (deathChoice == 2) {
                    // New game with new name - stop game loop, go to name entry
                    // We set a flag so main loop goes to runNewGameScreen
                    backToMenu = true;
                    // Use a special trick: after breaking out, we jump to name entry
                    // We'll handle this by going to menu and then to new game screen
                    IS_RUNNING = false;
                    t1.join();

                    // Show name entry screen directly
                    string result = runNewGameScreen(accounts, PLAYERS_FILE);
                    if (!result.empty() && !(result.length() > 5 && result.substr(0, 5) == "LOAD:")) {
                        playerName = result;
                        CAccount* acc = CAccount::findByName(accounts, playerName);
                        acc->incrementPlayCount();
                        CAccount::saveAll(accounts, PLAYERS_FILE);
                        cg.setPlayerInfo(playerName, acc->getPlayCount());
                        cg.resetFullGame();

                        // Restart the sub thread
                        IS_RUNNING = true;
                        IS_PAUSED = false;
                        backToMenu = false;
                        t1 = thread(SubThread);
                    }
                    else {
                        // User cancelled or chose load - go back to main menu
                        // IS_RUNNING already false, t1 already joined
                        goto endGameLoop;
                    }
                }
                else {
                    // deathChoice == 0: back to menu
                    backToMenu = true;
                }
                continue;
            }

            // === No keyboard input? Sleep briefly ===
            if (!_kbhit()) {
                Sleep(10);  // Reduced for responsive input
                continue;
            }

            char key = toupper(_getch());

            // Handle extended keys (arrows etc.) - consume and ignore
            if (key == -32 || key == 0 || (unsigned char)key == 224) {
                _getch();
                continue;
            }

            if (cg.playerDead() && !cg.isEndGame()) {
                // Player died but has attempts left
                // Death message already shown by handleCollision()
                if (key == 'Y') {
                    std::lock_guard<std::mutex> lock(GAME_MUTEX);
                    cg.resetLevel();  // retry same level
                } else {
                    // Quit early - save record of progress
                    Record rec = cg.buildRecord();
                    recordManager.addRecord(rec);
                    backToMenu = true;
                }
            }
            else if (!cg.playerDead() && !cg.isWinGame()) {
                // Normal gameplay
                if (key == 27) { // ESC - show exit confirmation
                    IS_PAUSED = true;
                    Sleep(100); // Let SubThread finish current iteration

                    int exitChoice = showExitConfirm();

                    if (exitChoice == 0) {
                        // Cancel - resume game
                        {
                            std::lock_guard<std::mutex> lock(GAME_MUTEX);
                            cg.redrawAll();
                        }
                        IS_PAUSED = false;
                    }
                    else if (exitChoice == 2) {
                        // Quit WITH save
                        SaveEntry entry;
                        entry.playCount = cg.getPlayCount();
                        entry.playerName = cg.getPlayerName();
                        entry.level = cg.getLevel();
                        entry.score = cg.getTotalScore();
                        entry.saveFileName = "save_" + entry.playerName + "_"
                            + to_string(entry.playCount) + "_" + to_string(entry.level) + ".dat";
                        string fullPath = saveManager.prepareSave(entry);
                        cg.saveGame(fullPath);

                        Record rec = cg.buildRecord();
                        recordManager.addRecord(rec);
                        backToMenu = true;
                    }
                    else {
                        // Quit WITHOUT save (exitChoice == 1)
                        Record rec = cg.buildRecord();
                        recordManager.addRecord(rec);
                        backToMenu = true;
                    }
                }
                else if (key == 'P') {
                    if (!IS_PAUSED) {
                        // Pausing
                        IS_PAUSED = true;
                        Sleep(100); // Let SubThread finish current iteration
                        GotoXY(CenterX(26), SCREEN_HEIGHT / 2);
                        SetColor(11); cout << "PAUSED - Press P to resume"; ResetColor();
                    } else {
                        // Unpausing - redraw BEFORE allowing SubThread to run
                        {
                            std::lock_guard<std::mutex> lock(GAME_MUTEX);
                            cg.redrawAll();
                        }
                        IS_PAUSED = false;
                    }
                }
                else if (key == 'L') {
                    handleSave();
                }
                else if (key == 'R') {
                    handleRecords();
                }
                else if (key == 'W' || key == 'A' || key == 'S' || key == 'D') {
                    // Direct movement - instant, with mutex protection
                    if (!IS_PAUSED) {
                        std::lock_guard<std::mutex> lock(GAME_MUTEX);
                        cg.updatePosPeople(key);
                    }
                }
            }
        }

        endGameLoop:
        // Stop SubThread gracefully (only if still running)
        if (IS_RUNNING) {
            IS_RUNNING = false;
            t1.join();
        }
    }

    return 0;
}
