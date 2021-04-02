# Physically-Based Rendering

A short course on pathtracing for Stamatics Projects '21

## Week 3

There are no code tasks this week, mainly because the changes in code are quite significant and understanding them is a task in itself. Here's a summary.

### Configuration options

We're going to configure our renderer using a few macros, defined in `config.h`. We should keep a track of various "knobs" that our renderer will provide to the artist, and therefore this file is meant to simulate a sort of a "settings panel".

The scene that we're using is described in `scene.cpp`. More spheres can be added here and their material properties can be tweaked. We'll eventually have a number of different scenes that we'll keep here and switch between them using the `PBR_ACTIVE_SCENE` config macro.

### Materials

A "material" is a general way to define surface properties for objects, like color. Right now, we have two kinds of materials - diffuse and specular.

1. Diffuse reflections are the haphazard reflections on a rough surface. You can see these on matte surfaces, they don't shine.
1. Specular reflections are perfect reflections from a mirror-like surface. You can see these on mirrors and shiny metals.

In reality, a surface is neither perfectly diffuse nor perfectly specular, which is why most renderers let artists define these properties on a scale of 0 to 1. The BRDF then uses these parameters to determine which rays to favor. To keep things simple for now, we will consider a surface to be either diffuse or specular, but not both. This is implemented with the `EMaterialType` enum.

### Sampler

To evaluate the integration term of the rendering equation, we need to discretize it and consider a finite number of rays approaching the current point. The `DiscreteSampler` class chooses these rays. For now, we choose just one ray. For specular materials, we choose the reflection about the surface normal. For diffuse materials, we add a random offset to the direction of this reflection. The magnitude of this offset can be configured with `PBR_DISCRETE_SAMPLER_DIFFUSE_OFFSET`.

### BRDF

After the sampler gives us a set of rays to evaluate, we need to find the BRDF term of the integrand. In the current state of our renderer, we implement a simple `DiscreteBRDF`. For specular surfaces, this gives a coefficient of 1, that is, the reflection goes unchanged. For diffuse surfaces, we multiply the surface color and a cos term to model the surface as per [Lambert's cosine law](https://en.wikipedia.org/wiki/Lambert%27s_cosine_law).

**NOTE:** It is very important to note here that the combination of material, sampler and the BRDF is what's involved in solving the rendering equation. The BRDF is what makes your surface look realistic, and therefore must be chosen carefully. We'll look at much better BRDFs and samplers in the future.

### Integrator

The integrator is an interface that we'll use from `main.cpp`. This integrator performs the necessary recursive raytracing, and calls the sampler and BRDF when there's a hit. The current implementation of the integrator calculates a simple Reimann sum.

We use a C++ feature called _templates_ in the integrator so that we can swap out the BRDF and sampler with other implementations and compare, with minimal changes to the code. This is why we tried to decouple the three steps of sampling, BRDF and integration.

### Result

You can run the program and get an output image like this. It should be in `build\bin\Debug\out.png`

![out.png](screenshots/out.png)

## Build Instructions

### Windows

**Requirements:**
- A C++ compiler. Visual Studio is the simplest way to set this up on Windows. Follow the steps [here](https://devblogs.microsoft.com/cppblog/getting-started-with-visual-studio-for-c-and-cpp-development/).
- CMake. Download [here](https://cmake.org/download/). Get the Windows installer (file with .msi extension). Run the setup wizard and install. If there's an option to add something to your PATH, do it. This will allow you to run a `cmake` command from the command prompt.
- git. You can get it [here](https://git-scm.com/downloads). The download can be extremely slow sometimes, let me know if that is the case.
- A text editor. I use [vscode](https://code.visualstudio.com/). For a tutorial to use it with C++, see [this page](https://code.visualstudio.com/docs/cpp/config-msvc). Also install the CMake extension.

**Build:**

Open the "Developer Command Prompt" that was installed with Visual Studio. You will be greeted with something like this
```
C:\...some-path...>_
```
The path mentioned here is the "current directory". All commands entered will be relative to the current directory. You can change the current directory by typing `cd <new-path>` and Enter. Note that if you want to change the drive from C to D, do `d:` first, and then you'll be able to `cd` into any directory in the D drive.

Go to the path where you want to download this repository. I will assume that this is in `D:\code\`. Use git to download the repository
```
git clone https://github.com/mayant15/stamatics-pbr
```
You should now have a folder `D:\code\stamatics-pbr` with all code. Go into this folder with `cd stamatics-pbr`. We can now use CMake to compile the code.
```
mkdir build
cd build
cmake ..
cmake --build .
```

You should now have an executable in `build\bin` or `build\bin\Debug`.
- `mkdir build` creates a new folder called `build` inside the repository. This is going to store temporary data that CMake needs.- `cd build` to go into the `build` folder
- `cmake ..` starts cmake's _configuration step_. This step needs a configuration file with the settings for this project. This file, the `CMakeLists.txt`, has already been provided with the repository.
- `cmake --build .` tells cmake to compile the configured project. This will use our C++ compiler to generate the final executable.

### Ubuntu

**Requirements:**
- A C++ compiler. You can get this by running `sudo apt install build-essential`, which installs `gcc` and `g++`, among other things.
- CMake. Get this via `pip` if you have python installed, with `pip install cmake`. The version in `apt` is fairly old.
- git. You can get this by running `sudo apt install git`.
- A text editor. I use [vscode](https://code.visualstudio.com/). For a tutorial to use it with C++, see [this page](https://code.visualstudio.com/docs/cpp/config-linux). Also install the CMake extension.

**Build:**

Open a terminal. You will be greeted with something like this
```
user@device:path$ _
```
The path mentioned here is the "current directory". All commands entered will be relative to the current directory. You can change the current directory by typing `cd <new-path>` and Enter.

Go to the path where you want to download this repository. I will assume that this is in `~/code/`. Use git to download the repository
```
git clone https://github.com/mayant15/stamatics-pbr
```
You should now have a folder `~/code/stamatics-pbr` with all code. Go into this folder with `cd stamatics-pbr`. We can now use CMake to compile the code.
```
mkdir build
cd build
cmake ..
cmake --build .
```

You should now have an executable in `build/bin` or `build/bin/Debug`.
- `mkdir build` creates a new folder called `build` inside the repository. This is going to store temporary data that CMake needs.- `cd build` to go into the `build` folder
- `cmake ..` starts cmake's _configuration step_. This step needs a configuration file with the settings for this project. This file, the `CMakeLists.txt`, has already been provided with the repository.
- `cmake --build .` tells cmake to compile the configured project. This will use our C++ compiler to generate the final executable.
