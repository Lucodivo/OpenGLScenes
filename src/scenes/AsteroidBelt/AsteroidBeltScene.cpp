#include "AsteroidBeltScene.h"
#include "../../common/FileLocations.h"
#include "../../Model.h"
#include "../../common/Util.h"
#include "../../common/ObjectData.h"

const uint32 skyboxTextureIndex = 0;
const uint32 skybox2TextureIndex = skyboxTextureIndex + 1;

AsteroidBeltScene::AsteroidBeltScene() : FirstPersonScene()
{
  camera.Position = glm::vec3(0.0f, 0.0f, 50.0f);
}

const char* AsteroidBeltScene::title()
{
  return "Planet & Asteroid";
}

void AsteroidBeltScene::init(Extent2D windowExtent)
{
  FirstPersonScene::init(windowExtent);

  skyboxVertexAtt = initializeCubePositionVertexAttBuffers();

  drawFramebuffer = initializeFramebuffer(windowExtent);

  modelShader = new ShaderProgram(posNormalVertexShaderFileLoc, skyboxReflectionFragmentShaderFileLoc);
  reflectModelInstanceShader = new ShaderProgram(AsteroidVertexShaderFileLoc, skyboxReflectionFragmentShaderFileLoc);
  skyboxShader = new ShaderProgram(skyboxVertexShaderFileLoc, skyboxFragmentShaderFileLoc);

  loadCubeMapTexture(skyboxInterstellarFaceLocations, skyboxTextureId);
  loadCubeMapTexture(skyboxSpaceLightBlueFaceLocations, skybox2TextureId);

  planetModel = new Model(planetModelLoc);
  asteroidModel = new Model(asteroidModelLoc);

  const float32 aspectRatio = (float32)windowExtent.width / (float32)windowExtent.height;
  const glm::mat4 projectionMat = glm::perspective(glm::radians(camera.Zoom), aspectRatio, 0.1f, 100.0f);

  modelShader->use();
  modelShader->setUniform("projection", projectionMat);
  modelShader->setUniform("skybox", 1);

  skyboxShader->use();
  skyboxShader->setUniform("projection", projectionMat);
  skyboxShader->setUniform("skybox", 0);

  reflectModelInstanceShader->use();
  reflectModelInstanceShader->setUniform("projection", projectionMat);
  reflectModelInstanceShader->setUniform("skybox", 0);

  glm::mat4* asteroidModelMatrices = new glm::mat4[numAsteroids];
  srand((uint32)getTime()); // initialize random seed
  float32 radius = 30.0;
  auto randDisplacement = []() -> float32 { return ((rand() % 2000) / 100.0f) - 10.0f; };
  glm::mat4 identityMat = glm::mat4(1.0);
  for (uint32 i = 0; i < numAsteroids; i++)
  {
    // Displace along circle with 'radius' in range [-offset, offset]
    float32 angle = (float)i / (float)numAsteroids * 360.0f;
    float32 displacement = randDisplacement();
    float32 x = sin(angle) * radius + displacement;
    displacement = randDisplacement();
    float32 y = displacement * 0.2f; // keep height of field smaller compared to width of x and z
    displacement = randDisplacement();
    float32 z = cos(angle) * radius + displacement;
    asteroidModelMatrices[i] = glm::translate(identityMat, glm::vec3(x, y, z));

    // scale
    float32 scale = ((rand() % 200) / 1000.0f) + 0.05f;
    asteroidModelMatrices[i] = glm::scale(asteroidModelMatrices[i], glm::vec3(scale));

    // rotate
    float32 rotAngle = (float32)(rand() % 360);
    asteroidModelMatrices[i] = glm::rotate(asteroidModelMatrices[i], rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
  }

  // vertex buffer object to store all model matrices for the asteroids
  glGenBuffers(1, &asteroidModelMatrixBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, asteroidModelMatrixBuffer);
  glBufferData(GL_ARRAY_BUFFER, numAsteroids * sizeof(glm::mat4), asteroidModelMatrices, GL_STATIC_DRAW);

  for (uint32 i = 0; i < asteroidModel->meshes.size(); i++)
  {
    glBindVertexArray(asteroidModel->meshes[i]->VAO);

    // A single vec4 is the largest attribute pointer available with a single call
    // So we must individually assign the 4 vec4 attribute pointers to receiver a mat4x4 in the shader
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));

    // For glVertexAttribDivisor, the default value is 0 which means that the attribute is updated once per
    // iteration of the vertex shader. Setting it to 1 means that the attribute is updated once per instance.
    glVertexAttribDivisor(0, 0); // vec3 position
    glVertexAttribDivisor(1, 0); // vec3 normal
    glVertexAttribDivisor(2, 0); // vec2 texture coordinate
    glVertexAttribDivisor(3, 1); //  \.
    glVertexAttribDivisor(4, 1); //   \ mat4
    glVertexAttribDivisor(5, 1); //   / model matrix
    glVertexAttribDivisor(6, 1); //  /'

    glBindVertexArray(0);
  }

  delete[] asteroidModelMatrices;
}

