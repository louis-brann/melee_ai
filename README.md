# melee_ai

Making a melee AI using my fork of sherman5's [MeleeModdingLibrary](https://github.com/louis-brann/MeleeModdingLibrary).

It has the same prereqs as the MeleeModdingLibrary, and it's probably
best to just follow the guide sherman5 provides.

## Building

1. Add a 'melee.iso' file in the root directory (or change the Makefile to point
   to the location of your .iso file)
2. Run "make AI" 

This will build the MeleeModdingLibrary, and build and inject the code 
specified in the AI/AI.ini file into the melee.iso. Then you can boot up the
.iso in Dolphin and have fun!

## TODO

I should probably be using Github issues for these, but I'm kinda lazy atm

#### Features

###### Core
- [x] Wave Dash
- [x] Wave Land (-ish)
- [x] L Cancel
- [ ] Dash Dance
- [ ] Wave Shine

###### Superfluous
- [ ] Shield Drop???? (this is hard T~T)
- [ ] Moonwalk/Stickywalk/Boost Run :P

#### Code Fixes

- [x] Recompile library every build (--> better code organization, can add flags, etc.)
- [ ] Cleanup air dodge code

#### Bots

- [ ] Wait n Bait
- [ ] SwagTech
