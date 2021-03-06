#pragma once

#include <glm/glm.hpp>

#include "../../LearnOpenGLPlatform.h"
#include "../../common/Kernels.h"
#include "../FirstPersonScene.h"
#include "../../common/ObjectData.h"
#include "../../ShaderProgram.h"

class ReflectRefractScene final : public FirstPersonScene
{
public:
  ReflectRefractScene();
  void init(Extent2D windowExtent);
  Framebuffer drawFrame();
  void deinit();
  void inputStatesUpdated();
  virtual void framebufferSizeChangeRequest(Extent2D windowExtent);
  const char* title();

private:
  ShaderProgram* explodingReflectionShader;
  ShaderProgram* exploding10InstanceReflectionShader;
  ShaderProgram* reflectionShader;
  ShaderProgram* reflection10InstanceShader;
  ShaderProgram* explodingRefractionShader;
  ShaderProgram* refractionShader;
  ShaderProgram* skyboxShader;
  ShaderProgram* normalVisualizationShader;
  ShaderProgram* normalVisualization10InstanceShader;

  VertexAtt cubeVertexAtt;
  VertexAtt skyboxVertexAtt;

  Framebuffer drawFramebuffer;

  Model* nanoSuitModel;
  glm::mat4 nanoSuitModelMat;

  uint32 skyboxTextureId;

  // frame rate
  float32 initTime = 0.0f;
  float32 deltaTime = 0.0f;  // Time between current frame and last frame
  float32 lastFrame = 0.0f; // Time of last frame

  const float32 angularSpeed = 7.3f;
  const glm::vec3 orbitAxis = glm::vec3(0.0f, 1.0f, 0.0f);
  const glm::vec3 rotationAxis = glm::vec3(1.0f, 0.3f, 0.5f);

  double reflactionModeSwitchTimer = 0.0f;
  double modeSwitchTimer = 0.0f;
  float32 windowAspectRatio;

  void nextModelReflaction();
  void prevModelReflaction();
  void nextMode();
  void prevMode();
};