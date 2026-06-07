#include "tetris.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "gui.h"

#define CELL_TINTS 9
#define CELL_SIZE_PROPORTION 26
#define OUTLINE_COLOR ((Color){240, 240, 240, 255})
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static const Color minoColors[CELL_TINTS] = {
    (Color){0, 240, 240, 255},     // I
    (Color){0, 0, 240, 255},       // J
    (Color){240, 165, 0, 255},     // L
    (Color){240, 240, 0, 255},     // O
    (Color){0, 240, 0, 255},       // S
    (Color){127, 0, 127, 255},     // T
    (Color){240, 0, 0, 255},       // Z
    (Color){127, 127, 127, 255},   // Garbage
    (Color){255, 255, 255, 255},   // Waiting Clear
};

static const char *CLEAR_STRINGS[12] = {
    " ",
    "SINGLE",
    "DOUBLE",
    "TRIPLE",
    "TETRIS",
    "MINI\nTSPIN",
    "MINI\nTSPIN\n SINGLE",
    "MINI\nTSPIN\n DOUBLE",
    "TSPIN",
    "TSPIN\nSINGLE",
    "TSPIN\nDOUBLE",
    "TSPIN\nTRIPLE",
};

static Texture2D empty;
static Texture2D outline;
static Texture2D skin;

void GuiLoadTextures(void) {
    empty = LoadTexture("resources/skins/default_empty.png");
    
    outline = LoadTexture("resources/skins/default_outline.png");
    skin = LoadTexture("resources/skins/default.png");
    
    if (!IsTextureValid(empty) || !IsTextureValid(outline) || !IsTextureValid(skin)) {
        TraceLog(LOG_FATAL, "Failed to load textures");
        exit(EXIT_FAILURE);
    }
    if (empty.width != empty.height) {
        TraceLog(LOG_WARNING, "empty width != empty height");
    }
    if (outline.width != outline.height) {
        TraceLog(LOG_WARNING, "outline width != outline height");
    }
    if (skin.width != skin.height) {
        TraceLog(LOG_WARNING, "skin width != skin height");
    }   
}

void GuiUnloadTextures(void) {
    UnloadTexture(empty);
    UnloadTexture(outline);
    UnloadTexture(skin);
}

