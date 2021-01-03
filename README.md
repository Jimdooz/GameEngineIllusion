# GameEngineIllusion

Illusion is a custom game engine developed in C++ in the context of the "Game Engine" teaching unit at the University of Montpellier, France. It has been created by Romain Saclier, Elodie Tribout and Lazlo Barragan.

## Table of contents
* [Setup](#setup)
* [Features](#features)
* [Technologies](#technologies)

## Setup

First of all, clone this repository to your desktop.

Several files are pre-configured in order to generate an executable version of the editor. For instance, the project has been tested on Windows NT OS.

### Windows NT

The current version of the project is fully functional on Windows NT 10.0.17763.316 (February, 2019) and has been built with Visual Studio 2019. To generate it, please follow the next steps:

* Clean the project from any residual previous builds by clicking on or executing with CL `GameEngineIllusion\Illusion\clean.bat` ;
* Generate a VS solution by executing the `GameEngineIllusion\Illusion\generate.bat` file. The following files will be created:
	* Illusion.sln;
	* IllusionEngine.vcxproj;
	* IllusionEditor.vcxproj.
* On Visual Studio 2019, do `File > Open > Project/Solution` and choose `GameEngineIllusion\Illusion\Illusion.sln` ;
* Depending on the build type the user needs, select "Debug" or "Release" and also the architectural target (x64_86 or x86) then run a building process.

## Features

* Creating, saving and loading projects through a File Explorer;
* Entity-Component-System pattern;
* Hierarchized entities:
	* add entities to the current scene;
	* hard delete entities, i.e. even their children;
	* soft delete entities, i.e. keep their children by increasing their level in the hierarchy the deleting their parents.
* Inspector window:
	* look at an entity properties;
	* add/remove its components; 
	* alter components if needed;
* Physics:
	* Rigid Body component:
		* Fixing position and rotation on X/Y/Z axis;
		* Apply gravity on an axis, a mass and a friction force;
	* Colliding box and collision detection;
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
* GLM: 0.9.8.6
* GLAD: 0.1.34
* GLFW: 3.3.2 
* Open Asset Import Library (Assimp): 4.1.0
* Dear ImGui: 1.79