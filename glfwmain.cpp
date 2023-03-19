#include "game_logic.hpp"
#include "bot.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>

#include "camera.hpp"
#include "shader.hpp"

#include <iostream>
#include <array>
#include <vector>
#include <queue>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

camera cam;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct InstanceData
{
    glm::vec4 color;
    glm::mat4 transformation;
};

std::queue<glm::u32vec2> next_move;
float snake_speed = 0.1;

int main()
{
    glm::u32vec2 game_size{ 10, 10 };
    game_logic::game_logic game(game_size);
    game_logic::bot bot(game_size);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    {
        shader ourShader("D:/dev/snake-cpp/cube.vert", "D:/dev/snake-cpp/cube.frag");

        constexpr auto vertices = std::array{
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
        };

        constexpr auto indexes = std::array<uint32_t, 14>{ 2, 3, 6, 7, 5, 3, 1, 2, 0, 6, 4, 5, 0, 1 };

        const auto VAO = gl::genVertexArray();
        const auto VBO = gl::genBuffer();
        const auto instanceVBO = gl::genBuffer();
        const auto EBO = gl::genBuffer();
        const auto instanceDataSize = game_size.x * game_size.y + 1 + 4;

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(uint32_t), indexes.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); gl::checkError();
        glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * instanceDataSize, nullptr, GL_DYNAMIC_DRAW); gl::checkError();

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)(0 * sizeof(glm::vec4))); gl::checkError();
        glEnableVertexAttribArray(1); gl::checkError();

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)(1 * sizeof(glm::vec4))); gl::checkError();
        glEnableVertexAttribArray(2); gl::checkError();

        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)(2 * sizeof(glm::vec4))); gl::checkError();
        glEnableVertexAttribArray(3); gl::checkError();

        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)(3 * sizeof(glm::vec4))); gl::checkError();
        glEnableVertexAttribArray(4); gl::checkError();

        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)(4 * sizeof(glm::vec4))); gl::checkError();
        glEnableVertexAttribArray(5); gl::checkError();

        glVertexAttribDivisor(1, 1); gl::checkError();
        glVertexAttribDivisor(2, 1); gl::checkError();
        glVertexAttribDivisor(3, 1); gl::checkError();
        glVertexAttribDivisor(4, 1); gl::checkError();
        glVertexAttribDivisor(5, 1); gl::checkError();

        glBindBuffer(GL_ARRAY_BUFFER, 0); gl::checkError();
        glBindVertexArray(0); gl::checkError();

        auto instanceData = std::vector<InstanceData>();
        instanceData.resize(instanceDataSize);

        auto& apple = instanceData[0];

        auto& top = instanceData[1];
        top.color = { 0.5, 0.25, 0.75, 1 };
        top.transformation = glm::translate(glm::mat4(1.f), glm::vec3(float(game_size.x - 1) / 2, game_size.y, 0));
        top.transformation = glm::scale(top.transformation, glm::vec3(game_size.x, 1, 1));
        auto& left = instanceData[2];
        left.color = top.color;
        left.transformation = glm::translate(glm::mat4(1.f), glm::vec3(-1, float(game_size.y - 1) / 2, 0));
        left.transformation = glm::scale(left.transformation, glm::vec3(1, game_size.y, 1));
        auto& right = instanceData[3];
        right.color = top.color;
        right.transformation = glm::translate(glm::mat4(1.f), glm::vec3(game_size.x, float(game_size.y - 1) / 2, 0));
        right.transformation = glm::scale(right.transformation, glm::vec3(1, game_size.y, 1));
        auto& bottom = instanceData[4];
        bottom.color = top.color;
        bottom.transformation = glm::translate(glm::mat4(1.f), glm::vec3(float(game_size.x - 1) / 2, -1, 0));
        bottom.transformation = glm::scale(bottom.transformation, glm::vec3(game_size.x, 1, 1));

        float step = 0.f;
        bool updateBuffer = false;
        bool game_lost = false;

        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);

            //glClearColor(155.f / 255.f, 196.f / 255.f, 226.f / 255.f, 1.0f);
            glClearColor(0, 0, 0, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ourShader.use();

            glm::mat4 projection = cam.projection(SCR_WIDTH, SCR_HEIGHT);
            ourShader.setMat4("projection", projection);

            glm::mat4 view = cam.view();
            ourShader.setMat4("view", view);

            instanceData[0].color = glm::vec4(1, 0, 0, 1);
            auto apple_transformation = glm::translate(glm::mat4(1.f), glm::vec3(game.apple(), 0));
            apple_transformation = glm::rotate(apple_transformation, currentFrame, glm::vec3(currentFrame,currentFrame,currentFrame));
            apple_transformation = glm::scale(apple_transformation, glm::vec3(0.5, 0.5, 0.5));
            instanceData[0].transformation = apple_transformation;

            auto snakeIt = game.snake().begin();
            for (int i = 5; i < 5 + game.snake().size(); ++i)
            {
                auto transformation = glm::mat4(1.0);

                transformation = glm::translate(transformation, glm::vec3(snakeIt->x, snakeIt->y, 0));
                //transformation = glm::rotate(transformation, currentFrame, glm::vec3(0, 1, 0));
                transformation = glm::scale(transformation, glm::vec3(0.75f, 0.75f, 0.75f));

                instanceData[i].transformation = transformation;
                instanceData[i].color = glm::lerp(glm::vec4(0.25f, 0.5f, 1.f, 1.f), glm::vec4(0.f, 1.f, 0.f, 1.f), float(i) / (game_size.x * game_size.y));

                if (snakeIt++ == game.snake().end())
                    break;

                //snakeIt++;
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

            if (updateBuffer)
            {
                const auto dataSize = sizeof(InstanceData) * (instanceDataSize);
                glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
                gl::checkError();
                glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, instanceData.data());
                gl::checkError();
                updateBuffer = false;
            }

            glBindVertexArray(VAO);
            gl::checkError();
            glDrawElementsInstanced(GL_TRIANGLE_STRIP, indexes.size(), GL_UNSIGNED_INT, nullptr, instanceDataSize);
            gl::checkError();

            glfwSwapBuffers(window);
            glfwPollEvents();

            if (currentFrame > step && !game.win() && !game_lost)
            {
                game.update(bot.next_move());
                //if (next_move.empty())
                //{
                //    game_lost = game.update({ 0, 0 });
                //}
                //else
                //{
                //    game_lost = game.update(next_move.front());
                //    next_move.pop();
                //}
                step += 0.001;
                updateBuffer = true;
            }
        }

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.moveForward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.moveBack(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.strafeLeft(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.strafeRight(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        next_move.push({ 0, 1 });
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        next_move.push({ 0, -1 });
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        next_move.push({ -1, 0 });
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        next_move.push({ 1, 0 });
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    cam.process_mouse(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.fov(-yoffset);
}
