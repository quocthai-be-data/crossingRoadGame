#include "Menu.h"
using namespace std;

// ========== Helper: draw menu options ==========
static void drawMenuOptions(int selected, int startY) {
    const char* options[] = { "1. New Game", "2. Load Game", "3. Settings", "4. About", "5. Quit" };
    for (int i = 0; i < 5; i++) {
        GotoXY(30, startY + i * 2);
        if (i == selected) {
            SetColor(11); cout << "> " << options[i] << "   ";
        } else {
            SetColor(7);  cout << "  " << options[i] << "   ";
        }
        ResetColor();
    }
}

// ========== Main Menu ==========
GameState runMainMenu() {
    system("cls");
    DrawScreenBorder();
    DrawBox(25, 5, 30, 16);

    GotoXY(28, 7);
    SetColor(14); cout << "CROSSING ROAD GAME"; ResetColor();

    int selected = 0;
    drawMenuOptions(selected, 10);

    while (true) {
        int key = _getch();

        if (key == '1') return STATE_NEW_GAME;
        if (key == '2') return STATE_LOAD;
        if (key == '3') return STATE_SETTINGS;
        if (key == '4') return STATE_ABOUT;
        if (key == '5') return STATE_QUIT;

        if (key == 224 || key == 0) {
            int arrow = _getch();
            if (arrow == 72 && selected > 0) selected--;       // UP
            if (arrow == 80 && selected < 4) selected++;       // DOWN
            drawMenuOptions(selected, 10);
        }
        else if (key == 13) { // Enter
            switch (selected) {
            case 0: return STATE_NEW_GAME;
            case 1: return STATE_LOAD;
            case 2: return STATE_SETTINGS;
            case 3: return STATE_ABOUT;
            case 4: return STATE_QUIT;
            }
        }
    }
}

// ========== New Game Screen ==========
string runNewGameScreen(vector<CAccount>& accounts, const string& playersFile) {
    system("cls");
    DrawScreenBorder();
    DrawBox(15, 8, 50, 12);

    GotoXY(18, 10);
    SetColor(14); cout << "NEW GAME"; ResetColor();

    GotoXY(18, 12);
    SetColor(7); cout << "Enter your name (ESC to go back):"; ResetColor();

    GotoXY(18, 13);
    ShowCursor();

    // Read name character by character
    string name = "";
    while (true) {
        int ch = _getch();
        if (ch == 27) { // ESC
            HideCursor();
            return "";
        }
        if (ch == 13) { // Enter
            if (!name.empty()) break;
        }
        else if (ch == 8) { // Backspace
            if (!name.empty()) {
                name.pop_back();
                cout << "\b \b";
            }
        }
        else if (ch >= 32 && ch <= 126 && ch != '|' && name.length() < 20) { // Printable, no pipe
            name += (char)ch;
            cout << (char)ch;
        }
    }
    HideCursor();

    if (name.empty()) return "";

    // Check if name already exists
    CAccount* existing = CAccount::findByName(accounts, name);
    if (existing) {
        GotoXY(18, 14);
        SetColor(12); cout << "This name is already registered!"; ResetColor();
        GotoXY(18, 15);
        SetColor(10); cout << "[1] Restore saved game data"; ResetColor();
        GotoXY(18, 16);
        SetColor(10); cout << "[2] Start new game with this name"; ResetColor();
        GotoXY(18, 17);
        SetColor(7); cout << "[B] Back"; ResetColor();

        while (true) {
            int key = toupper(_getch());
            if (key == '1') return "LOAD:" + name;
            if (key == '2') return name;
            if (key == 'B' || key == 27) return "";
        }
    }

    // New account
    accounts.emplace_back(name, 0);
    CAccount::saveAll(accounts, playersFile);
    return name;
}

