#include "tetris.h"

#define MILLISECONDS_PER_SECOND 1000.0f
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define GRAVITY(level) (pow(0.8 - (((double)level - 1) * 0.007), ((double)level - 1)) * MILLISECONDS_PER_SECOND)
#define GRAVITY_INSTANT_FALL_LEVEL 20
#define GRAVITY_CAP GRAVITY(GRAVITY_INSTANT_FALL_LEVEL)

#define CORNERS TETRIS_PIECE_MINOS
#define DIFFICULT_CLEAR_COUNT 6
#define PERFECT_CLEAR_TYPES 5
#define B2B_SCORE_BONUS ((double)1.5)

#define GARBAGE_TYPES 12
#define B2B_GARBAGE_BONUS 1
#define PC_GARBAGE 10
#define COMBO_RANGE 14
#define GARBAGE_RANDOMNESS 25

const TetrisPoint MINO_TABLE[TETRIS_SEVEN_BAG_SIZE][TETRIS_ROTATION_STATES][TETRIS_PIECE_MINOS] = {
    {   // I TetrisPiece
        { {-1,0},{0,0},{1,0},{2,0} },
        { {0,1},{0,0},{0,-1},{0,-2} },
        { {-2,0},{-1,0},{0,0},{1,0} },
        { {0,2},{0,1},{0,0},{0,-1} },
    },
    {   // L TetrisPiece
        { {-1,1},{-1,0},{0,0},{1,0} },
        { {1,1},{0,1},{0,0},{0,-1} },
        { {-1,0},{0,0},{1,0},{1,-1} },
        { {0,1},{0,0},{0,-1},{-1,-1} },
    },
    {   // J TetrisPiece
        { {-1,0},{0,0},{1,0},{1,1} },
        { {0,1},{0,0},{0,-1},{1,-1} },
        { {-1,-1},{-1,0},{0,0},{1,0} },
        { {-1,1},{0,1},{0,0},{0,-1} },
    },
    {   // O TetrisPiece
        { {0,0},{1,0},{1,1},{0,1} },
        { {0,0},{1,0},{1,1},{0,1} },
        { {0,0},{1,0},{1,1},{0,1} },
        { {0,0},{1,0},{1,1},{0,1} },
    },
    {   // S TetrisPiece
        { {-1,0},{0,0},{0,1},{1,1} },
        { {0,1},{0,0},{1,0},{1,-1} },
        { {-1,-1},{0,-1},{0,0},{1,0} },
        { {-1,1},{-1,0},{0,0},{0,-1} },
    },
    {   // T TetrisPiece
        { {-1,0},{0,1},{0,0},{1,0} },
        { {0,1},{0,0},{1,0},{0,-1} },
        { {-1,0},{0,-1},{0,0},{1,0} },
        { {0,1},{0,0},{-1,0},{0,-1} },
    },
    {   // Z TetrisPiece
        { {-1,1},{0,1},{0,0},{1,0} },
        { {1,1},{1,0},{0,0},{0,-1} },
        { {-1,0},{0,0},{0,-1},{1,-1} },
        { {-1,-1},{-1,0},{0,0},{0,1} },
    },
};

