#ifndef TETRIS_H
#define TETRIS_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TETRIS_ROWS 40
#define TETRIS_ROWS_VISIBLE 21
#define TETRIS_COLUMNS 10
#define TETRIS_QUEUE_LENGTH 5

#define TETRIS_PIECE_TYPES 7
#define TETRIS_PIECE_MINOS 4
#define TETRIS_ROTATION_TYPES 2
#define TETRIS_ROTATION_STATES 4
#define TETRIS_SRS_OFFSETS 5
#define TETRIS_SEVEN_BAG_SIZE 7
#define TETRIS_EXCEPTION_STATE_THRESHOLD 4
#define TETRIS_LOCK_DELAY 500.0f
#define TETRIS_LOCK_DELAY_RESET_ACTIONS 15
#define TETRIS_SCORING_TYPES 16
#define TETRIS_SOFTDROP_ACCELERATION 30

// Comment this out to enable leveling
#define TETRIS_DISABLE_LEVELING 

#define TETRIS_LINES_PER_LEVEL 10
#ifdef TETRIS_DISABLE_LEVELING
    #define TETRIS_LEVEL(lines) 1
#else
    #define TETRIS_LEVEL(lines) (((lines) / TETRIS_LINES_PER_LEVEL) + 1)
#endif

#define TETRIS_MAX_INCOMING_CHUNKS 256

#define TETRIS_DAS 166.67f
#define TETRIS_ARR 33.33f
#define TETRIS_LINE_CLEAR_DELAY 1.0f //683.33f

typedef struct {
    int32_t x;
    int32_t y;
} TetrisPoint;

#define TETRIS_START_POSITION (TetrisPoint){4, 19}
#define TETRIS_MOVE_RIGHT (TetrisPoint){1, 0}
#define TETRIS_MOVE_LEFT (TetrisPoint){-1, 0}
#define TETRIS_MOVE_DOWN (TetrisPoint){0, -1}

typedef enum {
    EMPTY = 0,
    I,
    J,
    L,
    O,
    S,
    T,
    Z,
    GARBAGE,
    AWAITING_CLEAR
} TetrisPiece;

typedef enum {
    NONE = 0,
    SINGLE,
    DOUBLE,
    TRIPLE,
    TETRIS,
    MINI_TSPIN_NONE,
    MINI_TSPIN_SINGLE,
    MINI_TSPIN_DOUBLE,
    TSPIN_NONE,
    TSPIN_SINGLE,
    TSPIN_DOUBLE,
    TSPIN_TRIPLE,
    B2B,
    COMBO,
    SOFTDROP,
    HARDDROP,
} TetrisClear;

#define DEFAULT_BAG { I, J, L, O, S, T, Z}

typedef struct {
    uint32_t seed;
} TetrisRng;

typedef enum {
    STARTING = 0,
    RUNNING,
    WAITING,
    WON,
    LOST,
} TetrisState;

typedef struct TetrisGame TetrisGame;
typedef struct TetrisDasMovement TetrisDasMovement;

typedef struct TetrisDasMovement {
    bool enabled;
    float dasTimer;
    float arrTimer;
    bool (*move)(TetrisGame *);
} TetrisDasMovement;

typedef struct TetrisGame {
    // General board and queue
    TetrisPiece board[TETRIS_ROWS][TETRIS_COLUMNS];
    TetrisPiece currentBag[TETRIS_SEVEN_BAG_SIZE];
    TetrisPiece nextBag[TETRIS_SEVEN_BAG_SIZE];
    uint32_t next;
    TetrisRng bagRng;
    // Active TetrisPiece
    TetrisPiece active;
    TetrisPoint position;
    uint32_t state;
    bool rotatedLast;
    bool kickException;
    float lockDelay;
    uint32_t remainingLockResetActions;
    uint32_t traveledSoftdrop;
    uint32_t traveledHarddrop;
    float gravityTimer;
    // Hold
    TetrisPiece held;
    bool canHold;
    // Gravity
    bool gravityAcceleration;
    float gravityInterval;
    // Control
    TetrisDasMovement *activeDas;
    TetrisDasMovement left;
    TetrisDasMovement right;
    // Statistics
    TetrisState gameState;
    float waitDelay;
    float elapsed;
    uint32_t score;
    uint32_t placed;
    uint32_t lines;
    uint32_t attack;
    // Versus info
    bool backToBack;
    int32_t combo;
    uint32_t incoming[TETRIS_MAX_INCOMING_CHUNKS];
    uint32_t outgoing;
    TetrisRng garbageRng;
    // Useful for Graphics
    TetrisClear lastClear;
    float lastClearTime;
    bool hadBackToBack;
    bool perfectClear;
} TetrisGame;

extern const TetrisPoint MINO_TABLE[TETRIS_SEVEN_BAG_SIZE][TETRIS_ROTATION_STATES][TETRIS_PIECE_MINOS];

// Recommended to seed Seven Bag RNG properly
TetrisGame TetrisGameNew(uint32_t seed);
// Place in the game loop, frametime has to be in milliseconds
void UpdateTetrisGame(TetrisGame *game, float frametime);
void SyncGames(TetrisGame *left, TetrisGame *right);
// TETRIS_DAS Movement should only be used for Player handling
bool ToggleDasRight(TetrisGame *game);
bool ToggleDasLeft(TetrisGame *game);
// - Das Movement
// Incremential Movement useful for Bot Movement
bool MoveRight(TetrisGame *game);
bool MoveLeft(TetrisGame *game);
bool MoveDown(TetrisGame *game);
// - Incremential Movement
bool RotateClockwise(TetrisGame *game);
bool RotateCounterClockwise(TetrisGame *game);
bool Hold(TetrisGame *game);
void ToggleSoftdrop(TetrisGame *game);
TetrisClear Harddrop(TetrisGame *game);
// Useful for drawing ghost TetrisPiece on screen
int32_t ToGroundOffset(TetrisGame *game);
float GetPPS(TetrisGame *game);
float GetAPM(TetrisGame *game);

#endif