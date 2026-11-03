#define GLFW_DLL
#define GLEW_DLL
#include "glew-2.1.0/include/GL/glew.h"
#include "glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#include "shader.h"
#include "Model.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <vector>

glm::vec3 cameraPos = glm::vec3(10.0f, 3.0f, -1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool  firstMouse = true;
float yaw = 180.0f;
float pitch = 0.0f;
float lastX = 512.0f;
float lastY = 384.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float offsetY0 = 0.0f;
const float MIN_Y0 = -0.5f;

float angle1 = 0.0f;
const float MAX_ANGLE1 = 50.0f;
const float MIN_ANGLE1 = -50.0f;

float offsetZ2 = 0.0f;
const float MAX_Z2 = 0.6f;
const float MIN_Z2 = -0.3f;

void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    float step = deltaTime * 1.0f;   // скорость изменения параметров

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        float newY = offsetY0 - step;
        if (newY < MIN_Y0) newY = MIN_Y0;
        offsetY0 = newY;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        float newY = offsetY0 + step;
        if (newY > 0.0f) newY = 0.0f;
        offsetY0 = newY;
    }

    float rotStep = deltaTime * 30.0f;  // градусов в секунду
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        float newA = angle1 - rotStep;
        if (newA < MIN_ANGLE1) newA = MIN_ANGLE1;
        angle1 = newA;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        float newA = angle1 + rotStep;
        if (newA > MAX_ANGLE1) newA = MAX_ANGLE1;
        angle1 = newA;
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        float newZ = offsetZ2 - step;
        if (newZ < MIN_Z2) newZ = MIN_Z2;
        offsetZ2 = newZ;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        float newZ = offsetZ2 + step;
        if (newZ > MAX_Z2) newZ = MAX_Z2;
        offsetZ2 = newZ;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(1024, 768, "laba_Zvereva_Marina_ASUb-24-1", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewInit();
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("vertex.txt", "fragment.txt");
    Model  ourModel("Laba_Zvereva_Marina.obj");

    std::cout << "Model loaded. Meshes: " << ourModel.meshes.size() << std::endl;

    glm::vec3 lightPos(5.0f, 5.0f, 5.0f);
    const float globalScale = 1.5f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glfwPollEvents();

        glClearColor(1.0f, 0.4f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.Use();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        ourShader.setVec3("light.position", lightPos.x, lightPos.y, lightPos.z);
        ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        ourShader.setVec3("material.ambient", 0.7f, 0.1f, 0.6f);
        ourShader.setVec3("material.diffuse", 0.7f, 0.1f, 0.6f);
        ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        ourShader.setFloat("material.shininess", 32.0f);

        ourShader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

        glm::mat4 base = glm::scale(glm::mat4(1.0f), glm::vec3(globalScale));

        glm::mat4 mat3 = base;

        glm::mat4 mat0 = mat3;
        mat0 = glm::translate(mat0, glm::vec3(0.0f, offsetY0, 0.0f));

        const glm::vec3 pivot1 = glm::vec3(-0.5f, 2.66f, -0.87f);

        glm::mat4 mat1 = mat0;
        mat1 = glm::translate(mat1, pivot1);
        mat1 = glm::rotate(mat1, glm::radians(angle1), glm::vec3(0.0f, 1.0f, 0.0f));
        mat1 = glm::translate(mat1, -pivot1);

        glm::mat4 mat2 = mat1;
        mat2 = glm::translate(mat2, glm::vec3(0.0f, 0.0f, offsetZ2));

        std::vector<glm::mat4> transforms(ourModel.meshes.size(), base);
        if (ourModel.meshes.size() >= 4) {
            transforms[0] = mat0;
            transforms[1] = mat1;
            transforms[2] = mat2;
            transforms[3] = mat3;
        }
        else {
            for (unsigned int i = 0; i < (unsigned int)ourModel.meshes.size(); i++) {
                switch (i) {
                case 0: transforms[i] = mat0; break;
                case 1: transforms[i] = mat1; break;
                case 2: transforms[i] = mat2; break;
                default: transforms[i] = mat3; break;
                }
            }
        }

        ourModel.Draw(ourShader, transforms);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}