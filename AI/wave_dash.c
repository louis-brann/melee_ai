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