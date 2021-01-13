# GameEngineIllusion

Illusion is a custom game engine developed in C++ in the context of the "Game Engine" teaching unit at the University of Montpellier, France. It has been created by Romain Saclier, Elodie Tribout and Lazlo Barragan.

## Table of contents
* [Setup](#setup)
* [Features](#features)
* [Technologies](#technologies)

## Setup

First of all, clone this repository to your desktop.

Several files are pre-configured in order to generate an executable version of the editor. For instance, the project has been tested on Windows NT OS. Make sure to support OpenGL version 4.5

### Windows NT

The current version of the project is fully functional on Windows NT 10.0.17763.316 (February, 2019) x64 and has been built with Visual Studio 2019. To generate it, please follow the next steps:

* Clean the project from any residual previous builds by clicking on or executing with CL `GameEngineIllusion\Illusion\clean.bat` ;
* Generate a VS solution by executing the `GameEngineIllusion\Illusion\generate.bat` file. The following files will be created:
	* Illusion.sln;
	* IllusionEngine.vcxproj;
	* IllusionEditor.vcxproj;
	* Glad.vcxproj;
* On Visual Studio 2019, do `File > Open > Project/Solution` and choose `GameEngineIllusion\Illusion\Illusion.sln` ;
* Depending on the build type the user needs, select "Debug" or "Release" and also the architectural target (x64_86) then run a building process.
* x64 is the only architecture currently supported
* **To see the engine's limits select Release mode it allow much better performance**

### Linux

The current version of the project doesn't compile due to compiler specific code and missing dependencies.
However, it was compiled before the remake settings for Linux have to be adjusted and the dependencies added.

## Features

* Creating, saving and loading projects through a File Explorer;
* Entity-Component-System pattern;
	* cache friendly architecture
* Hierarchized entities:
	* add entities to the current scene;
	* change entity level with drag'n drop.
	* hard delete entities, i.e. even their children;
	* soft delete entities, i.e. keep their children by increasing their level in the hierarchy the deleting their parents;
* Inspector window:
	* look at an entity properties;
	* add/remove its components; 
	* alter components if needed;
* Mesh:
	* Import system for OBJ and FBX files;
	* Material with a material editor;
	* Apply custom shaders;
* Animator:
	* load animation from OBJ/FBX files;
	* skeleton handling (if bones are filled in the imported file)
* Physics:
	* Rigid Body component:
		* Fixing position and rotation on X/Y/Z axis;
		* Apply gravity on an axis, a mass and a friction force;
	* Colliding box and collision detection;
* Controller:
	* Keyboard and Gamepad inputs;
* Camera:
	* Play Camera mode which can target a scene's object;
	* Switching between Play and Edit Camera mode;
* Audio:
	* Loading MP3, WAV and FLAC audio files;
	* Play, pause and loop sounds;
	* 3D audio effect.
* Post-Processing:
	* HDR effect;
	* Shadow mapping;
	* Bloom effect;
* Real-time statistics
	* frame time refresh in seconds;
	* FPS by moving average;
	* switching on/off Engine components:
		* Update Loop;
		* Compute Models;
		* Physics;
		* Rendering;
* Time handler:
	* config the scale time;
	* play, pause, step by step;
	* reload the whole scene with the last saved states.

## Technologies
The project is created with:

* C++17
* OpenGL: 4.5
* GLM: 0.9.8.6
* GLAD: 0.1.34
* GLFW: 3.3.2 
* Open Asset Import Library (Assimp): 5.0.1
* Dear ImGui: 1.79
* irrKlang: 1.6.0