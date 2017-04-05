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

// ----------------------------------------------------

// ----------------------------------------------------

static void init()
{
    initHeap(heap, heap + sizeof(heap));
    init_run = true;
}

// TODO: Which logic would take priority if both conditions are met?
static void loadDefaultLogic()
{
    // addLogic(&cpuPlayer, &jumpLogic);
    // addLogic(&cpuPlayer, &waveLandLogic);
    addLogic(&cpuPlayer, &LCancelLogic);
    addLogic(&cpuPlayer, &shortHopNairLogic);
}

void _main()
{
    if (!init_run) { init(); }

    if (needLogic(&cpuPlayer)) { loadDefaultLogic(); }

    updateAI(&cpuPlayer);
}

