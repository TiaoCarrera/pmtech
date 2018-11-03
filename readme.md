# pmtech [![Build Status](https://travis-ci.org/polymonster/pmtech.svg?branch=master)](https://travis-ci.org/polymonster/pmtech) [![Build Status](https://ci.appveyor.com/api/projects/status/5n3aguiq1ppjrhws?svg=true)](https://ci.appveyor.com/project/polymonster/pmtech)

Follow work in progress: 
[Trello](https://trello.com/b/ciujzpUT)

A lightweight code base with powerful features that can be used for games and real-time 3D applications. This project focuses on data-oriented and multithreaded systems with minimalistic c-style api's with a strong emphasis on performance. [more info](https://polymonster.github.io/articles.html)  

**Platform support**  
- Platforms: Windows, MacOS, Linux, iOS, Android (wip).   
- Renderers: Direct3D11, OpenGL3.1+, OpenGLES3+.   
- Compilers: vs2015, vs2017, Clang 6, Apple LLVM 9, Gcc 7. 

**Features**  
- Multithreaded Systems - Async render, physics, audio and component entity system threads. 
- Data-oriented Component Entity System - SoA memory layout for blazing fast scene representation.
- PMFX - High level data driven renderer, shader and post-processing system configured with json scripts.
- Hot loading - reload pmfx configs, shaders, models and textures in real time for rapid development.
- Tools - Scene editor, 3D volume / signed distance field generator.
- Asset Pipeline - binary model and skeleton format, texture compression, platform agnostic shader compilation. 

There are examples included of how to use the api's, set up projects and data, view them [here...](https://polymonster.github.io/index.html)

![sdf shadow](https://polymonster.github.io/assets/gifs/sdf-shadow.gif)

**Getting started** 

Take a look at getting started quick [guide](https://polymonster.github.io/articles.html#getting-started)

**Shoutout to thirdparty stuff!**  
- [Jsmn](https://github.com/zserge/jsmn)
- [Premake](https://github.com/premake/premake-core)
- [Bullet](https://github.com/bulletphysics/bullet3)
- [ImGui](https://github.com/ocornut/imgui)
- [NVTT](https://github.com/castano/nvidia-texture-tools)

