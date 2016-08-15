// Filename: main.frag
// 
// Copyright © James Acres
// http://www.jamesacres.com
// http://github.com/jacres
// @jimmyacres
// 
// Created: Sat Nov 14 13:50:14 2015 (-0500)
// Last-Updated: Mon Jan 25 17:05:45 2016 (-0500)

#version 330 core

#pragma include <inc/toneMapping.glsl>

uniform sampler2D uTexture;

in vec2 vTexCoord0;

out vec4 fragColor;

void main( void )
{
	vec4 baseColor = texture( uTexture, vTexCoord0 );

	fragColor = vec4( colorCorrect( baseColor.rgb, uExposure, uTonemapType, uGamma ), baseColor.a );
	//fragColor = baseColor;
}
