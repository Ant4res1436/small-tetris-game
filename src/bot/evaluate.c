#include "evaluate.h"

static EvalScoring scoring = {0};

void SetDefaultScoring() {
    scoring.terrainChange = -2.0f;
    scoring.holes = -1.0f;
    scoring.holeCover = -0.5f;
    scoring.height = -1.0f;
    scoring.upperHalf = -33.3f;
    scoring.upperQuarter = -100.0f;

    scoring.single = -100.0f;
    scoring.double_ = -40.0f;
    scoring.triple = -15.0f;
    scoring.tetris = 100.0f;
    scoring.tss = 80.0f;
    scoring.tsd = 130.0f;
    scoring.tst = 200.0f;
    scoring.combo = 10.0f;
    scoring.b2b = 20.0f;
}

void Evaluate(BotState *state) {

}