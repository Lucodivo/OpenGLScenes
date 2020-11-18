#pragma once

#define GLFW_INCLUDE_NONE // ensure GLFW doesn't load OpenGL headers

#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "../LearnOpenGLPlatform.h"

#define NO_FRAMEBUFFER_ATTACHMENT 0

// TODO: Split this file between GLFW util(toWindowMode), OpenGL util (load2DTexture), and general util(swap)
struct Framebuffer {
  uint32 id;
  uint32 colorAttachment;
  uint32 depthStencilAttachment;
  uint32 width;
  uint32 height;
};

// TODO
enum FramebufferCreationFlags {
  FramebufferCreate_NoValue = 0,
  FramebufferCreate_NoDepthStencil = 1 << 0,
  FramebufferCreate_color_sRGB = 1 << 1,
};

void load2DTexture(const char* imgLocation, uint32& textureId, bool flipImageVert = false, bool inputSRGB = false, uint32* width = NULL, uint32* height = NULL);
void loadCubeMapTexture(const char* const imgLocations[6], uint32& textureId, bool flipImageVert = false);
Framebuffer initializeFramebuffer(uint32 width, uint32 height, FramebufferCreationFlags flags = FramebufferCreate_NoValue);
void deleteFramebuffer(Framebuffer* framebuffer);
void deleteFramebuffers(uint32 count, Framebuffer** framebuffer);
void toFullScreenMode(GLFWwindow* window);
void toWindowedMode(GLFWwindow* window, const uint32 width, const uint32 height);
void snapshot(Framebuffer* framebuffer);
void swap(float32* a, float32* b);
glm::mat4& reverseZ(glm::mat4& mat);