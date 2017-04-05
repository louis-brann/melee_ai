#include <mml/ai.h>
#include <mml/system.h>
#include <mml/gctypes.h>
#include <mml/logic.h>
#include <mml/action_state.h>
#include <mml/state_check.h>
#include <mml/moves.h>
#include <mml/melee_info.h>
#include <mml/math.h>
#include <mml/game_state.h>
#include <mml/print.h>

#include "recovery.h"

static char heap[2000];
static bool init_run = false;

#define AI_PORT 2
static AI cpuPlayer = INIT_AI(AI_PORT, FALCO | FOX | MARTH | FALCON);

// ------------------ DASH DANCE ---------------------

// TODO: Figure out dash lag if there is any
RawInput _raw_dash[1] =
{
    {FULL_STICK | STICK_ANGLE(0.f), 0, NO_FLAGS}
};
#define SET_DASH_ANGLE(x) _raw_dash[0].controller = \
    FULL_STICK | STICK_ANGLE(x)
Move _mv_dash = {.inputs = _raw_dash, .size = 1};

#define FACING_DIRECTION(p) _gameState.playerData[p]->facingDirection
void dash(FunctionArg sameDir)
{
    float dashAngle = (FACING_DIRECTION(AI_PORT) == -1.f) ? 180.f : 0.f;
    if (!sameDir.u) { dashAngle += 180.f; }
    SET_DASH_ANGLE(dashAngle);
    addMove(&cpuPlayer, &_mv_dash);
}

Logic startDashLogic =
{
    {&actionStateEq, .arg1.u = AI_PORT, .arg2.u = _AS_Wait},
    {&dash, .arg1.u = 1}
};

bool onGroundMoving(FunctionArg port)
{
    return onGround(port) && !actionStateEq(port.u, _AS_Wait);
}
Logic dashDanceLogic = 
{
    {&onGroundMoving, .arg1.u = AI_PORT},
    {&dash, .arg1.u = 0}
};

// ----------------------------------------------------

Logic spotDodgeLogic = 
{
    {&actionStateEq, .arg1.u = AI_PORT, .arg2.u = _AS_Wait},
    {&addMove, .arg1.p = &cpuPlayer, .arg2.p = &_mv_spotDodge}
};

static void init()
{
    initHeap(heap, heap + sizeof(heap));
    init_run = true;
}

// TODO: Which logic takes priority if both conditions are met?
static void loadDefaultLogic()
{
    // addLogic(&cpuPlayer, &startDashLogic);
    // addLogic(&cpuPlayer, &dashDanceLogic);
    addLogic(&cpuPlayer, &spotDodgeLogic);
}

void _main()
{
    if (!init_run) { init(); }

    if (needLogic(&cpuPlayer)) { loadDefaultLogic(); }

    updateAI(&cpuPlayer);
}

