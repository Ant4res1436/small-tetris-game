#include "tetris.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include "gui.h"

#define CELL_TINTS 8
#define CELL_SIZE_PROPORTION 26
#define OUTLINE_COLOR ((Color){240, 240, 240, 255})

static const Color minoColors[CELL_TINTS] = {
    (Color){0, 240, 240, 255},     // I
    (Color){0, 0, 240, 255},       // J
    (Color){240, 165, 0, 255},     // L
    (Color){240, 240, 0, 255},     // O
    (Color){0, 240, 0, 255},       // S
    (Color){127, 0, 127, 255},     // T
    (Color){240, 0, 0, 255},       // Z
    (Color){127, 127, 127, 255},   // Garbage
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
    
    // Draw Board
    for (int r = TETRIS_ROWS_VISIBLE - 1; r >= 0; r--) {
        for (int c = 0; c < TETRIS_COLUMNS; c++) {
            if (game->board[r][c] == EMPTY) {
                DrawTextureEx(empty, position, 0, ((float)cellSize) / ((float)empty.width), WHITE);
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
    // Draw Ghost
    int32_t offset = -ToGroundOffset(game);
    position.y += (offset * cellSize);
    const TetrisPoint *minos = MINO_TABLE[((int)game->active - 1)][game->state];
    Vector2 tempPosition;
    if (offset > 0) {
        for (int i = 0; i < TETRIS_PIECE_MINOS; i++) {
            tempPosition = position;
            tempPosition.x += (minos[i].x * cellSize); 
            tempPosition.y -= (minos[i].y * cellSize); 
            DrawTextureEx(outline, tempPosition, 0, ((float)cellSize) / ((float)skin.width), minoColors[((int)game->active - 1)]);
        }
    }
    // Draw Active 
    position.y -= (offset * cellSize);
    for (int i = 0; i < TETRIS_PIECE_MINOS; i++) {
        tempPosition = position;
        tempPosition.x += (minos[i].x * cellSize); 
        tempPosition.y -= (minos[i].y * cellSize); 
        DrawTextureEx(skin, tempPosition, 0, ((float)cellSize) / ((float)skin.width), minoColors[((int)game->active - 1)]);
    }
    // Draw Held
    position.x = (boardRect.x - TETRIS_PIECE_MINOS * cellSize);
    position.y = (boardRect.y + 2 * cellSize);
    minos = MINO_TABLE[((int)game->held - 1)][0];
    for (int i = 0; i < TETRIS_PIECE_MINOS; i++) {
        tempPosition = position;
        tempPosition.x += (minos[i].x * cellSize); 
        tempPosition.y -= (minos[i].y * cellSize); 
        DrawTextureEx(skin, tempPosition, 0, ((float)cellSize) / ((float)skin.width), minoColors[((int)game->held - 1)]);
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
}

