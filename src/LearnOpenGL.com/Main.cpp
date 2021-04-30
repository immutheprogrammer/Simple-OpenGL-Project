#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include <stb_image.h>

#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>

#include "Shaders/Shader.h"


bool isWireFrame = false;

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;

void framebufferCallBack(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        isWireFrame = !isWireFrame;
    }
}

int main(void)
{
    GLFWwindow* window;
    /* Initialize the library */

    if (!glfwInit())
    {
        std::cout << "GLFW did not init!" << std::endl;
        return EXIT_FAILURE;
    }
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return EXIT_FAILURE;
    }
    std::cerr << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;


    /*
     We have to tell OpenGL the size of the rendering window so OpenGL knows how we want to display the data and coordinates with respect to the window. We can set those dimensions via the glViewport function
    */
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebufferCallBack);
    glfwSetKeyCallback(window, keyCallBack);

    Shader shader("Shaders/glsl/vert.glsl", "Shaders/glsl/frag.glsl");

    float vertices[] = {
       // positions         // tex coords
       0.5f,  0.5f, 0.0f,   1.0f, 1.0f,               // top right
       0.5f, -0.5f, 0.0f,   1.0f, 0.0f,               // bottom right
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,               // bottom left
      -0.5f,  0.5f, 0.0f,   0.0f, 1.0f                // top left 
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3

    };

    unsigned int VBO, VAO, EBO, texture;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    int width, height, nrChannels;
    unsigned char* data = stbi_load("Images/Crate.png", &width, &height, &nrChannels, 0);
    GLFWimage windowIcon[1];
    windowIcon[0].pixels = stbi_load("Images/Crate.png", &windowIcon[0].width, &windowIcon[0].height, &nrChannels, 0);
    if (data)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else 
    {
        std::cout << "Failed to load the texture" << std::endl;
    }

    glfwSetWindowIcon(window, 1, windowIcon);
    stbi_image_free(windowIcon[0].pixels);
    stbi_image_free(data);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glBindTexture(GL_TEXTURE_2D, texture);

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::scale(trans, glm::vec3(glfwGetTime() / 10, glfwGetTime() / 10, glfwGetTime() / 10));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 1.0f));

        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(trans));

        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        if (isWireFrame == true)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else if (isWireFrame == false)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }


    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader.ID);

    glfwTerminate();
    return 0;
}