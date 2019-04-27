#pragma once

void loadGLFW();
GLFWwindow * createWindow();
void initializeGLAD();
void initializeObjectBuffers(unsigned int & VAO, unsigned int & VBO, unsigned int & EBO);
void initializeLightBuffers(unsigned int & VAO, unsigned int & VBO, const unsigned int & EBO);
void renderLoop(GLFWwindow *window, unsigned int &shapesVAO, unsigned int &lightVAO);
void initializeTextures(Shader & shader);
void loadTexture(const char * imgLocation, unsigned int textureOffset);
void processInput(GLFWwindow * window);
void mouse_callback(GLFWwindow * window, double xpos, double ypos);
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow * window, int width, int height);

// ===== cube values =====
#define BottomLeftTexture 0.0f, 0.0f
#define BottomRightTexture 1.0f, 0.0f
#define TopLeftTexture 0.0f, 1.0f
#define TopRightTexture 1.0f, 1.0f
const unsigned int cubeVertexAttSizeInBytes = 8 * sizeof(float);
const unsigned int cubeNumElements = 12;
const unsigned int numCubes = 1; 
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

const unsigned int cubeIndices[]{
    0, 1, 2,
    2, 3, 0,
    4, 5, 6,
    6, 7, 4,
    8, 9, 10,
    10, 11, 8,
    12, 13, 14,
    14, 15, 12,
    16, 17, 18,
    18, 19, 16,
    20, 21, 22,
    22, 23, 20
};

const glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};
// ===== cube values =====

// ===== double triangle values =====
const unsigned int triangleVertexAttSize = 8;
const unsigned int triangleNumElements = 2;
const float triangleVertices[] = {
    // First triangle
    // positions            // colors           // texture coords
    -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // bottom left
    0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 1.0f,   1.0f, 0.0f, // bottom right
    0.0f, 0.5f, 0.0f,       0.0f, 1.0f, 0.0f,   0.5, 1.0f,  // top (for first triangle) / bottom (for second triangle)
    // Second triangle
    -0.25f, 0.75f, 0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // top left
    0.25f, 0.75f, 0.0f,     1.0f, 0.0f, 0.0f,   1.0f, 0.0f  // top right
};
const unsigned int indices[]{
    0, 1, 2,    // first triangle
    2, 3, 4     // second triangle
};
// ===== double triangle values =====