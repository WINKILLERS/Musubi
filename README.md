# Musubi

## Description

`Musubi`. is a Japanese braiding method for making decorative and functional cords, and it is depicted in several scenes in the movie. Musubi is a knot, a tying together, of connecting people and things. The photo shows two kumihimo cords in a musubi knot.

And here it is a `remote administration tool (RAT)` written in pure `C++` with `Qt`, and build system `xmake`.

## Build

![Github CI Badge](https://github.com/WINKILLERS/Musubi/actions/workflows/xmake.yaml/badge.svg)

Clone the repo and change the working directory.

```bash
git clone https://github.com/WINKILLERS/Musubi.git
cd Musubi
```

Install xmake (if you are using linux or macos, please following [xmake website](https://xmake.io/#/guide/installation) to find installing method)

```powershell
Invoke-Expression (Invoke-WebRequest 'https://xmake.io/psget.text' -UseBasicParsing).Content
```

Build the program (only tested on windows, please feel free to open issues when having build problem)

```bash
xmake -y
```

And run the server or client

```bash
xmake run MusubiServer
xmake run MusubiNativeClient
```
