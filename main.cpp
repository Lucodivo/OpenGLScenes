#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "main.h"

#define VIEWPORT_WIDTH 800
#define VIEWPORT_HEIGHT 600

// shader information
const char *vertexShaderFile = "VertexShader.glsl";
const char *fragmentShaderFile = "FragmentShader.glsl";

// texture 
const char *textureImgLoc1 = "Data/kanye_triangle1.jpg";
const char *textureImgLoc2 = "Data/kanye_triangle2.jpg";

// camera positions
float cameraZValue = 3.0f;
float cameraXValue = 0.0f;
float cameraYValue = 0.0f;

int main() {
    loadGLFW();
    GLFWwindow* window = createWindow();
    glfwMakeContextCurrent(window);

    initializeGLAD();

    glViewport(0, // int x: left-x of viewport rect
               0, // int y: bottom-y of viewport rect
               VIEWPORT_WIDTH, // int width
               VIEWPORT_HEIGHT); // int height

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    unsigned int VAO, VBO, EBO;
    initializeBuffers(VAO, VBO, EBO);
    renderLoop(window, VAO);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate(); // clean up gl resources
    return 0;
}

void loadGLFW() {
    int loadSucceeded = glfwInit();
    if (loadSucceeded == GLFW_FALSE) {
        std::cout << "Failed to load GLFW" << std::endl;
        exit(-1);
    }
}

GLFWwindow* createWindow() {
    // set what kind of window desired
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL version x._
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL version _.x
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment for OSX

    // create window
    GLFWwindow* window = glfwCreateWindow(VIEWPORT_WIDTH, // int Width
                                        VIEWPORT_HEIGHT, // int Height
                                        "LearnOpenGL", // const char* Title
                                        NULL, // GLFWmonitor* Monitor: Specified for which monitor for fullscreen, NULL for windowed mode
                                        NULL); // GLFWwindow* Share: window to share resources with
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    return window;
}

void initializeGLAD() {
    // intialize GLAD to help manage function pointers for OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}

void initializeBuffers(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,     // Num objects to generate 
        &VBO);  // Out parameters to store IDs of gen objects
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind object to array buffer
    glBufferData(GL_ARRAY_BUFFER, // which buffer data is being entered in
        sizeof(cubeVertices), // size of data being placed in array buffer
        cubeVertices,        // data to store in array buffer       
        GL_STATIC_DRAW); // GL_STATIC_DRAW (most likely not change), GL_DYNAMIC_DRAW (likely to change), GL_STREAM_DRAW (changes every time drawn)

    // position attribute
    glVertexAttribPointer(0, // position vertex attribute (used for location = 0 of Vertex Shader false) 
        3, // size of vertex attribute (we're using vec3)
        GL_FLOAT, // type of data being passed 
        GL_FALSE, // whether the data needs to be normalized
        cubeVertexAttSize * sizeof(float), // stride: space between consecutive vertex attribute sets
        (void*)(0 * sizeof(float))); // offset of where the data starts in the array
    glEnableVertexAttribArray(0);
    // color attribute
    /*glVertexAttribPointer(1,
        3,
        GL_FLOAT,
        GL_FALSE,
        vertexAttSize * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);*/
    // texture coords attribute
    glVertexAttribPointer(2,
        2,
        GL_FLOAT,
        GL_FALSE,
        cubeVertexAttSize * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /*glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

    // unbind VBO, VAO, & EBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // Must unbind EBO AFTER unbinding VAO, since VAO stores all glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _) calls
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void renderLoop(GLFWwindow *window, unsigned int &VAO) {
    Shader shader = Shader(vertexShaderFile, fragmentShaderFile);
    initializeTextures(shader);

    glEnable(GL_DEPTH_TEST);

    // model matrix
    //glm::mat4 model;
    //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // view matrix
    //glm::mat4 view;
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, cameraZValue));
    // projection matrix
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)VIEWPORT_WIDTH / (float)VIEWPORT_HEIGHT, 0.1f, 100.0f);

    //shader.setUniform("model", model);
    //shader.setUniform("view", view);
    shader.setUniform("projection", projection);

    // NOTE: render/game loop
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // check for input
        processInput(window);

        // == RENDERING COMMANDS ==
        // clear the background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);   // OpenGL state-setting function
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);           // OpenGL state-using function

        float t = static_cast<float>(glfwGetTime());
        /*float sineVal1 = (sin(t) / 2.0f) + 0.5f;
        float sineVal2 = (sin(t/2) / 2.0f) + 0.5f;*/
        float sineVal3 = (sin(t/3) / 2.0f) + 0.5f;
        shader.setUniform("sineVal", sineVal3);

        // User fragment shaders to draw a triangle
        shader.use();

        glm::mat4 view;
        view = glm::translate(view, glm::vec3(-cameraXValue, -cameraYValue, -cameraZValue));
        shader.setUniform("view", view);

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            float angle = 7.3f * (i + 1);
            model = glm::rotate(model, t * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setUniform("model", model);
            //glDrawElements(GL_TRIANGLES, // drawing mode
            //                triangleNumElements * 3, // number of elements to draw (6 vertices)
            //                GL_UNSIGNED_INT, // type of the indices
            //                0); // offset in the EBO
            glDrawArrays(GL_TRIANGLES, 0, cubeNumElements * 3);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window); // swaps double buffers (call after all render commands are completed)
        glfwPollEvents(); // checks for events (ex: keyboard/mouse input)
    }
}

void initializeTextures(Shader &shader) {
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, /*GL_CLAMP_TO_EDGE*/ GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, /*GL_CLAMP_TO_EDGE*/ GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    shader.use(); // must activate/use the shader before setting the uniforms!
    loadTexture(textureImgLoc1, 0);
    shader.setUniform("aTexture", 0);
    loadTexture(textureImgLoc2, 1);
    shader.setUniform("bTexture", 1);
}

void loadTexture(const char* imgLocation, unsigned int textureOffset) {
    unsigned int texture; // stores id of generated texture
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + textureOffset); // activate texture unit (by defauly it is bound to GL_TEXTURE0
    glBindTexture(GL_TEXTURE_2D, texture);

    // load image data
    int width, height, numChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(imgLocation, &width, &height, &numChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, // target
            0, // level of detail (level n is the nth mipmap reduction image)
            GL_RGB, // kind of format we want to store the texture
            width, // width of texture
            height, // height of texture
            0, // border (legacy stuff, MUST BE 0)
            GL_RGB, // Specifies format of the pixel data
            GL_UNSIGNED_BYTE, // specifies data type of pixel data
            data); // pointer to the image data
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data); // free texture image memory
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    float speedMult = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 3.0f : 1.0f;
    float movSpeed = speedMult * 0.07f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        cameraZValue -= movSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        cameraZValue += movSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cameraXValue -= movSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cameraXValue += movSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraYValue += movSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraYValue -= movSpeed;
    }
}

// Callback function for when user resizes our window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}