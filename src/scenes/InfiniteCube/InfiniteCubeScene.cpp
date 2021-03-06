#include <iostream>
#include "InfiniteCubeScene.h"
#include "../../common/FileLocations.h"
#include "../../common/ObjectData.h"
#include "../../common/Util.h"

const uint32 colorAttachmentTextureIndex = 0;
const uint32 outlineTextureIndex = 1;

InfiniteCubeScene::InfiniteCubeScene(): FirstPersonScene(){}

const char* InfiniteCubeScene::title()
{
  return "Infinite Cube";
}

void InfiniteCubeScene::init(Extent2D windowExtent)
{
  FirstPersonScene::init(windowExtent);

  cubeShader = new ShaderProgram(posNormTexVertexShaderFileLoc, discardAlphaFragmentShaderFileLoc);

  cubeVertexAtt = initializeCubePosNormTexVertexAttBuffers();

  load2DTexture(outlineTextureLoc, outlineTexture);

  drawFramebuffer = initializeFramebuffer(windowExtent);
  uint32 framebufferDimen = windowExtent.width < windowExtent.height ? windowExtent.width : windowExtent.height;
  infiniteCubeTextureFramebuffer = initializeFramebuffer(Extent2D{ framebufferDimen, framebufferDimen }, FramebufferCreate_NoDepthStencil);

  const float32 aspectRatio = (float32)windowExtent.width / (float32)windowExtent.height;
  projectionMat = glm::perspective(glm::radians(camera.Zoom), aspectRatio, 0.1f, 100.0f);

  {
    glGenBuffers(1, &globalVSUniformBufferID);

    glBindBuffer(GL_UNIFORM_BUFFER, globalVSUniformBufferID);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

    cubeShader->bindBlockIndex("globalBlockVS", globalVSBufferBindIndex);
  }
}

void InfiniteCubeScene::deinit()
{
  FirstPersonScene::deinit();

  cubeShader->deleteShaderResources();
  delete cubeShader;

  deleteVertexAtt(cubeVertexAtt);

  glDeleteTextures(1, &outlineTexture);

  Framebuffer* framebuffers[] = { &drawFramebuffer, &infiniteCubeTextureFramebuffer };
  deleteFramebuffers(ArrayCount(framebuffers), framebuffers);

  glDeleteBuffers(1, &globalVSUniformBufferID);
}

Framebuffer InfiniteCubeScene::drawFrame()
{
  glViewport(0, 0, windowExtent.width, windowExtent.height);

  glActiveTexture(GL_TEXTURE0 + colorAttachmentTextureIndex);
  glBindTexture(GL_TEXTURE_2D, infiniteCubeTextureFramebuffer.colorAttachment);
  glActiveTexture(GL_TEXTURE0 + outlineTextureIndex);
  glBindTexture(GL_TEXTURE_2D, outlineTexture);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  float32 t = getTime();
  deltaTime = t - lastFrame;
  lastFrame = t;

#if 0
  // control when we "change frames" for the cube
    local_access float32 elapsedTime = 0;
    elapsedTime += deltaTime;
    if (elapsedTime > 1.0f)
    {
      elapsedTime = 0;

      // set background color
      // more abrupt color changes
      colorIndex = (colorIndex + 1) % ArrayCount(colors);
      glClearColor(colors[colorIndex].x, colors[colorIndex].y, colors[colorIndex].z, 1.0f);
    }
#else
  // set background color
  // smoother color change
  float32 lightR = (sinf((t + 30.0f) / 3.0f) / 2.0f) + 0.5f;
  float32 lightG = (sinf((t + 60.0f) / 8.0f) / 2.0f) + 0.5f;
  float32 lightB = (sinf(t / 17.0f) / 2.0f) + 0.5f;
  glClearColor(lightR, lightG, lightB, 1.0f);
#endif

  glBindFramebuffer(GL_FRAMEBUFFER, drawFramebuffer.id);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glm::mat4 viewMat = camera.UpdateViewMatrix(deltaTime, cameraMovementSpeed);

  glBindBuffer(GL_UNIFORM_BUFFER, globalVSUniformBufferID);
  // update global view matrix uniform
  glBindBufferRange(GL_UNIFORM_BUFFER,    // target
                    globalVSBufferBindIndex,  // index of binding point
                    globalVSUniformBufferID,  // buffer id
                    0,            // starting offset into buffer object
                    4 * 16);        // size: 4 vec3's, 16 bits alignments
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projectionMat));
  glBufferSubData(GL_UNIFORM_BUFFER, globalVSBufferViewMatOffset, sizeof(glm::mat4), glm::value_ptr(viewMat));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // set texture uniforms
  glBindVertexArray(cubeVertexAtt.arrayObject);

  // rotate with time
  glm::mat4 cubeModelMatrix = glm::mat4(1.0f);
  cubeModelMatrix = glm::rotate(cubeModelMatrix, t * glm::radians(cubeRotationAngle), glm::vec3(1.0f, 0.3f, 0.5f));

  cubeShader->use();
  cubeShader->setUniform("diffTexture", outlineTextureIndex);
  cubeShader->setUniform("model", cubeModelMatrix);

  glDrawElements(GL_TRIANGLES,
                 cubePosNormTexNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
                 GL_UNSIGNED_INT,
                 0);

  // draw cube
  cubeShader->setUniform("diffTexture", colorAttachmentTextureIndex);
  glDrawElements(GL_TRIANGLES,
                 cubePosNormTexNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
                 GL_UNSIGNED_INT,
                 0);

  // bind our frame buffer as the draw buffer (the frame buffer we drew too will still be under the read buffer)
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, infiniteCubeTextureFramebuffer.id);
  uint32 xOffset = (windowExtent.width - infiniteCubeTextureFramebuffer.extent.width) / 2;
  uint32 yOffset = (windowExtent.height - infiniteCubeTextureFramebuffer.extent.height) / 2;
  glBlitFramebuffer(xOffset, yOffset, infiniteCubeTextureFramebuffer.extent.width + xOffset, infiniteCubeTextureFramebuffer.extent.height + yOffset, 0, 0, infiniteCubeTextureFramebuffer.extent.width, infiniteCubeTextureFramebuffer.extent.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

  return drawFramebuffer;
}

void InfiniteCubeScene::framebufferSizeChangeRequest(Extent2D windowExtent)
{
  Scene::framebufferSizeChangeRequest(windowExtent);

  uint32 framebufferDimen = windowExtent.width < windowExtent.height ? windowExtent.width : windowExtent.height;
  Framebuffer* framebuffers[] = { &infiniteCubeTextureFramebuffer, &drawFramebuffer };
  deleteFramebuffers(ArrayCount(framebuffers), framebuffers);
  infiniteCubeTextureFramebuffer = initializeFramebuffer(Extent2D{ framebufferDimen, framebufferDimen });
  drawFramebuffer = initializeFramebuffer(windowExtent);
}
