#include "Util.h"

#include <stb/stb_image.h>
#include <glad/glad.h>
#include <iostream>

void load2DTexture(const char* imgLocation, uint32& textureId, bool flipImageVert, bool sRGB)
{
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);

  // load image data
  int width, height, numChannels;
  stbi_set_flip_vertically_on_load(flipImageVert);
  unsigned char* data = stbi_load(imgLocation, &width, &height, &numChannels, 0);
  if (data)
  {
    uint32 internalFormat;
    uint32 externalFormat;
    if (sRGB)
    {
      if (numChannels == 3)
      {
        internalFormat = GL_SRGB;
        externalFormat = GL_RGB;
      } else
      {
        internalFormat = GL_SRGB_ALPHA;
        externalFormat = GL_RGBA;
      }
    } else
    {
      if (numChannels == 3)
      {
        internalFormat = externalFormat = GL_RGB;
      } else if (numChannels == 4)
      {
        internalFormat = externalFormat = GL_RGBA;
      } else if (numChannels == 1)
      {
        internalFormat = externalFormat = GL_RED;
      }
    }

    glTexImage2D(GL_TEXTURE_2D, // target
                 0, // level of detail (level n is the nth mipmap reduction image)
                 internalFormat, // kind of format we want to store the texture
                 width, // width of texture
                 height, // height of texture
                 0, // border (legacy stuff, MUST BE 0)
                 externalFormat, // Specifies format of the pixel data
                 GL_UNSIGNED_BYTE, // specifies data type of pixel data
                 data); // pointer to the image data
    glGenerateMipmap(GL_TEXTURE_2D);

    // set texture options
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // disables bilinear filtering (creates sharp edges when magnifying texture)
  } else
  {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data); // free texture image memory
}

void loadCubeMapTexture(const char* const imgLocations[6], uint32& textureId, bool flipImageVert)
{
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(flipImageVert);
  for (uint32 i = 0; i < 6; i++)
  {
    unsigned char* data = stbi_load(imgLocations[i], &width, &height, &nrChannels, 0);
    if (data)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                   0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
      );
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    } else
    {
      std::cout << "Cubemap texture failed to load at path: " << imgLocations[i] << std::endl;
    }
    stbi_image_free(data);
  }
}

void swap(float32* a, float32* b)
{
  float32 tmp = *a;
  *a = *b;
  *b = tmp;
}
