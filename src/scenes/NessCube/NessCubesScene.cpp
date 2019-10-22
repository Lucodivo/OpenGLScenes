#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>

#include "../../Model.h"
#include "../../common/FileLocations.h"
#include "../../common/Util.h"

#include "NessCubesScene.h"

// ===== cube values =====
const glm::vec3 cubePositions[] = {
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
};

const float32 cubeScales[] = {
        0.6f,
        0.65f,
        0.7f,
        0.75f,
        0.8f,
        0.85f,
        0.9f,
        0.95f,
        1.0f
};
// ===== cube values =====

NessCubesScene::NessCubesScene(GLFWwindow* window, uint32 initScreenHeight, uint32 initScreenWidth)
        : FirstPersonScene(window, initScreenHeight, initScreenWidth),
          cubeShader(posNormTexVertexShaderFileLoc, cubeFragmentShaderFileLoc),
          lightShader(posVertexShaderFileLoc, singleColorFragmentShaderFileLoc),
          modelShader(posNormTexVertexShaderFileLoc, dirPosSpotLightModelFragmentShaderFileLoc),
          stencilShader(posNormTexVertexShaderFileLoc, singleColorFragmentShaderFileLoc),
          frameBufferShader(frameBufferVertexShaderFileLoc, kernel5x5TextureFragmentShaderFileLoc),
          skyboxShader(skyboxVertexShaderFileLoc, skyboxFragmentShaderFileLoc) {}

void NessCubesScene::runScene()
{
  uint32 lightVAO, lightVBO, lightEBO;
  initializeCubePositionVertexAttBuffers(lightVAO, lightVBO, lightEBO);

  uint32 cubeVAO, cubeVBO, cubeEBO;
  initializeCubePosTexNormVertexAttBuffers(cubeVAO, cubeVBO, cubeEBO);

  uint32 quadVAO, quadVBO, quadEBO;
  initializeFrameBufferQuadVertexAttBuffers(quadVAO, quadVBO, quadEBO);

  uint32 skyboxVAO, skyboxVBO, skyboxEBO;
  initializeCubePositionVertexAttBuffers(skyboxVAO, skyboxVBO, skyboxEBO);

  renderLoop(window, cubeVAO, lightVAO, quadVAO, skyboxVAO);

  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &cubeEBO);

  glDeleteVertexArrays(1, &lightVAO);
  glDeleteBuffers(1, &lightVBO);
  glDeleteBuffers(1, &lightEBO);

  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);
  glDeleteBuffers(1, &quadEBO);

  glDeleteVertexArrays(1, &skyboxVAO);
  glDeleteBuffers(1, &skyboxVBO);
  glDeleteBuffers(1, &skyboxEBO);
}