static const TetrisPoint SRS_TABLE_JLSTZ[TETRIS_ROTATION_TYPES][TETRIS_ROTATION_STATES][TETRIS_SRS_OFFSETS] = {
    {
        {
            (TetrisPoint){ 0, 0},
            (TetrisPoint){-1, 0},
            (TetrisPoint){-1,+1},
            (TetrisPoint){ 0,-2},
            (TetrisPoint){-1,-2},
        },
        {
            (TetrisPoint){ 0, 0},
            (TetrisPoint){+1, 0},
            (TetrisPoint){+1,-1},
            (TetrisPoint){ 0,+2},
            (TetrisPoint){+1,+2},
        },
        {
            (TetrisPoint){ 0, 0},
            (TetrisPoint){+1, 0},
            (TetrisPoint){+1,+1},
            (TetrisPoint){ 0,-2},
            (TetrisPoint){+1,-2},
        },
        {
            (TetrisPoint){ 0, 0},
            (TetrisPoint){-1, 0},
            (TetrisPoint){-1,-1},
            (TetrisPoint){ 0,+2},
            (TetrisPoint){-1,+2},
        },
    },
    {
        {
            (TetrisPoint){ 0, 0},
            (TetrisPoint){+1, 0},
            (TetrisPoint){+1,+1},
            (TetrisPoint){ 0,-2},
            (TetrisPoint){+1,-2},
        },
        {
            (TetrisPoint){ 0, 0},
            (TetrisPoint){+1, 0},
            (TetrisPoint){+1,-1},
            (TetrisPoint){ 0,+2},
            (TetrisPoint){+1,+2},
        },
        {
            (TetrisPoint){ 0, 0},
            (TetrisPoint){-1, 0},
            (TetrisPoint){-1,+1},
            (TetrisPoint){ 0,-2},
            (TetrisPoint){-1,-2},
        },
        {
            (TetrisPoint){ 0, 0},
            (TetrisPoint){-1, 0},
            (TetrisPoint){-1,-1},
            (TetrisPoint){ 0,+2},
            (TetrisPoint){-1,+2},
        },
    }
};

static const TetrisPoint SRS_TABLE_I[TETRIS_ROTATION_TYPES][TETRIS_ROTATION_STATES][TETRIS_SRS_OFFSETS] = {
    {
        {
            (TetrisPoint){ 1, 0},
            (TetrisPoint){-1, 0},
            (TetrisPoint){+2, 0},
            (TetrisPoint){-1,-1},
            (TetrisPoint){+2,+2},
        },
        {
            (TetrisPoint){ 0,-1},
            (TetrisPoint){-1,-1},
            (TetrisPoint){+2,-1},
            (TetrisPoint){-1,+1},
            (TetrisPoint){+2,-2},
        },
        {
            (TetrisPoint){-1, 0},
            (TetrisPoint){+1, 0},
            (TetrisPoint){-2, 0},
            (TetrisPoint){+1,+1},
            (TetrisPoint){-2,-2},
        },
        {
            (TetrisPoint){ 0, 1},
            (TetrisPoint){+1, 1},
            (TetrisPoint){-2, 1},
            (TetrisPoint){+1,-1},
            (TetrisPoint){-2,+2},
        },
    },
    {
        {
            (TetrisPoint){ 0,-1},
            (TetrisPoint){-1,-1},
            (TetrisPoint){+2,-1},
            (TetrisPoint){-1,+1},
            (TetrisPoint){+2,-2},
        },
        {
            (TetrisPoint){-1, 0},
            (TetrisPoint){+1, 0},
            (TetrisPoint){-2, 0},
            (TetrisPoint){+1,+1},
            (TetrisPoint){-2,-2},
        },
        {
            (TetrisPoint){ 0, 1},
            (TetrisPoint){+1, 1},
            (TetrisPoint){-2, 1},
            (TetrisPoint){+1,-1},
            (TetrisPoint){-2,+2},
        },
        {
            (TetrisPoint){ 1, 0},
            (TetrisPoint){-1, 0},
            (TetrisPoint){+2, 0},
            (TetrisPoint){-1,-1},
            (TetrisPoint){+2,+2},
        },
        
    }
};

static const TetrisPoint T_SPIN_CORNERS[CORNERS] = {
    {-1,1},
    {1,1},
    {1,-1},
    {-1,-1},
};

static const uint32_t DIFFICULT_CLEARS[DIFFICULT_CLEAR_COUNT] = { 
    TETRIS,
    MINI_TSPIN_SINGLE,
    TSPIN_SINGLE,
    MINI_TSPIN_DOUBLE,
    TSPIN_DOUBLE,
    TSPIN_TRIPLE,
};

