# raylib-platformer

## Introduction

This is a repoitory that uses raylib to create a 2D platformer game (maybe). You can find out more about raylib [here](https://github.com/raysan5/raylib) if you are interested! This project is compiled with **C++17** and finally uses **CMake** to build the project!

## Installation

- First, begin my cloning the repository by downloading the .zip here on GitHub via the green button that reads <>Code.

- Alternatively, you can clone the repository on your machine within the terminal by inputting the following command:

```
git clone https://github.com/McFlubberBubber/raylib-platformer.git
```

- Once you have the repository cloned, navigate to the root directory of repo and create a directory dedicated to the build. I prefer to create a directory called 'build', which you can do so on Windows like so:

```
mkdir build
```

- Then navigate into the build directory and run the following commands:

```
# Run this command first.
cmake ..

# After that is done, run this.
cmake --build .
```

- Once completed, this should have **hopefully**, created an executable within the build/game path, which you can directly launch from either the terminal or the file explorer.

**NOTE:** I have not tested this build system on other operating systems such as Linux or MacOS, so there may be issues in that regard. If there are any issues, please let me know so I can sort out the build system to work out with it!