void NessCubesScene::renderLoop(GLFWwindow* window, uint32& shapesVAO, uint32& lightVAO, uint32& quadVAO, uint32& skyboxVAO)
{
  uint32 diffTextureId;
  uint32 specTextureId;
  uint32 skyboxTextureId;
  initializeTextures(diffTextureId, specTextureId, skyboxTextureId);
  initializeFrameBuffer(frameBuffer, rbo, frameBufferTexture, viewportWidth, viewportHeight);

  // load models
  Model nanoSuitModel((char*)nanoSuitModelLoc);

  const glm::mat4 projectionMat = glm::perspective(glm::radians(camera.Zoom), (float32)viewportWidth / (float32)viewportHeight, 0.1f, 100.0f);

  const float32 lightOrbitSpeed = 20.0f;
  const glm::vec3 lightAxisRot(0.0f, 1.0f, 0.0f);
  const float32 lightScale = 0.2f;

  const float32 cubRotAngle = 7.3f;

  glm::vec3 directionalLightDir = glm::vec3(1.0f, -0.5f, 1.0f);
  glm::vec3 directionalLightColor = glm::vec3(1.0f);

  // background clear color
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if 0
  // draw in wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

  auto setConstantLightUniforms = [&](Shader shader)
  {
    // positional light constants
    shader.setUniform("positionalLight.attenuation.constant", 1.0f);
    shader.setUniform("positionalLight.attenuation.linear", 0.09f);
    shader.setUniform("positionalLight.attenuation.quadratic", 0.032f);

    // flashlight constants
    shader.setUniform("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader.setUniform("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    shader.setUniform("spotLight.attenuation.constant", 1.0f);
    shader.setUniform("spotLight.attenuation.linear", 0.09f);
    shader.setUniform("spotLight.attenuation.quadratic", 0.032f);
  };

  uint32 globalFSUniformBuffer;
  uint32 globalFSBufferBindIndex = 0;
  {
    glGenBuffers(1, &globalFSUniformBuffer);

    glBindBuffer(GL_UNIFORM_BUFFER, globalFSUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);

    glBindBufferRange(GL_UNIFORM_BUFFER,    // target
                      globalFSBufferBindIndex,  // index of binding point
                      globalFSUniformBuffer,  // buffer id
                      0,            // starting offset into buffer object
                      4 * 16);          // size: 4 vec3's, 16 bits alignments

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(directionalLightDir));
    glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec3), glm::value_ptr(directionalLightColor * glm::vec3(0.3f)));
    glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(glm::vec3), glm::value_ptr(directionalLightColor * glm::vec3(0.5f)));
    glBufferSubData(GL_UNIFORM_BUFFER, 48, sizeof(glm::vec3), glm::value_ptr(directionalLightColor * glm::vec3(0.6f)));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    cubeShader.bindBlockIndex("globalBlockFS", globalFSBufferBindIndex);
    modelShader.bindBlockIndex("globalBlockFS", globalFSBufferBindIndex);
  }

  uint32 globalVSUniformBuffer;
  uint32 globalVSBufferBindIndex = 1;
  uint32 globalVSBufferViewMatOffset = sizeof(glm::mat4);
  {
    glGenBuffers(1, &globalVSUniformBuffer);

    glBindBuffer(GL_UNIFORM_BUFFER, globalVSUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

    glBindBufferRange(GL_UNIFORM_BUFFER,    // target
                      globalVSBufferBindIndex,  // index of binding point
                      globalVSUniformBuffer,  // buffer id
                      0,            // starting offset into buffer object
                      4 * 16);        // size: 4 vec3's, 16 bits alignments

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projectionMat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    cubeShader.bindBlockIndex("globalBlockVS", globalVSBufferBindIndex);
    modelShader.bindBlockIndex("globalBlockVS", globalVSBufferBindIndex);
    lightShader.bindBlockIndex("globalBlockVS", globalVSBufferBindIndex);
    stencilShader.bindBlockIndex("globalBlockVS", globalVSBufferBindIndex);
  }

  cubeShader.use();
  setConstantLightUniforms(cubeShader);
  cubeShader.setUniform("material.shininess", 32.0f);

  modelShader.use();
  setConstantLightUniforms(modelShader);

  skyboxShader.use();
  skyboxShader.setUniform("projection", projectionMat);

  frameBufferShader.use();
  frameBufferShader.setUniform("textureWidth", (float32)viewportWidth);
  frameBufferShader.setUniform("textureHeight", (float32)viewportHeight);

  const char* vendor = (char*)glGetString(GL_VENDOR); // Returns the vendor
  const char* renderer = (char*)glGetString(GL_RENDERER); // Returns a hint to the model

  camera.Position += glm::vec3(0.0f, 0.0f, 6.0f);

  // NOTE: render/game loop
  while (glfwWindowShouldClose(window) == GL_FALSE)
  {

    // check for input
    processKeyboardInput(window, this);
    processXInput(this);

    // bind our frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, // when stencil fails
                GL_KEEP, // when stencil passes but depth fails
                GL_REPLACE); // when stencil passes and depth passes
    glStencilFunc(GL_ALWAYS, // stencil function
                  1, // reference value for stencil test
                  0xFF); // mask that is ANDed with stencil value and reference value before the test compares them
    glStencilMask(0xFF); // mask that is ANDed with the stencil value that is about to be written to stencil buffer

#if 1
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);           // OpenGL state-using function
#else
    // FUN MODE - WINDOWS XP
    glClear(GL_DEPTH_BUFFER_BIT);
