# Dream
( Data Rules Everything Around Me ) 

# What?

Dream is a Demo and Game Engine.

## Key Features
* Platform Independant
* Written in C++
* OpenGL Graphics
* OpenAL Audio
* Bullet Physics & Collision
* AngelScript Scripting
* Project Editor Written with ImGui

## Architecture
This section will describe the key concepts used when working with Dream.

### Project
Projects are the highest level of object in Dream.  A Project consists of a json and an assets directory structure. The Project holds definitions of Scene(s), Actor(s) and AssetDefinition(s).

### Assets
A Project will contain several 'Asset' objects. An Asset describes something that can be added into a Scene, such as a 3D model, Audio Clip or Spline Path (full list below).

#### Asset Definitions
Asset Definitions are used to define an Asset's properties (at instanciation).
They are defined once at the Project level and can be reused multiple times within your Screen to create Asset Instances. These Asset Instances are then assigned to Actors within a scene. Asset Definitions are defined at the Project level so that they can be shared by many Scene Objects. There are several types of Asset available in Dream.

* Animation
* Audio
* Font
* Light
* Model
* Emitter
* Path
* Physics Object
* Script
* Shader
* Sprite
* Texture

### Scenes
A Project contains one or more Scenes that are to be displayed. A Scene is composed of one or more Actors. A Scene may for example represent a level, environment or stage within your application.

### Actors
Actors are used to construct a 'Scenegraph' that describes the relationship of objects within the Scene. A Scene has a single 'Root' Actor with n-children. Actors are arranged within a parent-child relationship tree, through which they can inherit their transformation. Each Actor is assigned zero or more 'Assets Definitions' to instanciate at runtime.

## DreamTool
DreamTool is the Project editor for Dream. It is cross-platrofm, written with Dear ImGui. It allows you to build and test Scenes that can later be run using the standalone Dream engine. Both the tool and engine share the same codebase 'DreamCore' so runtime results will be identical.

## Build Dependencies
Dream uses the following libraries.

* ALUT
* Assimp
* Bullet
* GLM
* GLAD
* GLFW
* AngelScript
* OggVorbis
* OpenAL
* OpenGL
* SPDLOG

### Example: Installing Ubuntu Build Dependencies From Repositories (apt)
```
sudo apt-get -y install \
    libassimp-dev \
    libbullet-dev \
    libopenal-dev \
    libvorbis-dev \
    libalut-dev \
    libspdlog-dev
```