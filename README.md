# Prerequisites
* [CMake](https://cmake.org/) (added to PATH)
* [Visual Studio 6 (**Build 8168**)](https://winworldpc.com/product/microsoft-visual-stu/60)
* [MASM 6.11 + 6.13 Update](https://archive.org/details/en_masm611)
* [NASM](https://www.nasm.us/)
* Duke Nukem Forever (2001)

# Building
1. Copy the contents of the `October 26` directory into the `Duke4` directory.
    * Do not overwrite any existing files!
2. Delete `Duke4/DirectX8/Inc/basetsd.h`.
3. Open `VSE600ENU1.ISO` and copy the `COMMON` and `VC98` directories into the `VS6` directory.
    * Do not overwrite the existing `VCVARS32.BAT` script!
4. Install MASM 6.11 and update it to MASM 6.13, then copy the contents of the `BIN` directory into `/VS6/VC98/BIN/`.
    * A pre-patched `BIN` directory is available [here](https://cdn.discordapp.com/attachments/309328208464183296/1154082976737542164/MASM613.7z).
5. Install NASM on your machine.
6. Run `build_vc6_release.cmd` or `build_vc6_debug.cmd`

# Visual Studio 2022
The `generate_vc17.cmd` script will generate a Visual Studio 2022 solution in `/Duke4/Build/VS2022/`.

# Galaxy
Building `Galaxy` with Visual Studio 2022 is currently unsupported. It can be built with VS6 by opening `devcmd_vc6.cmd` and entering:
```cmd
cmake --preset Release
cmake --build --preset Release --target Galaxy
```
