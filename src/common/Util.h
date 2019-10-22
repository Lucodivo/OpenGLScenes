#pragma once

#include "../LearnOpenGLPlatform.h"

void load2DTexture(const char* imgLocation, uint32& textureId, bool flipImageVert = false, bool sRGB = false);
void loadCubeMapTexture(const char* const imgLocations[6], uint32& textureId, bool flipImageVert = false);