static const uint32_t SCORING_TABLE[TETRIS_SCORING_TYPES] = {
    0,      // NONE
    100,    // SINGLE
    300,    // DOUBLE
    500,    // TRIPLE
    800,    // TETRIS
    100,    // MINI_TSPIN_NONE
    200,    // MINI_TSPIN_SINGLE
    400,    // MINI_TSPIN_DOUBLE
    400,    // TSPIN_NONE
    800,    // TSPIN_SINGLE
    1200,   // TSPIN_DOUBLE
    1600,   // TSPIN_TRIPLE
    1,      // B2B
    50,     // COMBO
    1,      // SOFTDROP
    2,      // HARDDROP
};

static const uint32_t PERFECT_CLEAR_SCORING[(PERFECT_CLEAR_TYPES + 1)] = {
    0,      // just for padding
    800,    // Single PC
    1200,   // Double PC
    1800,   // Triple PC
    2000,   // Tetris PC
    3200,   // BackToBack Tetris PC
};

static const uint32_t GARBAGE_TABLE[GARBAGE_TYPES] = {
    0,      // NONE
    0,      // SINGLE
    1,      // DOUBLE
    2,      // TRIPLE
    4,      // TETRIS
    0,      // MINI_TSPIN_NONE (Padding)
    0,      // MINI_TSPIN_SINGLE
    1,      // MINI_TSPIN_DOUBLE
    0,      // TSPIN_NONE (Padding)
    2,      // TSPIN_SINGLE
    4,      // TSPIN_DOUBLE
    6,      // TSPIN_TRIPLE
};

static const uint32_t COMBO_GARBAGE_TABLE[COMBO_RANGE] = { // Using PPT2 Combo Table
    0,      
    0,
    1,
    1,
    2,
    2,
    3,
    3,
    4,
    4,
    4,
    4,
    4,
    5,
};

static const TetrisPiece GARBAGE_ROW[TETRIS_COLUMNS] = { 
    GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE, GARBAGE
};

static uint32_t GetRandomNumber(TetrisRng *rng, uint32_t max);
static void ShuffleBag(TetrisRng *rng, TetrisPiece bag[TETRIS_SEVEN_BAG_SIZE]);
static bool Collision(TetrisGame *game, uint32_t state, TetrisPoint offset);
static bool CheckTSpinCorner(TetrisGame *game, uint32_t corner);
static bool TrySRS(TetrisGame *game, bool counterClockwise);
static bool UpdateDas(TetrisDasMovement *das, float frametime);
static uint32_t UpdatedState(TetrisGame *game, bool counterClockwise);
static void GoToNext(TetrisGame *game);
static void SendGarbage(TetrisGame *sender, TetrisGame *reciever);
static void ResetActive(TetrisGame *game);
static void ResetDas(TetrisDasMovement *das);
static void TryResettingLockDelay(TetrisGame *game);
static void NewBagIfNeeded(TetrisGame *game);
static void RecalculateGravity(TetrisGame *game);
static void PointAdd(TetrisPoint *point, TetrisPoint add);
static void PointSubtract(TetrisPoint *point, TetrisPoint subtract);

TetrisGame TetrisGameNew(uint32_t seed) {
    TetrisGame game = (TetrisGame){0};

    TetrisPiece bag[TETRIS_SEVEN_BAG_SIZE] = DEFAULT_BAG; 
    memcpy(game.currentBag, bag, sizeof(bag));
    memcpy(game.nextBag, bag, sizeof(bag));
    game.bagRng = (TetrisRng){seed};
    ShuffleBag(&game.bagRng, game.currentBag);
    ShuffleBag(&game.bagRng, game.nextBag);
    game.gravityInterval = GRAVITY(TETRIS_LEVEL(9));
    game.activeDas = NULL;
    game.right.move = MoveRight;
    game.left.move = MoveLeft;
    game.elapsed = -3.0f;
    game.combo = -1;
    game.garbageRng = (TetrisRng){((uint32_t)seed ^ 1664525u)};
    
    return game;
}

