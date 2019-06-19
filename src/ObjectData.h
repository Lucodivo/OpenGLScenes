#pragma once

#include "LearnOpenGLPlatform.h"

void initializeCubeBuffers(uint32& VAO, uint32& VBO, uint32& EBO); 
void initializeLightBuffers(uint32& VAO, uint32& VBO, uint32& EBO);
void initializeQuadBuffers(uint32& VAO, uint32& VBO, uint32& EBO);
void initializeFrameBuffer(uint32& frameBuffer, uint32& rbo, uint32& frameBufferTexture, uint32 width, uint32 height);

// ===== cube values =====
#define BottomLeftTexture 0.0f, 0.0f
#define BottomRightTexture 1.0f, 0.0f
#define TopLeftTexture 0.0f, 1.0f
#define TopRightTexture 1.0f, 1.0f
const uint32 cubeVertexAttSizeInBytes = 8 * sizeof(float32);
const uint32 cubeNumElements = 12; // 2 triangles per side * 6 sides per cube
const float cubeVertexAttributes[] = {
	// positions          // texture positions  // normals (unit vectors orthogonal to surface) 
	// face #1
	-0.5f, -0.5f, -0.5f,  BottomLeftTexture,     0.0f,  0.0f, -1.0f, // bottom left
	0.5f, -0.5f, -0.5f,   BottomRightTexture,    0.0f,  0.0f, -1.0f, // bottom right
	0.5f,  0.5f, -0.5f,   TopRightTexture,       0.0f,  0.0f, -1.0f, // top right
	-0.5f,  0.5f, -0.5f,  TopLeftTexture,        0.0f,  0.0f, -1.0f, // top left
	// face #2
	-0.5f, -0.5f,  0.5f,  BottomLeftTexture,     0.0f,  0.0f, 1.0f, // bottom left
	0.5f, -0.5f,  0.5f,   BottomRightTexture,    0.0f,  0.0f, 1.0f, // bottom right
	0.5f,  0.5f,  0.5f,   TopRightTexture,       0.0f,  0.0f, 1.0f, // top right
	-0.5f,  0.5f,  0.5f,  TopLeftTexture,        0.0f,  0.0f, 1.0f, // top left
	// face #3
	-0.5f,  0.5f,  0.5f,  BottomRightTexture,    -1.0f,  0.0f,  0.0f, // bottom right
	-0.5f,  0.5f, -0.5f,  TopRightTexture,       -1.0f,  0.0f,  0.0f, // top right
	-0.5f, -0.5f, -0.5f,  TopLeftTexture,        -1.0f,  0.0f,  0.0f, // top left
	-0.5f, -0.5f,  0.5f,  BottomLeftTexture,     -1.0f,  0.0f,  0.0f, // bottom left
	// face #4
	0.5f,  0.5f,  0.5f,   BottomRightTexture,    1.0f,  0.0f,  0.0f, // bottom right
	0.5f,  0.5f, -0.5f,   TopRightTexture,       1.0f,  0.0f,  0.0f, // top right
	0.5f, -0.5f, -0.5f,   TopLeftTexture,        1.0f,  0.0f,  0.0f, // top left
	0.5f, -0.5f,  0.5f,   BottomLeftTexture,     1.0f,  0.0f,  0.0f, // bottom left
	// face #5
	-0.5f, -0.5f, -0.5f,  TopLeftTexture,        0.0f, -1.0f,  0.0f, // top left
	0.5f, -0.5f, -0.5f,   TopRightTexture,       0.0f, -1.0f,  0.0f, // top right
	0.5f, -0.5f,  0.5f,   BottomRightTexture,    0.0f, -1.0f,  0.0f, // bottom right
	-0.5f, -0.5f,  0.5f,  BottomLeftTexture,     0.0f, -1.0f,  0.0f, // bottom left
	// face #6
	-0.5f,  0.5f, -0.5f,  TopLeftTexture,        0.0f,  1.0f,  0.0f, // top left
	0.5f,  0.5f, -0.5f,   TopRightTexture,       0.0f,  1.0f,  0.0f, // top right
	0.5f,  0.5f,  0.5f,   BottomRightTexture,    0.0f,  1.0f,  0.0f, // bottom right
	-0.5f,  0.5f,  0.5f,  BottomLeftTexture,     0.0f,  1.0f,  0.0f  // bottom left
};

const uint32 cubeIndices[]{
	0, 2, 1,
	2, 0, 3,
	4, 5, 6,
	6, 7, 4,
	8, 9, 10,
	10, 11, 8,
	12, 14, 13,
	14, 12, 15,
	16, 17, 18,
	18, 19, 16,
	20, 22, 21,
	22, 20, 23
};
// ===== cube values =====


// ===== frame buffer quad values =====
const uint32 quadVertexAttSizeInBytes = 4 * sizeof(float32);
const float quadVertexAttributes[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

const uint32 quadIndices[]{
	0, 1, 2,
	0, 2, 3
};
// ===== frame buffer quad values =====