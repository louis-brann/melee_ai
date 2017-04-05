// TODO: Difference between Fall and Jump action states

#define DELTA_Y(p)          (_gameState.playerData[p]->deltaCoordinates.y)
#define X_COORD(p)          (_gameState.playerData[p]->coordinates.x)
#define Y_COORD(p)          (_gameState.playerData[p]->coordinates.y)
#define STAGE               _gameState.stage
#define ACTION_STATE(p)     (_gameState.playerData[p]->actionState)

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
    return nearerSurface(port) && \
           inAir(port) && \
           DELTA_Y(port.u) < 0.f && \
           (ACTION_STATE(port.u) >= _AS_JumpF \
            && ACTION_STATE(port.u) <= _AS_FallAerialB);
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

// L CANCEL

RawInput _raw_Lbutton[2] =
{
    {L_BUTTON, 0, NO_FLAGS},
    {RELEASE, 0, NO_FLAGS}
};
Move _mv_Lbutton = {.inputs = _raw_Lbutton, .size = 2};

bool LCancelSituation(FunctionArg port)
{
    return DELTA_Y(port.u) < 0.f \
        && nearerSurface(port) \
        && (ACTION_STATE(port.u) >= _AS_AttackAirN \
            && ACTION_STATE(port.u) <= _AS_AttackAirLw);
}
// TODO: Check that the aerial won't expire before landing?
// Always L Cancel when appropriate (up to 7 frames before landing)
Logic LCancelLogic =
{
    {&LCancelSituation, .arg1.u = AI_PORT},
    {&addMove, .arg1.p = &cpuPlayer, .arg2.p = &_mv_Lbutton}
};

// Short Hop Nair for Testing L Cancel

// 9 frames for Fox to reach peak of short hop (useful for fast falling?)
RawInput _raw_shortHop_nAir[4] =
{
    {X_BUTTON, 0, NO_FLAGS},
    {RELEASE, -1, JUMPSQUAT},
    {A_BUTTON, 0, NO_FLAGS},
    {RELEASE, 0, NO_FLAGS}
};
Move _mv_shortHop_nAir = {.inputs = _raw_shortHop_nAir, .size = 4};
Logic shortHopNairLogic =
{
    {&actionStateEq, .arg1.u = AI_PORT, _AS_Wait},
    {&addMove, .arg1.p = &cpuPlayer, .arg2.p = &_mv_shortHop_nAir}
};

