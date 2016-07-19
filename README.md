# ofxRenderToolkit

## Introduction

An addon for incorporating advanced rendering techniques to your scenes. Currently includes clustered forward shading and image-based lighting (IBL).

**THIS IS A WORK IN PROGRESS, USE AT YOUR OWN RISK**

## Usage

Note that this is an advanced addon and is not "plug and play". You'll need to copy and modify the shaders (probably only the vertex shader) for your project.
A good first step is to start with the example, which includes options for rendering instanced geometry, and rendering inside an `ofFbo`.

## Dependencies

None for the addon, but the example uses [ofxImGui](https://github.com/jvcleave/ofxImGui/).

## Compatibility

* Only compatible with openFrameworks using [glm](http://glm.g-truc.net/0.9.7/index.html) for vector math, so `master` from Jul 19 2016 onwards should be good.
* OpenGL 3.3 and up (programmable pipeline).
