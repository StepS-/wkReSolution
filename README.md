**ReSolution** is a new [WormKit](http://worms2d.info/WormKit) module for Worms 2 (version 1.05 and GOG) to change the in-game screen resolution to any value. Its key features are simplicity (don't need to patch the executable) and automatic detection of island and cavern-type landscapes, and is thus preferred over the hardcoded patches.

**ReSolution v0.2** and newer (also known as "ReSolutionDD") add ability to **resize** (shrink or extend) the game window so that it actually resizes the free screen space available in the game (and doesn't perform bitmap stretching). An additional windowed mode wrapper (wndmode.dll) is needed for this to work: get the windowed bundle.

**ReSolution v0.3** and newer add ability to **zoom** (in and out) the game environment using your mouse wheel or keyboard so that you can either expand the view or look more closely at the objects that you see. Alt+Enter is also introduced in v0.3, giving the ability to switch from and to borderless fullscreen windowed representation. An additional windowed mode wrapper (wndmode.dll) is needed for this to work: get the windowed bundle.

# Installation

The installation is very simple:
- Download the archive containing the module.
- Extract the archive's contents to the directory where your copy of Worms 2 is installed.
- Optional: if you want to get the advanced window resizing or zooming features, install the windowed mode wrapper as well, by downloading the respective bundle.
- Run the game: the desktop resolution will be autodetected and enabled. You can also change this using the W2.ini file to any value you want.

# Features

- Can change resolution (and thus, window size) to any value using the W2.ini file
- In conjunction with windowed mode (only), allows to **resize** the game window so that it actually resizes the real screen estate of the game.
 + Enable the "Enable" setting from [Resizing] in the ini file to activate this.
 + Enabling the "ProgressiveUpdate" (experimental) setting from [Resizing] will immediately redraw the window as you resize it, making the scenery resize very lively and smoothly.
 + As usual with windowed mode, make sure that the ScreenWidth and ScreenHeight are set to values lower than the screen resolution of your primary monitor. However, v0.3 and later add a new key combination: Alt+Enter, which allows you to switch between these two modes directly in the game. Enable the "AltEnter" setting from [Resizing] in the ini file to activate this.
 + In windowed mode, pressing Ctrl+G will show the cursor and allow you to move it above the game's window, to resize the window.
- In conjunction with windowed mode (only), allows to **zoom** the game environment in and out using either mouse wheel or keyboard.
 + Enable the "Enable" setting from [Zooming] in the ini file to activate this.
 + Enabling "UseMouseWheel" will let you zoom the game using your mouse wheel: roll it back and forth. Pressing the middle mouse button will reset your zooming level to the native default.
 + Enabling "UseKeyboardZoom" will let you zoom the game using your Numpad + and - keys. Zooming in this mode will be performed on a frame-by-frame basis, rather than delta like with mouse wheel. Pressing the End button will reset your zooming level to the native default.
- Works flawlessly on open-island maps
- Respects caverns, automatically applying the screen box for resolutions bigger than 1920x856
- Supports island resolutions bigger than 6012x2902
- Does everything in memory without touching your executable
- Compatible only with version 1.05 (multi-version support may be added later); GOG is also supported since it's almost the same file.

# TODO

- Automatically adjust the position of the instant-replay TV brackets and allow a way to disable them.
- Detect when the water starts flooding in caverns and apply the needed screen size mode. Right now, an offline fix is applied, requiring everyone online to have the module installed in order to maintain sync.
