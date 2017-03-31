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

// ----------------- WAVE DASH --------------------------

// Note: Flags can't be user-defined without rebuilding library from scratch every time.
//          https://stackoverflow.com/questions/14919366/how-to-compile-library-on-c-using-gcc
// Question: What is best practice regarding ^ that?
// TODO: Figure out tradeoff between frameOffset and flags...? For new moves,
//       we should just use offset?
RawInput _raw_waveDash[4] = 
{
    {X_BUTTON, 0, NO_FLAGS},
    {RELEASE, -1, JUMPSQUAT},
    {OVERWRITE, 0, NO_FLAGS},
    {RELEASE, 10, NO_FLAGS}
};
Move _mv_waveDash = {.inputs = _raw_waveDash, .size = 4};
#define SET_WAVEDASH_FRAME_OFFSET(x) _raw_waveDash[2].frameOffset = x
#define SET_WAVEDASH_ANGLE(x) _raw_waveDash[2].controller = \
    L_BUTTON | FULL_STICK | STICK_ANGLE((x))


void waveDash(AI* ai)
{
    setGlobalVariables(ai);
    float ang = rInfo.stageDir > 90.f ? 200.f : 340.f;
    // SET_WAVEDASH_FRAME_OFFSET(_ledgedash_frames[rInfo.character-1]);
    SET_WAVEDASH_ANGLE(ang);
    addMove(ai, &_mv_waveDash);
}

Logic waveDashLogic = 
{
    {&actionStateEq, .arg1.u = AI_PORT, .arg2.u = _AS_Wait},
    {&waveDash, .arg1.p = &cpuPlayer}
};

// ------------------- SHIELD DROP ------------------------

// Note: currently only for action state waiting
bool onPlatform(FunctionArg port)
{
    // Platform side = _gameState.stage.side;
    // Platform top = _gameState.stage.top;

    // float x = _gameState.playerData[port.u]->coordinates.x;
    float y = _gameState.playerData[port.u]->coordinates.y;
    // float abs_x = fabs(x);

    // bool onSide = y == side.height && \
    //               abs_x >= side.left && \
    //               abs_x <= side.right;  
    // bool onTop = y == top.height && \
    //              abs_x >= top.left && \
    //              abs_x <= top.right;
    return actionStateEq(port, _AS_Wait) && y > 0.f;
}

// Shai drops work by buffering down to disable rolling and
// shielding prior to actually shielding 
// https://smashboards.com/threads/shai-drop-update-detailed-guide-frame-data.322062/
// 
RawInput _raw_shaiDrop[3] = 
{
    {FULL_STICK | STICK_ANGLE(270.f), 0, NO_FLAGS},
    {L_BUTTON, 4, NO_FLAGS},
    {RELEASE, 2, NO_FLAGS}
};
Move _mv_shaiDrop = {.inputs = _raw_shaiDrop, .size=3};
Logic shaiDropLogic =
{
    {&onPlatform, .arg1.u = AI_PORT},
    {&addMove, .arg1.p = &cpuPlayer, .arg2.p = &_mv_shaiDrop}
};

// ------------------ LANDING -------------------------

#define X_COORD(p)          (_gameState.playerData[p]->coordinates.x)
#define Y_COORD(p)          (_gameState.playerData[p]->coordinates.y)
#define STAGE               _gameState.stage

// to mess around with how near to the surface we need for wave landing
#define WAVELAND_HEIGHT 0.5f
bool nearerSurface(FunctionArg port)
{
    float x = fabs(X_COORD(port.u));
    float y = Y_COORD(port.u);

    bool xSidePlat = x > STAGE.side.left && x < STAGE.side.right;

    bool ySidePlat = y > STAGE.side.height
        && y < STAGE.side.height + WAVELAND_HEIGHT;

    bool topPlat = x < STAGE.top.right
        && y > STAGE.top.height && y < STAGE.top.height + WAVELAND_HEIGHT;

    if (STAGE.side.height < 1.f) {xSidePlat = false;}
    if (STAGE.top.height  < 1.f) {topPlat = false;}

    bool stage = !offstage(port) && y < WAVELAND_HEIGHT;

    return (xSidePlat && ySidePlat) || topPlat || stage;
}

RawInput _raw_airDodge[2] = 
{
    {L_BUTTON, 0, NO_FLAGS},
    {RELEASE, 0, NO_FLAGS}
};
Move _mv_airDodge = {.inputs = _raw_airDodge, .size = 2};
#define SET_AIRDODGE_ANGLE(x) _raw_airDodge[0].controller = \
    L_BUTTON | FULL_STICK | STICK_ANGLE(x)
void waveLand(AI* ai)
{
    setGlobalVariables(ai);
    float ang = rInfo.stageDir > 90.f ? 200.f : 340.f;
    SET_AIRDODGE_ANGLE(ang);
    addMove(ai, &_mv_airDodge);
}
bool wavelandSituation(FunctionArg port)
{
    // debug
    char info[20];
    sprintf(info, "v: %f, x: %f", \
            _gameState.playerData[AI_PORT]->deltaCoordinates.y, \
            _gameState.playerData[AI_PORT]->coordinates.y);
    print(info);

    return nearerSurface(port) && \
           inAir(port) && \
           _gameState.playerData[port.u]->deltaCoordinates.y < 0.f;
}
Logic waveLandLogic =
{
    {&wavelandSituation, AI_PORT},
    {&waveLand, .arg1.p = &cpuPlayer}
};
Logic jumpLogic =
{
    {&actionStateEq, .arg1.u = AI_PORT, .arg2.u = _AS_Wait},
    {&addMove, .arg1.p = &cpuPlayer, .arg2.p = &_mv_shortHop}
};

// ----------------------------------------------------

static void init()
{
    initHeap(heap, heap + sizeof(heap));
    init_run = true;
}

// TODO: Which logic would take priority if both conditions are met?
static void loadDefaultLogic()
{
    addLogic(&cpuPlayer, &jumpLogic);
    addLogic(&cpuPlayer, &waveLandLogic);
}

void _main()
{
    if (!init_run) { init(); }

    if (needLogic(&cpuPlayer)) { loadDefaultLogic(); }

    updateAI(&cpuPlayer);
}