bool ToggleDasRight(TetrisGame *game) {
    if (!(game->gameState == RUNNING || game->gameState == WAITING)) {
        return false;
    }
    game->right.enabled = !game->right.enabled;
    if (game->right.enabled) {
        game->activeDas = &game->right;
        return MoveRight(game);
    } else {
        game->activeDas = NULL;
        if (game->left.enabled) {
            game->activeDas = &game->left;
        }
        ResetDas(&game->right);
        return false;
    }
}
bool ToggleDasLeft(TetrisGame *game) {
    if (!(game->gameState == RUNNING || game->gameState == WAITING)) {
        return false;
    }
    game->left.enabled = !game->left.enabled;
    if (game->left.enabled) {
        game->activeDas = &game->left;
        return MoveLeft(game);
    } else {
        game->activeDas = NULL;
        if (game->right.enabled) {
            game->activeDas = &game->right;
        }
        ResetDas(&game->left);
        return false;
    }
}

bool MoveRight(TetrisGame *game) {
    if (game->gameState != RUNNING) {
        return false;
    }
    bool success = !Collision(game, game->state, TETRIS_MOVE_RIGHT);
    if (success) {
        PointAdd(&game->position, TETRIS_MOVE_RIGHT);
        TryResettingLockDelay(game);
    }
    return success;
}

bool MoveLeft(TetrisGame *game) {
    if (game->gameState == STARTING) {
        return false;
    }
    bool success = !Collision(game, game->state, TETRIS_MOVE_LEFT);
    if (success) {
        PointAdd(&game->position, TETRIS_MOVE_LEFT);
        TryResettingLockDelay(game);
    }
    return success;
}

bool MoveDown(TetrisGame *game) {
    if (game->gameState == STARTING) {
        return false;
    }
    bool success = !Collision(game, game->state, TETRIS_MOVE_DOWN);
    if (success) {
        PointAdd(&game->position, TETRIS_MOVE_DOWN);
    }
    return success;
}

bool RotateClockwise(TetrisGame *game) {
    if (game->gameState != RUNNING) {
        return false;
    }
    bool success = (TrySRS(game, false));
    if (success) {
        TryResettingLockDelay(game);
    }
    return success;
}

bool RotateCounterClockwise(TetrisGame *game) {
    if (game->gameState != RUNNING) {
        return false;
    }
    bool success = TrySRS(game, true);
    if (success) {
        TryResettingLockDelay(game);
    }
    return success;
}

bool Hold(TetrisGame *game) {
    if (game->gameState != RUNNING) {
        return false;
    }
    if (game->canHold) {
        if (game->held == EMPTY) {
            game->held = game->active;
            game->active = (TetrisPiece)game->currentBag[game->next++];
            ResetActive(game);
            NewBagIfNeeded(game);
        } else {
            TetrisPiece swap = game->active;
            game->active = game->held;
            game->held = swap;
            ResetActive(game);
        }
        game->canHold = false;
        return true;
    }
    return false;
}

void ToggleSoftdrop(TetrisGame *game) {
    if (!(game->gameState == RUNNING || game->gameState == WAITING)) {
        return;
    }
    if (TETRIS_LEVEL(game->lines) < GRAVITY_INSTANT_FALL_LEVEL) {
        game->gravityAcceleration = !game->gravityAcceleration;
        RecalculateGravity(game);
    } else {
        game->gravityAcceleration = false;
    }
}