void DrawTetrisGames(TetrisGame *left, TetrisGame *right) {
    DrawTetrisGame(left, (Rectangle){ 0, 0, (WIDTH / 2), HEIGHT});
    DrawTetrisGame(right, (Rectangle){ (WIDTH / 2), 0, (WIDTH / 2), HEIGHT});
}
void DrawTetrisGame(TetrisGame *game, Rectangle bounds) {
    int cellSize = bounds.width / CELL_SIZE_PROPORTION;
    if (bounds.height < bounds.width) {
        cellSize = bounds.height / CELL_SIZE_PROPORTION;
    }

    Vector2 position = (Vector2) { 
        (bounds.x + (bounds.width - cellSize * TETRIS_COLUMNS) / 2),
        ((bounds.height - cellSize * TETRIS_ROWS_VISIBLE) / 3),
    };

    Rectangle boardRect = { 
        position.x,
        position.y,
        (cellSize * TETRIS_COLUMNS),
        (cellSize * TETRIS_ROWS_VISIBLE)
    };
    
    Color clearColor;
    // Draw Board
    for (int r = TETRIS_ROWS_VISIBLE - 1; r >= 0; r--) {
        for (int c = 0; c < TETRIS_COLUMNS; c++) {
            if (game->board[r][c] == EMPTY || game->board[r][c] == AWAITING_CLEAR) {
                DrawTextureEx(empty, position, 0, ((float)cellSize) / ((float)empty.width), WHITE);
                if (game->board[r][c] == AWAITING_CLEAR) {
                    clearColor = minoColors[(int)game->board[r][c] - 1];
                    clearColor = (Color){
                        clearColor.r,
                        clearColor.g,
                        clearColor.b,
                        (clearColor.a * powf((1.0f - (MIN((-(game->lastClearTime - game->elapsed)), 1.0f) / (TETRIS_LINE_CLEAR_DELAY / 1000.0f))), 1.5f))
                    };
                    DrawRectangleV(position, (Vector2){cellSize, cellSize}, clearColor);
                }
            } else {
                DrawTextureEx(skin, position, 0, ((float)cellSize) / ((float)skin.width), minoColors[(int)game->board[r][c] - 1]);
            }
            position.x += cellSize;
        }
        position.x = boardRect.x;
        position.y += cellSize;
    }
    
    position.x += (game->position.x * cellSize);
    position.y -= ((game->position.y + 1) * cellSize);
    
    int32_t offset = -ToGroundOffset(game);
    position.y += (offset * cellSize);
    const TetrisPoint *minos = MINO_TABLE[((int)game->active - 1)][game->state];
    Vector2 tempPosition;
    if (game->active != EMPTY) {
        // Draw Ghost
        if (offset > 0) {
            for (int i = 0; i < TETRIS_PIECE_MINOS; i++) {
                tempPosition = position;
                tempPosition.x += (minos[i].x * cellSize); 
                tempPosition.y -= (minos[i].y * cellSize); 
                if (game->board[game->position.y + minos[i].y][0] != AWAITING_CLEAR) {
                    DrawTextureEx(outline, tempPosition, 0, ((float)cellSize) / ((float)skin.width), minoColors[((int)game->active - 1)]);
                }
            }
        }
        // Draw Active 
        position.y -= (offset * cellSize);
        for (int i = 0; i < TETRIS_PIECE_MINOS; i++) {
            tempPosition = position;
            tempPosition.x += (minos[i].x * cellSize); 
            tempPosition.y -= (minos[i].y * cellSize); 
            if (game->board[game->position.y + minos[i].y][0] != AWAITING_CLEAR) {
                DrawTextureEx(skin, tempPosition, 0, ((float)cellSize) / ((float)skin.width), minoColors[((int)game->active - 1)]);
            }
            
        }
    }
    // Draw Held
    position.x = (boardRect.x - TETRIS_PIECE_MINOS * cellSize);
    position.y = (boardRect.y + 2 * cellSize);
    minos = MINO_TABLE[((int)game->held - 1)][0];
    if (game->held != EMPTY) {
        for (int i = 0; i < TETRIS_PIECE_MINOS; i++) {
            tempPosition = position;
            tempPosition.x += (minos[i].x * cellSize); 
            tempPosition.y -= (minos[i].y * cellSize); 
            DrawTextureEx(skin, tempPosition, 0, ((float)cellSize) / ((float)skin.width), minoColors[((int)game->held - 1)]);
        }
    }
    // Draw Queue
    int queuePiece;
    int queueMino;
    for (int i = 0; i < TETRIS_QUEUE_LENGTH; i++) {
        position.x = (boardRect.x + (TETRIS_COLUMNS + TETRIS_PIECE_MINOS - 2) * cellSize);
        position.y = (boardRect.y + (2 + 3 * i) * cellSize);
        queuePiece = (game->next + i);
        if (queuePiece < TETRIS_SEVEN_BAG_SIZE) {
            queueMino = ((int)game->currentBag[queuePiece] - 1);
        } else {
            queueMino = ((int)game->nextBag[(queuePiece % 7)] - 1);
        }
        minos = MINO_TABLE[queueMino][0];
        for (int i = 0; i < TETRIS_PIECE_MINOS; i++) {
            tempPosition = position;
            tempPosition.x += (minos[i].x * cellSize); 
            tempPosition.y -= (minos[i].y * cellSize); 
            DrawTextureEx(skin, tempPosition, 0, ((float)cellSize) / ((float)skin.width), minoColors[queueMino]);
        }
    }

    boardRect.x -= cellSize / 8;
    boardRect.y -= cellSize / 8;
    boardRect.width += cellSize / 4;
    boardRect.height += cellSize / 4;
    DrawRectangleLinesEx(boardRect, cellSize / 8, OUTLINE_COLOR);

    // Allign to bottom of board
    position.x = boardRect.x - cellSize * 1.3f;
    position.y = boardRect.y + boardRect.height - cellSize;

    // Draw Garbage
    int garbageSum = 0;
    for (int i = 0; (i < TETRIS_MAX_INCOMING_CHUNKS && game->incoming[i] > 0 && garbageSum <= 21); i++) {
        position.y -= cellSize * 0.125f;
        for (int g = 0; (g < game->incoming[i] && garbageSum <= 21); g++) {
            DrawTextureEx(skin, position, 0, ((float)cellSize * 0.9f) / ((float)skin.width), minoColors[((int)GARBAGE - 1)]);
            position.y -= (cellSize * 0.9f);
            garbageSum++;
        }
        position.y -= cellSize * 0.125f;
    }

    // Allign to bottom of board
    position.x = boardRect.x;
    position.y = boardRect.y + boardRect.height;

    float fontSize = cellSize * 1.5f;
    float spacing = fontSize / 10.f;
    // Draw Score
    position.y += cellSize * 0.25;
    char str[40];
    #ifdef TETRIS_DISABLE_LEVELING
        DrawTextEx(GetFontDefault(), "APM:", position, fontSize, spacing, WHITE);
    #else
        DrawTextEx(GetFontDefault(), "Score:", position, fontSize, spacing, WHITE);
    #endif
    position.x += cellSize * 6;
    #ifdef TETRIS_DISABLE_LEVELING
        sprintf(str, "%.1f", GetAPM(game));
    #else
        sprintf(str, "%d", game->score);
    #endif
    DrawTextEx(GetFontDefault(), str, position, fontSize, spacing, WHITE);
    // Draw PPS
    position.x = boardRect.x;
    position.y += cellSize * 1.4;
    DrawTextEx(GetFontDefault(), "PPS:", position, cellSize * 1.5, 6, WHITE);
    position.x += cellSize * 6;
    sprintf(str, "%.2f", GetPPS(game));
    DrawTextEx(GetFontDefault(), str, position, cellSize * 1.5, cellSize / 10, WHITE);
    // Draw Level or Time (if leveling disabled)
    
    position.x = boardRect.x;
    position.y += cellSize * 1.4;
    #ifdef TETRIS_DISABLE_LEVELING
        DrawTextEx(GetFontDefault(), "Time:", position, fontSize, spacing, WHITE);
    #else
        DrawTextEx(GetFontDefault(), "Level:", position, fontSize, spacing, WHITE);
    #endif
    position.x += cellSize * 6;
    #ifdef TETRIS_DISABLE_LEVELING
        if (game->elapsed >= 0) {
            uint32_t hours, minutes;
            float seconds;
            hours = game->elapsed / 3600;
            minutes = game->elapsed / 60 - hours * 60;
            seconds = (game->elapsed - minutes * 60) - (hours * 3600);
            char minutePadding[2] = "";
            if (minutes < 10) {
                sprintf(minutePadding, "0");
            }
            char secondPadding[2] = "";
            if (seconds < 10) {
                sprintf(secondPadding, "0");
            }
            if (hours > 0) {
                sprintf(str, "%d:%s%d:%s%.2f", hours, minutePadding, minutes, secondPadding, seconds);
            } else if (minutes > 0) {
                sprintf(str, "%d:%s%.2f", minutes, secondPadding, seconds);
            } else {
                sprintf(str, "%.2f", seconds);
            }
        } else {
            sprintf(str, "0.00");
        }
    #else
        sprintf(str, "%d", TETRIS_LEVEL(game->lines));
    #endif
    DrawTextEx(GetFontDefault(), str, position, fontSize, spacing, WHITE);

    // Draw FPS and Frametime
    DrawTextEx(GetFontDefault(), "FPS", (Vector2){(cellSize / 3), (cellSize / 3)}, fontSize / 2, spacing / 2, DARKGREEN);
    sprintf(str, "%d", GetFPS());
    DrawTextEx(GetFontDefault(), str, (Vector2){(cellSize * 5), (cellSize / 3)}, fontSize / 2, spacing / 2, DARKGREEN);
    DrawTextEx(GetFontDefault(), "Frametime", (Vector2){(cellSize / 3), (cellSize * 1.2)}, fontSize / 2, spacing / 2, DARKGREEN);
    sprintf(str, "%.2f", (GetFrameTime() * 1000.0f));
    DrawTextEx(GetFontDefault(), str, (Vector2){(cellSize * 5), (cellSize * 1.2)}, fontSize / 2, spacing / 2, DARKGREEN);
    DrawTextEx(GetFontDefault(), "ms", (Vector2){(cellSize * 6.6), (cellSize * 1.2)}, fontSize / 2, spacing / 2, DARKGREEN);
    
    Color textColor = YELLOW;
    Color textShadow;
    Vector2 textSize;
    if (game->elapsed < 1.0f) {
        if (game->elapsed < 0.0f) {
            sprintf(str, "%.0f", roundf(-game->elapsed + 0.5f));
        } else {
            textColor = (Color){
                textColor.r,
                textColor.g,
                textColor.b,
                (textColor.a * powf((1.0f - game->elapsed), 3.0f))
            };
            sprintf(str, "GO!");
        }
    } else if (game->gameState == WON) {
        textColor = BLUE;
        sprintf(str, "WON");
    } else if (game->gameState == LOST) {
        textColor = RED;
        sprintf(str, "LOST");
    } else if (game->perfectClear && (game->elapsed - game->lastClearTime) < 1.5f) {
        textColor = (Color){
            textColor.r,
            textColor.g,
            textColor.b,
            (textColor.a * powf((1.0f - MIN((-(game->lastClearTime - game->elapsed)) / 1.5f, 1.0f)), 2.0f))
        };
        sprintf(str, "PERFECT\n  CLEAR");
    } else {
        sprintf(str, " ");
    }
    textSize = MeasureTextEx(GetFontDefault(), str, fontSize * 2, spacing * 2);
    position.x = boardRect.x + (boardRect.width - textSize.x) / 2;
    position.y = boardRect.y + (boardRect.height - textSize.y) / 2;
    position.x += cellSize * 0.1f;
    position.y += cellSize * 0.1f;
    textShadow = (Color){0, 0, 0, textColor.a};
    if (game->gameState == RUNNING && game->perfectClear && (game->elapsed - game->lastClearTime) < 1.5f) {
        DrawTextEx(GetFontDefault(), str, position, ((fontSize / 1.8f) * 2) + 1, ((spacing / 1.8f) * 2), textShadow);
    } else {
        DrawTextEx(GetFontDefault(), str, position, fontSize * 2 + 1, spacing * 2, textShadow);
    }
    position.x -= cellSize * 0.2f;
    position.y -= cellSize * 0.2f;
    if (game->gameState == RUNNING && game->perfectClear && (game->elapsed - game->lastClearTime) < 1.5f) {
        DrawTextEx(GetFontDefault(), str, position, ((fontSize / 1.8f) * 2) + 1, ((spacing / 1.8f) * 2), textColor);
    } else {
        DrawTextEx(GetFontDefault(), str, position, fontSize * 2 + 1, spacing * 2, textColor);
    }
    
    
    // Draw Clear
    if ((game->elapsed - game->lastClearTime) < 2.0f) {
        sprintf(str, "%s", CLEAR_STRINGS[(int)game->lastClear]);
        textSize = MeasureTextEx(GetFontDefault(), str, fontSize, spacing);
        position.x = boardRect.x - (cellSize * 4) - (textSize.x / 2);
        position.y = boardRect.y + (boardRect.height / 1.33f) - (textSize.y / 2);
        clearColor = WHITE;
        if (game->hadBackToBack) {
            clearColor = YELLOW;
        }
        clearColor = (Color){
            clearColor.r,
            clearColor.g,
            clearColor.b,
            clearColor.a * (1.0f - MIN((-(game->lastClearTime - game->elapsed) / 2), 1.0f))
        };
        DrawTextEx(GetFontDefault(), str, position, fontSize, spacing, clearColor);
    
        if (game->combo > 0) {
            sprintf(str, "%d COMBO", game->combo);
            textSize = MeasureTextEx(GetFontDefault(), str, fontSize, spacing);
            position.x = boardRect.x - (cellSize * 4) - (textSize.x / 2);
            position.y = boardRect.y + (boardRect.height / 2.0f) - (textSize.y / 2);
            clearColor = WHITE;
            clearColor = (Color){
                clearColor.r,
                clearColor.g,
                clearColor.b,
                clearColor.a * (1.0f - MIN((-(game->lastClearTime - game->elapsed) / 2), 1.0f))
            };
            DrawTextEx(GetFontDefault(), str, position, fontSize, spacing, clearColor);
        }
    }
}

