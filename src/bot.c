#include "bot.h"
#include <stdio.h>
#include <string.h>

CCAsyncBot *bot = NULL;

static CCPiece ToCCPiece(TetrisPiece p) {
    switch (p) {
        case I:
            return CC_I;
        case O:
            return CC_O;
        case T:
            return CC_T;
        case L:
            return CC_L;
        case J:
            return CC_J;
        case S:
            return CC_S;
        case Z:
            return CC_Z;
        default:
            return CC_I; // shouldn't happen
    }
}

void StartBot(TetrisGame *game) { 
    CCOptions options = {0};
    CCWeights weights = {0};
    cc_default_options(&options);
    cc_default_weights(&weights);
    //options.mode = CC_HARD_DROP_ONLY;
    CCPiece queue[(TETRIS_QUEUE_LENGTH + 1)];
    for (int i = 0; i < (TETRIS_QUEUE_LENGTH + 1); i++) {
        queue[i] = ToCCPiece((game->currentBag[i]));
    }
    bot = cc_launch_async(&options, &weights, NULL, queue, (TETRIS_QUEUE_LENGTH + 1));
}

void RequestBotMove(TetrisGame *game) {
    cc_request_next_move(bot, game->incoming[0]);
}

static void AddNextPiece(TetrisGame *game) {
    CCPiece lastInQueue;
    int nextPieceIndex = game->next + (TETRIS_QUEUE_LENGTH - 1);
    if (game->gameState == WAITING) {
        nextPieceIndex++;
    }
    if (nextPieceIndex < TETRIS_SEVEN_BAG_SIZE) {
        lastInQueue = ToCCPiece((game->currentBag[nextPieceIndex]));
    } else {
        lastInQueue = ToCCPiece((game->nextBag[(nextPieceIndex % 7)]));
    }
    cc_add_next_piece_async(bot, lastInQueue);
}

void MakeBotMove(TetrisGame *game) {
    CCMove move;
    cc_block_next_move(bot, &move, NULL, NULL);
    if (move.hold) {
        if (game->held == EMPTY) {
            Hold(game);
            AddNextPiece(game);
        } else {
            Hold(game);
        }
    }

    for (int i = 0; i < move.movement_count; i++) {
        switch (move.movements[i]) {
            case CC_LEFT:
                MoveLeft(game);
                break;
            case CC_RIGHT:
                MoveRight(game);
                break;
            case CC_CW:
                RotateClockwise(game);
                break;
            case CC_CCW:
                RotateCounterClockwise(game);
                break;
            case CC_DROP:
                while (MoveDown(game));
                break;
            default:
                break;
        }
    }
    Harddrop(game);

    AddNextPiece(game);
}

void UpdateBot(TetrisGame *game) {
    bool board[400] = {0};
    for (int r = 0; r < TETRIS_ROWS_VISIBLE; r++) {
        for (int c = 0; c < TETRIS_COLUMNS; c++) {
            board[c + (r * TETRIS_COLUMNS)] = game->board[r][c];
        }
    }
    cc_reset_async(bot, board, game->backToBack, game->combo);
}

void StopBot(void) {
    if (bot != NULL) {
        cc_destroy_async(bot);
        bot = NULL;
    }
}