void UpdateTetrisGame(TetrisGame *game, float frametime) {
    if (!(game->gameState == WON || game->gameState == LOST)) {
        game->elapsed += (frametime / MILLISECONDS_PER_SECOND);
    }
    if (game->gameState == STARTING) {
        if (game->elapsed >= 0) {
            game->gameState = RUNNING;
            game->active = game->currentBag[game->next++];
            ResetActive(game);
        } else {
            return;
        }
    } else if (game->gameState == WAITING) {
        game->waitDelay -= frametime;
        if (game->waitDelay <= 0.0f) {
            game->gameState = RUNNING;
            bool full;
            for (int r = (TETRIS_ROWS - 1); r >= 0; r--) {
                full = true;
                for (int c = 0; c < TETRIS_COLUMNS; c++) {
                    if (game->board[r][c] == EMPTY) {
                        full = false;
                        break;
                    }
                }
                if (full) {
                    memmove(&game->board[r], &game->board[(r + 1)], sizeof(TetrisPiece) * TETRIS_COLUMNS * (TETRIS_ROWS - (r + 1)));
                    memset(&game->board[(TETRIS_ROWS - 1)], 0, sizeof(TetrisPiece) * TETRIS_COLUMNS);
                    r++;
                }
            }
            GoToNext(game);
        }
    }
    if (game->gameState != RUNNING) {
        return;
    }
    // Gravity
    uint32_t level = TETRIS_LEVEL(game->lines);
    int32_t offset = ToGroundOffset(game);
    if (offset >= 0) {
        game->lockDelay -= frametime;
        if (game->lockDelay <= 0) {
            Harddrop(game);
        }
    } else if (level > GRAVITY_INSTANT_FALL_LEVEL) {
        TetrisPoint position = (TetrisPoint){0, offset};
        game->rotatedLast = (game->rotatedLast && (position.y == 0));
        PointAdd(&game->position, position);
        game->lockDelay -= frametime;
        if (game->lockDelay <= 0) {
            Harddrop(game);
        }
    } else {
        game->gravityTimer = MAX(game->gravityTimer, 0); // Fix because for some reason gravity timer gets set to negative?
        game->gravityTimer += frametime;
        while (game->gravityTimer >= game->gravityInterval) {
            MoveDown(game);
            if (game->gravityAcceleration) {
                game->traveledSoftdrop++;
            }
            game->gravityTimer -= game->gravityInterval;
        }
    }
    // TETRIS_DAS Movement
    if (game->activeDas != NULL) {
        if (UpdateDas(game->activeDas, frametime)) {
            game->activeDas->move(game);
        }
    }
}

void SyncGames(TetrisGame *left, TetrisGame *right) {
    if (left->gameState == LOST) {
        right->gameState = WON;
        return;
    } else if (right->gameState == LOST) {
        left->gameState = WON;
        return;
    }
    if (left->outgoing == right->outgoing) {
        left->outgoing = 0;
        right->outgoing = 0;
    } else if (left->outgoing > right->outgoing) {
        SendGarbage(left, right);
    } else {
        SendGarbage(right, left);
    }
}

