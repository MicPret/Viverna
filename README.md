# Viverna Engine

---
###### (put some logo here)
---

Viverna is a simple C++17 framework I wrote for building cross-platform games for Windows, Linux and Android. As of now, it uses OpenGL 4.6 on Windows/Linux and OpenGL ES 3.2 on Android.

---

## Usage

### Install dependencies

To use Viverna, you'll need the following:

- CMake
    - version 3.24 or later
- Android SDK
    - ANDROID_HOME environment variable set to the SDK path
    - Android NDK

### Make your game

1. Clone the repository: `git clone https://github.com/MicPret/Viverna.git`
    - ###### (or fork it)
2. Edit `Application.cpp` in `src/game`
3. Edit `config.yaml` to give your game a name

### Build

- Desktop
    1. `cmake --preset desktopDebug` or `cmake --preset desktopRelease`
    2. `cmake --build build`
    3. Your executable will be in the `build` folder
- Android
    1. `cd android`
    2. Use the gradle wrapper to assemble the APK
        - Windows: `.\gradlew.bat assembleDebug`
        - Linux: `./gradlew assembleDebug`
    3. Your APK will be in `android/app/build/outputs/apk/debug`
    4. If you want to directly install the APK instead of assembling it, you can pair and connect a device through `adb` in the `platform-tools` of the Android SDK and run `installDebug` instead of `assembleDebug`

### Run!

Remember you need OpenGL 4.6 on desktop and OpenGL ES 3.2 on Android and have fun!

![Screenshot of Snake demo](screenshot.png)
###### *This screenshot is from the demo you can find on branch demo/snake*

---

## Special thanks

This project depends on the following libraries:

- [GLFW](https://github.com/glfw/glfw)
- [glad2](https://github.com/Dav1dde/glad)
- [stb_image](https://github.com/nothings/stb)
