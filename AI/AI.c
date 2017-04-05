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
void dash(bool sameDir)
{
    float dashAngle = (FACING_DIRECTION(AI_PORT) == -1.f) ? 180.f : 0.f;
    if (!sameDir) { dashAngle += 180.f; }
    SET_DASH_ANGLE(dashAngle);
    addMove(&cpuPlayer, &_mv_dash);
}

// ---- Starting the dash dance ---- 

bool inDashDance = 0;
unsigned int dd_frames = 0;
unsigned int lastDirectionFrame = 0;
#define MAX_DD_FRAMES 120
bool exitDashDance = 0;

bool isInDashDance(){ return inDashDance; }
bool waiting()
{ 
    FunctionArg arg;
    arg.u = AI_PORT;
    return actionStateEq(arg, _AS_Wait) && \
                       !isInDashDance(); }

void startDashDance()
{
    dd_frames = 0;
    inDashDance = 1;
    lastDirectionFrame = 0;
    dash(1);
}

Logic startDashLogic =
{
    {&waiting},
    {&startDashDance}
};

// ----- Mid dash-dance -----

void dashDance()
{
    // exit dash dance we hit max number of frames (TODO: or if the opponent tries to attack)
    if (dd_frames > MAX_DD_FRAMES) { 
        exitDashDance = 1;  
        inDashDance = 0;
        return;
    }

    // reasons to switch direction:
    // currently: always
    // 1. TODO about to turn to run
    // 2. TODO about to run off stage
    // 3. TODO running into danger zone
    dd_frames += 1;

    bool switchDir = 1;
    if (switchDir) {
        lastDirectionFrame = dd_frames;
        dash(0);
    }
}

Logic dashDanceLogic = 
{
    {&isInDashDance, .arg1.u = AI_PORT},
    {&dashDance}
};

// ---- Exiting dash dance ---- 

bool shouldExitDashDance() { return exitDashDance; }
RawInput _raw_releaseSH[3] = 
{
    {RELEASE, 0, NO_FLAGS},
    {X_BUTTON, 0, JUMPSQUAT},
    {RELEASE, 0, NO_FLAGS}
};
Move _mv_releaseSH = {.inputs = _raw_releaseSH, .size = 3};
void endDashDance()
{
    exitDashDance = 0;
    addMove(&cpuPlayer, &_mv_releaseSH);
}

Logic exitDashDanceLogic = 
{
    {&shouldExitDashDance},
    {&endDashDance}
};

// ----------------------------------------------------

static void init()
{
    initHeap(heap, heap + sizeof(heap));
    init_run = true;
}

// TODO: Which logic takes priority if both conditions are met?
static void loadDefaultLogic()
{
    addLogic(&cpuPlayer, &startDashLogic);
    addLogic(&cpuPlayer, &dashDanceLogic);
    addLogic(&cpuPlayer, &exitDashDanceLogic);
}

void _main()
{
    if (!init_run) { init(); }

    if (needLogic(&cpuPlayer)) { loadDefaultLogic(); }

    updateAI(&cpuPlayer);
}