TetrisClear Harddrop(TetrisGame *game) {
    if (game->gameState != RUNNING) {
        return NONE;
    }
    TetrisPoint position = (TetrisPoint){0, ToGroundOffset(game)};
    game->rotatedLast = (game->rotatedLast && (position.y == 0));
    PointAdd(&game->position, position);
    game->traveledHarddrop = -position.y;
    // Add minos to board
    for (int i = 0; i < TETRIS_PIECE_MINOS; i++) {
        position = game->position;
        PointAdd(&position, MINO_TABLE[((int)game->active - 1)][(int)game->state][i]);
        game->board[position.y][position.x] = game->active;
    }
    // Check T-Spin Rules
    TetrisClear clear = NONE;
    if (game->active == T && game->rotatedLast) {
        if (game->kickException) {
            clear = TSPIN_NONE;
        } else {
            uint32_t threeRuleCorners = 0;
            for (int i = 0; i < TETRIS_ROTATION_STATES; i++) {
                if (CheckTSpinCorner(game, i)) {
                    threeRuleCorners++;
                }
            }
            if (threeRuleCorners >= 3) {
                clear = MINI_TSPIN_NONE;
                if (CheckTSpinCorner(game, game->state) && CheckTSpinCorner(game, UpdatedState(game, false))) {
                    clear = TSPIN_NONE;
                }
            }
        }
    }
    // TetrisClear Full Lines
    uint32_t cleared = 0;
    bool full;
    for (int r = (TETRIS_ROWS - 1); r >= 0; r--) {
        full = true;
        for (int c = 0; c < TETRIS_COLUMNS; c++) {
            if (game->board[r][c] == EMPTY) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int c = 0; c < TETRIS_COLUMNS; c++) {
                game->board[r][c] = AWAITING_CLEAR;
            }
            cleared++;
        }
    }
    game->lines += cleared;
    // Correct TetrisClear
    if (cleared > 0) {
        if (clear != NONE) {
            clear = (TetrisClear)((int)clear + cleared);
        } else {
            clear = (TetrisClear)cleared;
        }
        game->combo++;
    } else {
        game->combo = -1;
    }
    // Check PC
    bool perfectClear = true;
    for (int c = 0; c < TETRIS_COLUMNS; c++) {
        if (game->board[0 + cleared][c] != EMPTY)  {
            perfectClear = false;
            break;
        }
    }
    // Check difficult TetrisClear
    bool difficultClear = false;
    for (int i = 0; i < DIFFICULT_CLEAR_COUNT; i++) {
        if (clear == DIFFICULT_CLEARS[i]) {
            difficultClear = true;
            break;
        }
    }
    // Send Garbage
    if (perfectClear) {
        game->outgoing = PC_GARBAGE;
    } else {
        game->outgoing = GARBAGE_TABLE[clear];
        if (game->combo >= 0) {
            game->outgoing += COMBO_GARBAGE_TABLE[MIN(game->combo, (COMBO_RANGE - 1))];
        }
        if (game->backToBack && difficultClear) {
            game->outgoing += B2B_GARBAGE_BONUS;
        }
    }
    game->attack += game->outgoing;
    // Deal with Garbage
    if (game->incoming[0] > 0) {
    // Block Garbage
        if (game->combo >= 0) {
            while (game->outgoing > 0 && game->incoming[0] > 0) {
                if (game->outgoing > game->incoming[0]) {
                    game->outgoing -= game->incoming[0];
                    game->incoming[0] = 0;
                    memmove(&game->incoming[0], &game->incoming[1], sizeof(uint32_t) * (TETRIS_MAX_INCOMING_CHUNKS - 1));
                } else {
                    game->incoming[0] -= game->outgoing;
                    game->outgoing = 0;
                    if (game->incoming[0] == 0) {
                        memmove(&game->incoming[0], &game->incoming[1], sizeof(uint32_t) * (TETRIS_MAX_INCOMING_CHUNKS - 1));
                    }
                }
            }
        // Recieve Garbage
        } else {
            bool randomize = (GARBAGE_RANDOMNESS > GetRandomNumber(&game->garbageRng, 100));
            uint32_t baseColumn = GetRandomNumber(&game->garbageRng, TETRIS_COLUMNS);
            memmove(&game->board[game->incoming[0]], &game->board[0], sizeof(TetrisPiece) * TETRIS_COLUMNS * (TETRIS_ROWS - (game->incoming[0] + 1)));
            for (int r = 0; r < game->incoming[0]; r++) {
                memcpy(&game->board[r], &GARBAGE_ROW, sizeof(TetrisPiece) * TETRIS_COLUMNS);
                if (randomize) {
                    game->board[r][GetRandomNumber(&game->garbageRng, TETRIS_COLUMNS)] = EMPTY;
                } else {
                    game->board[r][baseColumn] = EMPTY;
                }
            }
            memmove(&game->incoming[0], &game->incoming[1], sizeof(uint32_t) * (TETRIS_MAX_INCOMING_CHUNKS - 1));
        }
    }
    // Scoring
    uint32_t level = TETRIS_LEVEL(game->lines);
    uint32_t score = (SCORING_TABLE[(int)clear] * level);
    if (game->backToBack && difficultClear) {
        score = (uint32_t)(((double)score) * B2B_SCORE_BONUS);
        
    }
    if (game->combo > 0) {
        score += (game->combo * SCORING_TABLE[(int)COMBO] * level);
    }
    score += (game->traveledSoftdrop * SCORING_TABLE[(int)SOFTDROP]);
    score += (game->traveledHarddrop * SCORING_TABLE[(int)HARDDROP]);
    int32_t perfectClearIndex = cleared;
    if (perfectClear && game->backToBack && clear == TETRIS) {
        perfectClearIndex++;
    }
    if (perfectClear) {
        score += (PERFECT_CLEAR_SCORING[perfectClearIndex] * level);
    }
    game->score += score;
    if (clear != NONE) {
        game->lastClear = clear;
        game->perfectClear = perfectClear;
        game->lastClearTime = game->elapsed;
        game->hadBackToBack = ((game->backToBack && difficultClear) || clear == NONE);
    }
    // UpdateTetrisGame BackToBack
    game->backToBack = ((game->backToBack && (clear == NONE || clear == MINI_TSPIN_NONE || clear == TSPIN_NONE)) || difficultClear);
    game->placed++;
    // Set Wait Delay
    if (cleared > 0) {
        game->waitDelay = TETRIS_LINE_CLEAR_DELAY;
        game->gameState = WAITING;
    } else {
        GoToNext(game);
    }
    return clear;
}