void AsteroidBeltScene::deinit()
{
  FirstPersonScene::deinit();

  deleteVertexAtt(skyboxVertexAtt);

  deleteFramebuffer(&drawFramebuffer);

  modelShader->deleteShaderResources();
  reflectModelInstanceShader->deleteShaderResources();
  skyboxShader->deleteShaderResources();
  delete modelShader;
  delete reflectModelInstanceShader;
  delete skyboxShader;

  uint32 deleteTextures[] = { skyboxTextureId, skybox2TextureId };
  glDeleteTextures(ArrayCount(deleteTextures), deleteTextures);

  delete planetModel;
  delete asteroidModel;

  glDeleteBuffers(1, &asteroidModelMatrixBuffer);
}

Framebuffer AsteroidBeltScene::drawFrame()
{

  glActiveTexture(GL_TEXTURE0 + skyboxTextureIndex);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureId);
  glActiveTexture(GL_TEXTURE0 + skybox2TextureIndex);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox2TextureId);

  // background clear color
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDisable(GL_CULL_FACE);
  glViewport(0, 0, windowExtent.width, windowExtent.height);

  glBindFramebuffer(GL_FRAMEBUFFER, drawFramebuffer.id);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);           // OpenGL state-using function

  float32 t = getTime();
  deltaTime = t - lastFrame;
  lastFrame = t;

  glm::mat4 viewMat = camera.UpdateViewMatrix(deltaTime, cameraMovementSpeed);

  modelShader->use();
  modelShader->setUniform("view", viewMat);
  modelShader->setUniform("cameraPos", camera.Position);

  // draw Planet
  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
  model = glm::rotate(model, t * glm::radians(planetRotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
  modelShader->setUniform("model", model);
  planetModel->Draw(*modelShader);

  // draw meteorites
  reflectModelInstanceShader->use();
  reflectModelInstanceShader->setUniform("view", viewMat);
  reflectModelInstanceShader->setUniform("cameraPos", camera.Position);
  reflectModelInstanceShader->setUniform("orbit", glm::rotate(glm::mat4(1.0f), t * glm::radians(-planetRotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f)));
  for (uint32 i = 0; i < asteroidModel->meshes.size(); i++)
  {
    glBindVertexArray(asteroidModel->meshes[i]->VAO);
    glDrawElementsInstanced(GL_TRIANGLES, asteroidModel->meshes[i]->indicesCount, GL_UNSIGNED_INT, 0, numAsteroids);
  }

  // draw skybox
  glBindVertexArray(skyboxVertexAtt.arrayObject);
  skyboxShader->use();
  glm::mat4 viewMinusTranslation = glm::mat4(glm::mat3(viewMat));
  skyboxShader->setUniform("view", viewMinusTranslation);
  glDrawElements(GL_TRIANGLES, // drawing mode
                 36, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
                 GL_UNSIGNED_INT, // type of the indices
                 0); // offset in the EBO

  return drawFramebuffer;
}

void AsteroidBeltScene::framebufferSizeChangeRequest(Extent2D windowExtent)
{
  Scene::framebufferSizeChangeRequest(windowExtent);

  deleteFramebuffer(&drawFramebuffer);
  drawFramebuffer = initializeFramebuffer(windowExtent);
}