// ========== Settings Screen ==========
void runSettingsScreen(CSoundManager& sound) {
    while (true) {
        system("cls");
    DrawScreenBorder();
        DrawBox(20, 7, 40, 14);

        GotoXY(23, 9);
        SetColor(14); cout << "SETTINGS"; ResetColor();

        GotoXY(23, 11);
        SetColor(7); cout << "[1] Background Music: ";
        if (sound.isBGMEnabled()) { SetColor(10); cout << "ON "; }
        else                      { SetColor(12); cout << "OFF"; }
        ResetColor();

        GotoXY(23, 13);
        SetColor(7); cout << "[2] Game Sounds:      ";
        if (sound.isSFXEnabled()) { SetColor(10); cout << "ON "; }
        else                      { SetColor(12); cout << "OFF"; }
        ResetColor();

        GotoXY(23, 15);
        SetColor(7); cout << "[3] Game Rules"; ResetColor();

        GotoXY(23, 17);
        SetColor(7); cout << "[B] Back"; ResetColor();

        int key = toupper(_getch());

        if (key == '1') {
            sound.toggleBGM();
        }
        else if (key == '2') {
            sound.toggleSFX();
        }
        else if (key == '3') {
            // Show game rules
            system("cls");
    DrawScreenBorder();
            DrawBox(10, 3, 60, 22);
            GotoXY(13, 5);
            SetColor(14); cout << "GAME RULES"; ResetColor();

            ifstream ruleFile(RULE_FILE);
            if (ruleFile.is_open()) {
                string line;
                int y = 8;
                while (getline(ruleFile, line) && y < 23) {
                    GotoXY(13, y);
                    SetColor(7); cout << line; ResetColor();
                    y++;
                }
                ruleFile.close();
                if (y == 8) { // file was empty
                    GotoXY(13, 8);
                    SetColor(8); cout << "Rules will be added later."; ResetColor();
                }
            } else {
                GotoXY(13, 8);
                SetColor(8); cout << "Rules will be added later."; ResetColor();
            }

            GotoXY(13, 23);
            SetColor(8); cout << "Press any key to go back..."; ResetColor();
            _getch();
        }
        else if (key == 'B' || key == 27) {
            return;
        }
    }
}

// ========== About Screen ==========
void runAboutScreen() {
    system("cls");
    DrawScreenBorder();
    DrawBox(20, 8, 40, 12);

    GotoXY(23, 10);
    SetColor(14); cout << "ABOUT"; ResetColor();

    GotoXY(23, 12);
    SetColor(11); cout << "Game: Crossing Road Game"; ResetColor();

    GotoXY(23, 13);
    SetColor(7); cout << "Group: 12"; ResetColor();

    GotoXY(23, 14);
    SetColor(10); cout << "Members: TRUONG MINH QUOC THAI"; ResetColor();

    GotoXY(23, 15);
    SetColor(10); cout << "MSSV: 24127537"; ResetColor();

    GotoXY(23, 18);
    SetColor(8); cout << "Press any key to go back..."; ResetColor();
    _getch();
}

// ========== Quit Confirmation ==========
bool runQuitConfirm() {
    DrawBox(20, 11, 40, 7);

    GotoXY(23, 13);
    SetColor(14); cout << "Do you want to exit the game?"; ResetColor();

    GotoXY(25, 15);
    SetColor(7); cout << "[Y] Yes        [N] No"; ResetColor();

    while (true) {
        int key = toupper(_getch());
        if (key == 'Y') return true;
        if (key == 'N' || key == 27) return false;
    }
}

// ========== In-Game: Exit Confirmation (ESC) ==========
// Returns: 0 = cancel (go back to game), 1 = quit without saving, 2 = quit with save
int showExitConfirm() {
    DrawBox(18, 10, 44, 9);

    GotoXY(24, 12);
    SetColor(14); cout << "Do you want to quit?"; ResetColor();

    GotoXY(26, 14);
    SetColor(7); cout << "[Y] Yes        [N] No"; ResetColor();

    while (true) {
        int key = toupper(_getch());
        if (key == 'N' || key == 27) return 0;  // Cancel - back to game
        if (key == 'Y') {
            // Ask if they want to save
            DrawBox(18, 10, 44, 9);

            GotoXY(22, 12);
            SetColor(14); cout << "Save game before quitting?"; ResetColor();

            GotoXY(26, 14);
            SetColor(7); cout << "[Y] Yes        [N] No"; ResetColor();

            while (true) {
                int key2 = toupper(_getch());
                if (key2 == 'Y') return 2;  // Quit with save
                if (key2 == 'N' || key2 == 27) return 1;  // Quit without save
            }
        }
    }
}

// ========== In-Game: Save Dialog ==========
bool showSaveDialog() {
    DrawBox(25, 11, 30, 7);

    GotoXY(33, 13);
    SetColor(14); cout << "Save game?"; ResetColor();

    GotoXY(30, 15);
    SetColor(7); cout << "[Y] Yes    [N] No"; ResetColor();

    while (true) {
        int key = toupper(_getch());
        if (key == 'Y') return true;
        if (key == 'N' || key == 27) return false;
    }
}

// ========== In-Game: Play Again Dialog ==========
bool showPlayAgainDialog() {
    DrawBox(18, 10, 44, 9);

    GotoXY(28, 12);
    SetColor(14); cout << "Congratulations!"; ResetColor();

    GotoXY(24, 13);
    SetColor(10); cout << "You completed all levels!"; ResetColor();

    GotoXY(30, 15);
    SetColor(7); cout << "Play again?"; ResetColor();

    GotoXY(28, 16);
    SetColor(7); cout << "[Y] Yes    [N] No"; ResetColor();

    while (true) {
        int key = toupper(_getch());
        if (key == 'Y') return true;
        if (key == 'N' || key == 27) return false;
    }
}

