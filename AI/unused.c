// from https://www.gamefaqs.com/boards/516492-super-smash-bros-melee/49685942
const u8 _wavedash_frames[25] = 
{
    5, //DR_MARIO
    5, //MARIO
    5, //LUIGI
    9, //BOWSER
    6, //PEACH
    6, //YOSHI
    6, //DK
    5, //FALCON
    7, //GANON
    6, //FALCO
    4, //FOX
    5, //NESS
    4, //ICE_CLIMBERS
    4, //KIRBY
    4, //SAMUS
    7, //ZELDA 4, //SHEIK
    7, //LINK
    5, //YOUNG_LINK
    4, //PICHU
    4, //PIKACHU
    6, //PUFF
    6, //MEWTWO
    5, //MR_GNW
    6, //MARTH
    5 //ROY
};

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

// TODO: Figure out if this is right
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
    {&actionStateEq, .arg1.u = 2, .arg2.u = _AS_Wait},
    {&waveDash, .arg1.p = &cpuPlayer}
};