#endif

    // if flashlight is off, simply remove all color from light
    glm::vec3 flashLightColor = flashLightOn ? glm::vec3(0.93f, 0.84f, 0.72f) : glm::vec3(0.0f);

    float32 t = (float32)glfwGetTime();
    deltaTime = t - lastFrame;
    lastFrame = t;
    float32 sineVal = sinf(t);
    float32 lightR = (sinf((t + 30.0f) / 3.0f) / 2.0f) + 0.5f;
    float32 lightG = (sinf((t + 60.0f) / 8.0f) / 2.0f) + 0.5f;
    float32 lightB = (sinf(t / 17.0f) / 2.0f) + 0.5f;
    glm::vec3 positionalLightColor(lightR, lightG, lightB);

    glm::mat4 viewMat = camera.GetViewMatrix(deltaTime);

    glBindBuffer(GL_UNIFORM_BUFFER, globalVSUniformBuffer);
    // update global view matrix uniform
    glBufferSubData(GL_UNIFORM_BUFFER, globalVSBufferViewMatOffset, sizeof(glm::mat4), glm::value_ptr(viewMat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // oscillate with time
    const glm::vec3 lightPosition = glm::vec3(2.0f, 0.0f + sineVal, 2.0f);
    // orbit with time
    glm::mat4 lightModel; // default constructor is identity matrix
    lightModel = glm::rotate(lightModel, t * glm::radians(lightOrbitSpeed), lightAxisRot);
    lightModel = glm::translate(lightModel, lightPosition);
    lightModel = glm::scale(lightModel, glm::vec3(lightScale));

    // draw positional light
    lightShader.use();
    glBindVertexArray(lightVAO);

    lightShader.setUniform("model", lightModel);
    lightShader.setUniform("color", positionalLightColor);
    glDrawElements(GL_TRIANGLES, // drawing mode
                   cubePosTexNormNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
                   GL_UNSIGNED_INT, // type of the indices
                   0); // offset in the EBO
    glBindVertexArray(0);

    // User fragment shaders to draw a triangle
    glm::vec4 worldLightPos = lightModel * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    // rotate with time
    glm::mat4 cubeModel = glm::rotate(glm::mat4(), t * glm::radians(cubRotAngle), glm::vec3(1.0f, 0.3f, 0.5f));
    // switch between two images over time
    bool animSwitch = sin(8 * t) > 0;
    // emission strength fluctuating over time
    float32 emissionStrength = ((sin(t * 2) + 1.0f) / 4) + 0.15f;

    auto setDynamicLightUniforms = [&](Shader shader)
    {
      // positional light (orbiting light)
      shader.setUniform("positionalLight.position", worldLightPos.x, worldLightPos.y, worldLightPos.z);
      shader.setUniform("positionalLight.color.ambient", positionalLightColor * glm::vec3(0.05f));
      shader.setUniform("positionalLight.color.diffuse", positionalLightColor * glm::vec3(0.5f));
      shader.setUniform("positionalLight.color.specular", positionalLightColor * glm::vec3(1.0f));

      // flash light
      shader.setUniform("spotLight.position", camera.Position);
      shader.setUniform("spotLight.direction", camera.Front);
      shader.setUniform("spotLight.color.ambient", flashLightColor * glm::vec3(0.05f));
      shader.setUniform("spotLight.color.diffuse", flashLightColor * glm::vec3(0.3f));
      shader.setUniform("spotLight.color.specular", flashLightColor * glm::vec3(0.5f));
    };

    // draw skybox
    skyboxShader.use();

    glBindVertexArray(skyboxVAO);

    glEnable(GL_DEPTH_TEST);
    //glStencilMask(0xFF); // mask that is ANDed with the stencil value that is about to be written to stencil buffer
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureId);
    skyboxShader.setUniform("skybox", 0);

    glm::mat4 viewMinusTranslation = glm::mat4(glm::mat3(viewMat));
    skyboxShader.setUniform("view", viewMinusTranslation);

    glDrawElements(GL_TRIANGLES, // drawing mode
                   36, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
                   GL_UNSIGNED_INT, // type of the indices
                   0); // offset in the EBO

    // draw cubes
    cubeShader.use();

    // bind shapesVAO
    glBindVertexArray(shapesVAO);

    setDynamicLightUniforms(cubeShader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffTextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specTextureId);
    cubeShader.setUniform("material.diffTexture1", 0);
    cubeShader.setUniform("material.specTexture1", 1);

    cubeShader.setUniform("animSwitch", animSwitch);

    cubeShader.setUniform("viewPos", camera.Position);
    cubeShader.setUniform("view", viewMat);

    for (uint32 i = 0; i < ArrayCount(cubePositions); i++)
    {
      glm::mat4 model;
      float32 angularSpeed = 7.3f * (i + 1);

      // orbit around the specified axis from the translated distance
      model = glm::rotate(model, t * glm::radians(angularSpeed), glm::vec3(50.0f - (i * 10), 100.0f, -50.0f + (i * 10)));
      // translate to position in world
      model = glm::translate(model, cubePositions[i]);
      // rotate with time
      model = glm::rotate(model, t * glm::radians(angularSpeed), glm::vec3(1.0f, 0.3f, 0.5f));
      // scale object
      model = glm::scale(model, glm::vec3(cubeScales[i]));
      cubeShader.setUniform("model", model);
      glDrawElements(GL_TRIANGLES, // drawing mode
                     cubePosTexNormNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
                     GL_UNSIGNED_INT, // type of the indices
                     0); // offset in the EBO
    }

    // draw cube stencil outlines
    for (uint32 i = 0; i < ArrayCount(cubePositions); i++)
    {
      float32 angularSpeed = 7.3f * (i + 1);

      glm::mat4 model;
      // orbit around the specified axis from the translated distance
      model = glm::rotate(model, t * glm::radians(angularSpeed), glm::vec3(50.0f - (i * 10), 100.0f, -50.0f + (i * 10)));
      // translate to position in world
      model = glm::translate(model, cubePositions[i]);
      // rotate with time
      model = glm::rotate(model, t * glm::radians(angularSpeed), glm::vec3(1.0f, 0.3f, 0.5f));
      // scale object
      model = glm::scale(model, glm::vec3(cubeScales[i] + 0.05f));
      stencilShader.use();
      stencilShader.setUniform("color", glm::vec3(1.0f, 1.0f, 1.0f));
      stencilShader.setUniform("view", viewMat);
      stencilShader.setUniform("model", model);
      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
      glStencilMask(0x00);
      glDisable(GL_DEPTH_TEST);
      glDrawElements(GL_TRIANGLES, // drawing mode
                     cubePosTexNormNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
                     GL_UNSIGNED_INT, // type of the indices
                     0); // offset in the EBO
      glEnable(GL_DEPTH_TEST);
    }
    glBindVertexArray(0);

    glStencilFunc(GL_ALWAYS, // stencil function
                  1, // reference value for stencil test
                  0xFF); // mask that is ANDed with stencil value and reference value before the test compares them
    glStencilMask(0xFF); // mask that is ANDed with the stencil value that is about to be written to stencil buffer
    glClear(GL_STENCIL_BUFFER_BIT); // NOTE: glClear(GL_STENCIL_BUFFER_BIT) counts as writing to the stencil buffer and will be directly ANDed with the stencil mask

    // draw models
    float32 modelScale = 0.2f;
    {
      // Drawing the model
      modelShader.use();
      setDynamicLightUniforms(modelShader);

      modelShader.setUniform("material.shininess", 32.0f);

      modelShader.setUniform("viewPos", camera.Position);
      modelShader.setUniform("view", viewMat);

      glm::mat4 model;
      model = glm::scale(model, glm::vec3(modelScale));  // it's a bit too big for our scene, so scale it down
      model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
      modelShader.setUniform("model", model);
      nanoSuitModel.Draw(modelShader);

      //  Wall Hack Stencil For Model
      stencilShader.use();
      stencilShader.setUniform("color", glm::vec3(0.5f, 0.0f, 0.0f));
      stencilShader.setUniform("view", viewMat);
      stencilShader.setUniform("model", model);
      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
      glStencilMask(0x00);
      glDisable(GL_DEPTH_TEST);
      nanoSuitModel.Draw(stencilShader);
    }

    // bind default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    frameBufferShader.use();
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
    frameBufferShader.setUniform("tex", 2);
    frameBufferShader.setUniform("kernel", kernels5x5[selectedKernelIndex], ArrayCount(kernels5x5[selectedKernelIndex]));
    glDisable(GL_DEPTH_TEST);
    glDrawElements(GL_TRIANGLES, // drawing mode
                   6, // number of elements to draw (3 vertices per triangle * 2 triangles per quad)
                   GL_UNSIGNED_INT, // type of the indices
                   0); // offset in the EBO

    glDisable(GL_DEPTH_TEST);
    uint32 numFrames = (uint32)(1 / deltaTime);
    renderText(std::to_string(numFrames) + " FPS", 25.0f, 25.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    renderText(vendor, 25.0f, 100.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    renderText(renderer, 25.0f, 150.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    glEnable(GL_DEPTH_TEST);

    glfwSwapBuffers(window); // swaps double buffers (call after all render commands are completed)
    glfwPollEvents(); // checks for events (ex: keyboard/mouse input)
  }
}

void NessCubesScene::initializeTextures(uint32& diffTextureId, uint32& specTextureId, uint32& skyboxTextureId)
{
  load2DTexture(diffuseTextureLoc, diffTextureId, true);
  load2DTexture(specularTextureLoc, specTextureId, true);
  loadCubeMapTexture(skyboxWaterFaceLocations, skyboxTextureId);
}

void NessCubesScene::frameBufferSize(uint32 width, uint32 height)
{
  FirstPersonScene::frameBufferSize(width, height);
  initializeFrameBuffer(frameBuffer, rbo, frameBufferTexture, width, height);
  frameBufferShader.setUniform("textureWidth", (float32)width);
  frameBufferShader.setUniform("textureHeight", (float32)height);
}

void NessCubesScene::key_Up()
{
  nextImageKernel();
}

void NessCubesScene::key_Down()
{
  prevImageKernel();
}

void NessCubesScene::key_LeftMouseButton_pressed()
{
  toggleFlashlight();
}

void NessCubesScene::button_X_pressed()
{
  key_LeftMouseButton_pressed();
}

void NessCubesScene::button_dPadUp_pressed()
{
  nextImageKernel();
}

void NessCubesScene::button_dPadDown_pressed()
{
  prevImageKernel();
}

void NessCubesScene::toggleFlashlight()
{
  flashLightOn = !flashLightOn;
}

void NessCubesScene::nextImageKernel()
{
  double currentTime = glfwGetTime();
  if (currentTime - kernelModeSwitchTimer > 0.5f)
  {
    selectedKernelIndex = (selectedKernelIndex + 1) % kernelCount;
    kernelModeSwitchTimer = currentTime;
  }
}

void NessCubesScene::prevImageKernel()
{
  double currentTime = glfwGetTime();
  if (currentTime - kernelModeSwitchTimer > 0.5f)
  {
    selectedKernelIndex = selectedKernelIndex != 0 ? ((selectedKernelIndex - 1) % kernelCount) : (kernelCount - 1);
    kernelModeSwitchTimer = currentTime;
  }
}