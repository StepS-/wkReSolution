**ReSolution** is a new [WormKit](http://worms2d.info/WormKit) module for Worms 2 (version 1.05 and GOG) to change the in-game screen resolution to any value. Its key features are simplicity (don't need to patch the executable) and automatic detection of island and cavern-type landscapes, and is thus preferred over the hardcoded patches.
**ReSolution v0.2** and newer (also known as "ReSolutionDD") add ability to resize (shrink or extend) the game window so that it actually resizes the free screen space available in the game (and doesn't perform bitmap stretching). An additional windowed mode wrapper (wndmode.dll) is needed for this to work: get the windowed bundle.

# Installation

The installation is very simple:
- Download the archive containing the module.
- Extract the archive's contents to the directory where your copy of Worms 2 is installed.
- Optional: if you want to get the advanced window resizing feature, install the windowed mode wrapper as well, by downloading the respective bundle.
- Run the game: the desktop resolution will be autodetected and enabled. You can also change this using the W2.ini file to any value you want.

# Features

- Can change resolution (and thus, window size) to any value using the W2.ini file
- In conjunction with windowed mode, allows to shrink or extend the game window so that it actually resizes the free screen space available in the game.
 + Enable "AllowResize" setting in the ini file to activate this.
 + Enabling "ProgressiveResize" (experimental) will immediately redraw the window as you resize it, making the scenery resize very lively and smoothly.
- Works flawlessly on open-island maps
- Respects caverns, automatically applying the screen box for resolutions bigger than 1920x856
- Supports island resolutions bigger than 6012x2902
- Does everything in memory without touching your executable
- Compatible only with version 1.05 (multi-version support may be added later); GOG is also supported since it's almost the same file.

# TODO

- Detect when the water starts flooding in caverns and apply the needed screen size mode. Right now, an offline fix is applied, requiring everyone online to have the module installed in order to maintain sync.
