#pragma once

#include <glm/glm.hpp>

#include "../../LearnOpenGLPlatform.h"
#include "../../common/Kernels.h"
#include "../FirstPersonScene.h"
#include "../../common/ObjectData.h"
#include "../../ShaderProgram.h"
#include "../../common/Util.h"

const glm::vec3 colors[] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.875f, 0.125f, 0.0f),
        glm::vec3(0.75f, 0.25f, 0.0f),
        glm::vec3(0.625f, 0.375f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.0f),
        glm::vec3(.375f, 0.625f, 0.0f),
        glm::vec3(0.25f, 0.75f, 0.0f),
        glm::vec3(0.125f, 0.875f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.875f, 0.125f),
        glm::vec3(0.0f, 0.75f, 0.25f),
        glm::vec3(0.0f, 0.625f, 0.375f),
        glm::vec3(0.0f, 0.5f, 0.5f),
        glm::vec3(0.0f, 0.375f, 0.625f),
        glm::vec3(0.0f, 0.25f, 0.75f),
        glm::vec3(0.0f, 0.125f, 0.875f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.125f, 0.0f, 0.875f),
        glm::vec3(0.25f, 0.0f, 0.75f),
        glm::vec3(0.375f, 0.0f, 0.625f),
        glm::vec3(0.5f, 0.0f, 0.5f),
        glm::vec3(0.625f, 0.0f, 0.375f),
        glm::vec3(0.75f, 0.0f, 0.25f),
        glm::vec3(0.875f, 0.0f, 0.125f),
};

class InfiniteCubeScene final : public FirstPersonScene
{
public:
  InfiniteCubeScene();
  void init(Extent2D windowExtent);
  Framebuffer drawFrame();
  void deinit();
  virtual void framebufferSizeChangeRequest(Extent2D windowExtent);
  const char* title();

private:
  ShaderProgram* cubeShader = NULL;

  VertexAtt cubeVertexAtt;

  glm::mat4 projectionMat;

  uint32 outlineTexture;
  uint32 globalVSUniformBufferID;
  uint32 globalVSBufferBindIndex = 0;
  uint32 globalVSBufferViewMatOffset = sizeof(glm::mat4);
  const float32 cubeRotationAngle = 2.5f;

  // frame rate
  float32 deltaTime = 0.0f;  // Time between current frame and last frame
  float32 lastFrame = 0.0f; // Time of last frame

  Framebuffer drawFramebuffer;
  Framebuffer infiniteCubeTextureFramebuffer;
  uint32 colorIndex = 0;
};