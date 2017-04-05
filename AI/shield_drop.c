// currently always shield drops using SE notch
// TODO: figure out where this was
// #undef TILT_RADIUS 0.3
// #define TILT_RADIUS 0.53f

RawInput _raw_shieldDrop[4] =
{
    {L_BUTTON, 0, NO_FLAGS},
    {L_BUTTON | TILT_STICK | STICK_ANGLE(340.f), 1, NO_FLAGS},
    {L_BUTTON | FULL_STICK | STICK_ANGLE(317.55f), 2, NO_FLAGS},
    {RELEASE, 10, NO_FLAGS}
};
#define SET_SHIELDDROP_ANGLE(x) _raw_shieldDrop[2].controller = \
    L_BUTTON | FULL_STICK | STICK_ANGLE((x))
Move _mv_shieldDrop = {.inputs = _raw_shieldDrop, .size=4};

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

float ang = 317.550000000f;
void shieldDrop(AI* ai)
{
    // if (ang < 360.f) {
    //     ang += 0.05f;
        
    char buffer[10] = "";
    sprintf(buffer, "Hello %f", ang);
    print(buffer);
    // } else {
    //     ang = 317.00f;
    // }
    // SET_WAVEDASH_FRAME_OFFSET(_ledgedash_frames[rInfo.character-1]);
    SET_SHIELDDROP_ANGLE(ang);
    addMove(ai, &_mv_shieldDrop);
}

Logic shieldDropLogic =
{
    {&onPlatform, .arg1.u = AI_PORT},
    {&shieldDrop, .arg1.p = &cpuPlayer}
};


RawInput _raw_shaiDrop[3] = 
{
    {FULL_STICK | STICK_ANGLE(370.f), 0, NO_FLAGS},
    {L_BUTTON | FULL_STICK | STICK_ANGLE(370.f), 3, NO_FLAGS},
    {RELEASE, 0, NO_FLAGS}
};
Move _mv_shaiDrop = {.inputs = _raw_shaiDrop, .size=3};
Logic shaiDropLogic =
{
    {&onPlatform, .arg1.u = AI_PORT},
    {&addMove, .arg1.p = &cpuPlayer, .arg2.p = &_mv_shaiDrop}
};

