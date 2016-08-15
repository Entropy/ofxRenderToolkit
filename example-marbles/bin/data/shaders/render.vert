// Filename: main.vert
// 
// Copyright © James Acres
// http://www.jamesacres.com
// http://github.com/jacres
// @jimmyacres
// 
// Created: Tue Nov 10 21:26:11 2015 (-0500)
// Last-Updated: Mon Jan 25 10:15:51 2016 (-0500)

#version 400

#pragma include <inc/ofDefaultUniforms.glsl>
#pragma include <inc/ofDefaultVertexInAttributes.glsl> 

#pragma include <inc/viewData.glsl>

out vec2 vTexCoord0;

const vec2 triangleVerts[3] = vec2[](
  vec2( -1, -1 ),
  vec2(  3, -1 ),
  vec2( -1,  3 )
);

const vec2 triangleTexCoords[3] = vec2[](
  vec2( 0, 0 ),
  vec2( 2, 0 ),
  vec2( 0, 2 )
);

void main( void )
{
   	vTexCoord0 = triangleTexCoords[ gl_VertexID ];

	vec4 vertex = vec4( triangleVerts[ gl_VertexID ], 0.0f, 1.0f );
    gl_Position = vec4( vertex.xyz, 1.0f );
}