int32_t ToGroundOffset(TetrisGame *game) {
    TetrisPoint offset = {0};
    while (!Collision(game, game->state, offset)) {
        PointAdd(&offset, TETRIS_MOVE_DOWN);
    } 
    PointSubtract(&offset, TETRIS_MOVE_DOWN);
    return offset.y;    
}

float GetPPS(TetrisGame *game) {
    if (game->placed == 0) {
        return 0;
    }
    return ((float)game->placed / game->elapsed);
}

float GetAPM(TetrisGame *game) {
    if (game->attack == 0) {
        return 0;
    }
    return ((float)game->attack / (game->elapsed / 60.0f));
}

static void GoToNext(TetrisGame *game) {
    game->active = game->currentBag[game->next++];
    ResetActive(game);
    NewBagIfNeeded(game);
    RecalculateGravity(game);
    if (Collision(game, game->state, (TetrisPoint){0})) {
        game->gameState = LOST;
    }
}

static uint32_t GetRandomNumber(TetrisRng *rng, uint32_t max) {
    rng->seed = (25214903917u * rng->seed + 1013904223u);
    return ((rng->seed >> 16) % max);
}

static void ShuffleBag(TetrisRng *rng, TetrisPiece bag[TETRIS_SEVEN_BAG_SIZE]) {
    TetrisPiece temp;
    for (int i = TETRIS_SEVEN_BAG_SIZE - 1; i > 0; i--) {
        int j = (GetRandomNumber(rng, (i + 1))); 
        temp = bag[i];
        bag[i] = bag[j];
        bag[j] = temp;
    }
}

static bool Collision(TetrisGame *game, uint32_t state, TetrisPoint offset) {
    TetrisPoint newPosition = game->position;
    PointAdd(&newPosition, offset);
    const TetrisPoint *minos = MINO_TABLE[((int)game->active - 1)][state];
    for (int i = 0; i < TETRIS_PIECE_MINOS; i++) {
        if (((newPosition.y + minos[i].y) < 0) || 
            ((newPosition.y + minos[i].y) >= TETRIS_ROWS) || 
            ((newPosition.x + minos[i].x) < 0) ||
            (TETRIS_COLUMNS <= newPosition.x + minos[i].x) ||
            (game->board[(newPosition.y + minos[i].y)][(newPosition.x + minos[i].x)] != EMPTY)) {
            return true;
        }
    }
    return false;
}

static bool CheckTSpinCorner(TetrisGame *game, uint32_t corner) {
    return (((game->position.y + T_SPIN_CORNERS[corner].y) < 0) || 
            ((game->position.y + T_SPIN_CORNERS[corner].y) >= TETRIS_ROWS) || 
            ((game->position.x + T_SPIN_CORNERS[corner].x) < 0) ||
            (TETRIS_COLUMNS <= game->position.x + T_SPIN_CORNERS[corner].x) ||
            (game->board[(game->position.y + T_SPIN_CORNERS[corner].y)][(game->position.x + T_SPIN_CORNERS[corner].x)] != EMPTY));
}

