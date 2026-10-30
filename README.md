# NosTale Client Creator

Creates a copy of `NostaleClientX.exe` that connects to a custom server (`IP:port`), for private servers.

The copy is patched so it starts directly (double click, no launcher needed), and the login server IP / port are rewritten.

## Build

Qt 6 (MinGW 64-bit) is required. Adapt `QT_DIR` / `MINGW_DIR` at the top of the scripts, then run:

- `build_release.bat` → `build\release\NostaleClientCreator.exe`
- `build_debug.bat` → `build\debug\NostaleClientCreator.exe`

The output folder only contains the exe and the Qt dlls it needs.

Requires a C++23 compiler (MinGW 13+).

Created by [Pumba98](https://www.elitepvpers.com/forum/members/3141926-pumba98.html), maintained by [Zanou](https://www.elitepvpers.com/forum/members/7384346-zanouu.html).

[![Discord](https://img.shields.io/badge/Discord-join%20the%20community-5865F2?logo=discord&logoColor=white)](https://discord.gg/x6JY7QGDyx)
