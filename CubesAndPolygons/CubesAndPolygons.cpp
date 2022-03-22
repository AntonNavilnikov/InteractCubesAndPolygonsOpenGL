#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scene.h"

static GLFWwindow* InitGL();
static GLuint LinkShaders();

static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

static void AddTestData();

const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"gl_Position = transform * vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"uniform vec4 col;\n"
"void main()\n"
"{\n"
"color = col;\n"
"}\n\0";

int main()
{
    GLFWwindow* window = InitGL();
    GLuint shaderProgram = LinkShaders();

    AddTestData();

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Scene::Instance().GetBufferAllocationSize(), Scene::Instance().GetBufferAsArray(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat4 transformMatrix;
    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    GLuint colorLoc = glGetUniformLocation(shaderProgram, "col");

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        size_t lastIndex = 0;
        std::vector<std::shared_ptr<Entity>>& entities = Scene::Instance().GetEntities();
        for (int i = 0; i < entities.size(); ++i) {
            glStencilFunc(GL_ALWAYS, i + 1, -1);

            std::shared_ptr<Entity> entity = entities[i];
            transformMatrix = entity->translation * entity->rotation;
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformMatrix));
            glUniform4fv(colorLoc, 1, entity->GetColor());

            GLsizei verticesCount = entity->GetTrianglesCount() * 3;
            glDrawArrays(GL_TRIANGLES, lastIndex, verticesCount);
            lastIndex += verticesCount;
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    
    return 0;
}

static GLFWwindow* InitGL()
{
    //GLFW
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Cubes and polygons", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    //Keys and mouse events
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetKeyCallback(window, KeyCallback);

    glewExperimental = GL_TRUE;

    //GLEW
    glewInit();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    return window;
}

static GLuint LinkShaders()
{
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation error\n" << infoLog << std::endl;
    }
    
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation error\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

static int GetObjectIndex(double xpos, double ypos)
{
    GLbyte color[4];
    GLfloat depth;
    GLuint index;

    glReadPixels(xpos, HEIGHT - ypos - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
    glReadPixels(xpos, HEIGHT - ypos - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    glReadPixels(xpos, HEIGHT - ypos - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

    return index;
}

static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    Scene::Instance().MouseMove(xpos, ypos, width, height);
}

static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            Scene::Instance().SetSelected(GetObjectIndex(xpos, ypos), xpos, ypos);
        }
        else if (action == GLFW_RELEASE) {
            Scene::Instance().SetSelected(0);
        }
    }
}

// Is called whenever a key is pressed/released via GLFW
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (key == GLFW_KEY_LEFT_CONTROL ||
        key == GLFW_KEY_RIGHT_CONTROL ||
        key == GLFW_KEY_LEFT_SHIFT ||
        key == GLFW_KEY_RIGHT_SHIFT) {
        Scene::Instance().SetRotationMode(action != GLFW_RELEASE);
    }
}

static void AddTestData()
{
    Scene::Instance().AddEntity(std::shared_ptr<Entity>(
        new Cube(glm::vec3(-0.5, 0.5, 0.0), 0.3, glm::vec3(1.0, 1.0, 1.0), glm::vec3(5.0, 0.0, 0.0)
    )));
    
    Scene::Instance().AddEntity(std::shared_ptr<Entity>(
        new Cube(glm::vec3(0.5, 0.0, 0.0), 0.2, glm::vec3(1.0, 0.5, -1.0), glm::vec3(0.0, 0.2, 0.0)
    )));
    
    std::vector<glm::vec2> vertices(4);
    vertices[0] = glm::vec2(0.0, -0.4);
    vertices[1] = glm::vec2(0.1, 0.2);
    vertices[2] = glm::vec2(-0.1, -0.2);
    vertices[3] = glm::vec2(-0.2, -0.2);
    Scene::Instance().AddEntity(std::shared_ptr<Entity>(new Polygon2D(vertices)));
    
    vertices.resize(5);
    vertices[0] = glm::vec2(0.1, -0.4);
    vertices[1] = glm::vec2(0.3, -0.3);
    vertices[2] = glm::vec2(0.5, -0.5);
    vertices[3] = glm::vec2(0.4, -0.1);
    vertices[4] = glm::vec2(0.1, -0.1);
    Scene::Instance().AddEntity(std::shared_ptr<Entity>(new Polygon2D(vertices)));
}