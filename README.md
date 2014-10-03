**ReSolution** is a new [WormKit](http://worms2d.info/WormKit) module for Worms World Party and Worms 2 to change the in-game screen resolution to any value. Its key features are simplicity (don't need to patch the executable) and automatic detection of island and cavern-type landscapes, and is thus preferred over the hardcoded patches.

**ReSolution v0.2** and newer (also known as "ReSolutionDD") add ability to **resize** (shrink or extend) the game window so that it actually resizes the free screen space available in the game (and doesn't perform bitmap stretching). An additional windowed mode wrapper (wndmode.dll) is needed for this to work: get the windowed bundle.

**ReSolution v0.3** and newer add ability to **zoom** (in and out) the game environment using your mouse wheel or keyboard so that you can either expand the view or look more closely at the objects that you see. **Alt+Enter** is also introduced in v0.3, giving the ability to switch from and to borderless fullscreen windowed representation. An additional windowed mode wrapper (wndmode.dll) is needed for this to work: get the windowed bundle.

**ReSolution v0.4** and newer (also known as "ReSolution HD") add **WWP support** (1.01 and 1.00) and finally introduce the automatic screen confining technique on waterrise in enclosed environments (removing the old workaround), also fixing the proper letterboxed dimensions in caverns. A new option called **UseTouchscreenZoom** has also been added, which allows to utilize the touch pinch zoom gesture in order to zoom the game in and out, when Zooming is enabled. The settings file is now **ReSolution.ini**, fitting both games. Various other internal improvements as well.

# Installation

The installation is very simple:
- Download the archive containing the module.
- Extract the archive's contents to the directory where your copy of Worms World Party or Worms 2 is installed.
- Optional: if you want to get the advanced window resizing or zooming features, install the windowed mode wrapper as well, by downloading the respective bundle.
- Run the game: the desktop resolution will be autodetected and enabled. You can also change this using the ReSolution.ini file to any value you want.

# Features

- Can change resolution (and thus, window size) to any value using the ReSolution.ini file
- In conjunction with windowed mode (only), allows to **resize** the game window so that it actually resizes the real screen estate of the game.
 + Enable the "Enable" setting from [Resizing] in the ini file to activate this.
 + **Worms 2 only**: Enabling the "ProgressiveUpdate" (experimental) setting from [Resizing] will immediately redraw the window as you resize it, making the scenery resize very lively and smoothly.
 + As usual with windowed mode, make sure that the ScreenWidth and ScreenHeight are set to values lower than the screen resolution of your primary monitor. However, v0.3 and later add a new key combination: Alt+Enter, which allows you to switch between these two modes directly in the game. Enable the "AltEnter" setting from [Resizing] in the ini file to activate this.
 + In windowed mode, pressing Ctrl+G will show the cursor and allow you to move it above the game's window, to resize the window.
- In conjunction with windowed mode (only), allows to **zoom** the game environment in and out using either mouse wheel or keyboard.
 + Enable the "Enable" setting from [Zooming] in the ini file to activate this.
 + Enabling "UseMouseWheel" will let you zoom the game using your mouse wheel: roll it back and forth. Pressing the middle mouse button will reset your zooming level to the native default.
 + Enabling "UseKeyboardZoom" will let you zoom the game using your Numpad + and - keys. Zooming in this mode will be performed on a frame-by-frame basis, rather than delta like with mouse wheel. Pressing the End button will reset your zooming level to the native default.
 + Enabling "UseTouchscreenZoom" will let you zoom the game using your touchscreen. The pinch gesture will allow to zoom the game in and out.
- Works flawlessly on open-island maps
- Adds letterboxing support for resolutions larger than 6012x2902 (on islands) and 1916x854 (in caverns)
- Does everything in memory without touching your executable
- Compatible with both Worms 2 and Worms World Party (as of v0.4). Supported versions:
 + Worms 2 - 1.05 (CD/GOG)
 + Worms World Party - 1.00 (Europe)
 + Worms World Party - 1.01 (Europe)

# TODO

- Worms 2: Adjust or provide a way to disable the televisor brackets while watching an instant replay.
- Chat panel quirks with zooming/resizing