// ========== In-Game: End Game Screen ==========
void showEndGameScreen(bool isWin, int score, int level) {
    DrawBox(20, 9, 40, 10);

    if (isWin) {
        GotoXY(34, 11);
        SetColor(10); cout << "YOU WIN!"; ResetColor();

        GotoXY(27, 13);
        SetColor(14); cout << "Total Score: " << score << "p"; ResetColor();
    } else {
        GotoXY(33, 11);
        SetColor(12); cout << "GAME OVER"; ResetColor();

        GotoXY(27, 12);
        SetColor(7); cout << "Highest Level: " << level; ResetColor();

        GotoXY(27, 13);
        SetColor(7); cout << "Total Score: " << score << "p"; ResetColor();
    }

    GotoXY(30, 15);
    SetColor(10); cout << "Record saved!"; ResetColor();

    GotoXY(25, 16);
    SetColor(8); cout << "Press any key to continue..."; ResetColor();
    _getch();
}

// ========== In-Game: Death Panel (bottom-right corner) ==========
// Returns: 0 = back to menu, 1 = new game same name, 2 = new game new name
int showDeathPanel(int score, int level, const std::string& playerName) {
    // Panel position: bottom-right area of the game screen
    // Panel size: 30 wide x 12 tall
    const int panelW = 30;
    const int panelH = 12;
    const int panelX = SCREEN_WIDTH - panelW - 2;  // 2 cells from right edge
    const int panelY = SCREEN_HEIGHT - panelH - 2; // 2 cells from bottom edge

    // Draw the panel box
    DrawBox(panelX, panelY, panelW, panelH);

    int innerX = panelX + 2;
    int innerY = panelY + 1;

    // "YOU DIED" title - centered in the panel
    int titleLen = 8;
    int titleX = panelX + (panelW - titleLen) / 2;
    GotoXY(titleX, innerY);
    SetColor(12); cout << "YOU DIED"; ResetColor();

    // Score info
    string scoreLine = "Score: " + to_string(score) + "p";
    GotoXY(innerX, innerY + 2);
    SetColor(14); cout << scoreLine; ResetColor();

    // Level info
    string levelLine = "Died at Level: " + to_string(level);
    GotoXY(innerX, innerY + 3);
    SetColor(14); cout << levelLine; ResetColor();

    // Separator
    GotoXY(innerX, innerY + 5);
    SetColor(8);
    for (int i = 0; i < panelW - 4; i++) cout << (char)196;
    ResetColor();

    // "Want to start a new game?"
    GotoXY(innerX, innerY + 6);
    SetColor(7); cout << "Want to start a new game?"; ResetColor();

    GotoXY(innerX, innerY + 8);
    SetColor(10); cout << "[Y] Yes"; ResetColor();
    GotoXY(innerX + 12, innerY + 8);
    SetColor(12); cout << "[N] No"; ResetColor();

    // Wait for Y/N
    while (true) {
        int key = toupper(_getch());
        if (key == 'N' || key == 27) return 0;  // Back to menu
        if (key == 'Y') break;
    }

    // Clear the inner content below separator for the second question
    for (int row = innerY + 5; row < panelY + panelH - 1; row++) {
        GotoXY(panelX + 1, row);
        for (int i = 0; i < panelW - 2; i++) cout << ' ';
    }

    // Separator
    GotoXY(innerX, innerY + 5);
    SetColor(8);
    for (int i = 0; i < panelW - 4; i++) cout << (char)196;
    ResetColor();

    // Show current name
    string nameLine = "Name: " + playerName;
    if ((int)nameLine.length() > panelW - 4)
        nameLine = nameLine.substr(0, panelW - 4);
    GotoXY(innerX, innerY + 6);
    SetColor(11); cout << nameLine; ResetColor();

    // "This Name, or New Name?"
    GotoXY(innerX, innerY + 7);
    SetColor(7); cout << "Keep name or new name?"; ResetColor();

    GotoXY(innerX, innerY + 9);
    SetColor(10); cout << "[T] This name"; ResetColor();
    GotoXY(innerX + 16, innerY + 9);
    SetColor(13); cout << "[N] New"; ResetColor();

    // Wait for T/N
    while (true) {
        int key = toupper(_getch());
        if (key == 'T') return 1;  // New game, same name
        if (key == 'N') return 2;  // New game, new name
        if (key == 27) return 0;   // ESC = back to menu
    }
}
