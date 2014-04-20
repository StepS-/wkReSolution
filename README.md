**ReSolution** is a new [WormKit](http://worms2d.info/WormKit) module for Worms 2 (version 1.05 and GOG) to change the in-game screen resolution to any value. Its key features are simplicity (don't need to patch the executable) and automatic detection of island and cavern-type landscapes, and is thus preferred over the hardcoded patches.
**ReSolutionDD** is a work-in-progress version of the module, which adds ability to resize (shrink or extend) the game window so that it actually resizes the free screen space available in the game (and doesn't perform bitmap stretching).

# Installation

The installation is very simple:
- Download the archive containing the module.
- Extract the archive's contents to the directory where your copy of Worms 2 is installed.
- Optional: if you want to get the advanced window resizing feature of ReSolutionDD, install the windowed mode wrapper (wndmode.dll) as well.
- Run the game: the desktop resolution will be autodetected and enabled. You can also change this using the W2.ini file to any value you want.

# Features

- Can change resolution to any value using the W2.ini file
- In conjunction with windowed mode, allows to shrink or extend the game window so that it actually resizes the free screen space available in the game.
- Works flawlessly on open-island maps
- Respects caverns, automatically applying the screen box for resolutions bigger than 1920x856
- Supports island resolutions bigger than 6012x2902
- Does everything in memory without touching your executable
- Compatible only with version 1.05 (multi-version support may be added later); GOG is also supported since it's almost the same file.

# TODO

- Detect when the water starts flooding in caverns and apply the needed screen size mode. Right now, an offline fix is applied, requiring everyone online to have the module installed in order to maintain sync.