static bool TrySRS(TetrisGame *game, bool counterClockwise) {
    if (game->active == O) {
        game->rotatedLast = true;
        return true;
    }
    uint32_t newState = UpdatedState(game, counterClockwise);
    const TetrisPoint *offsets;
    if (game->active == I) {
        offsets = SRS_TABLE_I[(int)counterClockwise][game->state];
    } else {
        offsets = SRS_TABLE_JLSTZ[(int)counterClockwise][game->state];
    }
    for (int i = 0; i < TETRIS_SRS_OFFSETS; i++) {
        if (!Collision(game, newState, offsets[i])) {
            PointAdd(&game->position, offsets[i]);
            game->state = newState;
            game->rotatedLast = true;
            if (i >= TETRIS_EXCEPTION_STATE_THRESHOLD) {
                game->kickException = true;
            }
            return true;
        }
    }
    return false;
}

static uint32_t UpdatedState(TetrisGame *game, bool counterClockwise) {
    if (counterClockwise) {
        return ((game->state + (TETRIS_ROTATION_STATES - 1)) % TETRIS_ROTATION_STATES);
    } else {
        return ((game->state + 1) % TETRIS_ROTATION_STATES);
    }
}

static void SendGarbage(TetrisGame *sender, TetrisGame *reciever) {
    if (sender->outgoing <= reciever->outgoing) {
        return;
    }
    sender->outgoing -= reciever->outgoing;
    reciever->outgoing = 0;
    for (int i = 0; i < TETRIS_MAX_INCOMING_CHUNKS; i++) {
        if (reciever->incoming[i] == 0) {
            reciever->incoming[i] = sender->outgoing;
            sender->outgoing = 0;
            return;
        }
    }
    // Edge case that we reach end of array just pile it up there I guess
    reciever->incoming[(TETRIS_MAX_INCOMING_CHUNKS - 1)] += sender->outgoing;
    sender->outgoing = 0;
}

static void ResetActive(TetrisGame *game) {
    game->position = TETRIS_START_POSITION;
    game->state = 0;
    game->rotatedLast = false;
    game->kickException = false;
    game->lockDelay = TETRIS_LOCK_DELAY;
    game->remainingLockResetActions = TETRIS_LOCK_DELAY_RESET_ACTIONS;
    game->traveledSoftdrop = 0;
    game->traveledHarddrop = 0;
    game->canHold = true;
    game->gravityTimer = 0;
}

static void ResetDas(TetrisDasMovement *das) {
    das->arrTimer = 0;
    das->dasTimer = 0;
    das->enabled = false;
}

static void TryResettingLockDelay(TetrisGame *game) {
    if (game->remainingLockResetActions > 0 && game->lockDelay < (TETRIS_LOCK_DELAY - 1.0f)) {
        game->lockDelay = TETRIS_LOCK_DELAY;
        game->remainingLockResetActions--;
    }
}

static bool UpdateDas(TetrisDasMovement *das, float frametime) {
    if (das->dasTimer < TETRIS_DAS) {
        das->dasTimer += frametime;
        return (das->dasTimer >= TETRIS_DAS);
    } else {
        das->arrTimer += frametime;
        if (das->arrTimer >= TETRIS_ARR) {
            das->arrTimer -= TETRIS_ARR;
            return true;
        }
    }
    return false;
}

static void RecalculateGravity(TetrisGame *game) {
    game->gravityInterval = GRAVITY(TETRIS_LEVEL(game->lines));
    if (game->gravityAcceleration) {
            game->gravityInterval /= TETRIS_SOFTDROP_ACCELERATION;
    }
    game->gravityTimer = 0;
}

static void NewBagIfNeeded(TetrisGame *game) {
    if (game->next >= TETRIS_SEVEN_BAG_SIZE) {
        memcpy(game->currentBag, game->nextBag, sizeof(game->currentBag));
        TetrisPiece bag[TETRIS_SEVEN_BAG_SIZE] = DEFAULT_BAG; 
        memcpy(game->nextBag, bag, sizeof(bag));
        ShuffleBag(&game->bagRng, game->nextBag);
        game->next = 0;
    }
}

static void PointAdd(TetrisPoint *point, TetrisPoint add) {
    point->x += add.x;
    point->y += add.y;
}

static void PointSubtract(TetrisPoint *point, TetrisPoint subtract) {
    point->x -= subtract.x;
    point->y -= subtract.y;
}