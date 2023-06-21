#include "game_logic.hpp"
#include "bot.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "deps/stb_image.h"

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
#include <numeric>
#include <array>
#include <string_view>
#include <memory>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const* path);

unsigned int SCR_WIDTH = 2*800;
unsigned int SCR_HEIGHT = 2*600;

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
bool progressGame = false;

static constexpr auto grid_shader_paths = std::tuple{ std::string_view("D:/dev/snake-cpp/grid.vs"), std::string_view("D:/dev/snake-cpp/grid.fs") };
static constexpr auto sun_shader_paths = std::tuple{ std::string_view("D:/dev/snake-cpp/sun.vs"), std::string_view("D:/dev/snake-cpp/sun.fs") };
static constexpr auto skybox_shader_paths = std::tuple{ std::string_view("D:/dev/snake-cpp/skybox.vs"), std::string_view("D:/dev/snake-cpp/skybox.fs") };
static constexpr auto star_shader_paths = std::tuple{ std::string_view("D:/dev/snake-cpp/star.vs"), std::string_view("D:/dev/snake-cpp/star.fs")};
std::unique_ptr<shader> grid_shader;
std::unique_ptr<shader> sun_shader;
std::unique_ptr<shader> skybox_shader;
std::unique_ptr<shader> star_shader;

namespace cubes
{
    auto init(std::size_t instances_count)
    {
        static constexpr auto vertices = std::array{
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
        };

        const auto VAO = gl::genVertexArray();
        const auto VBO = gl::genBuffer();
        const auto instanceVBO = gl::genBuffer();

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); gl::checkError();
        glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * instances_count, nullptr, GL_DYNAMIC_DRAW); gl::checkError();

        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)(0 * sizeof(glm::vec4))); gl::checkError();
        glEnableVertexAttribArray(3); gl::checkError();

        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)(1 * sizeof(glm::vec4))); gl::checkError();
        glEnableVertexAttribArray(4); gl::checkError();

        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)(2 * sizeof(glm::vec4))); gl::checkError();
        glEnableVertexAttribArray(5); gl::checkError();

        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)(3 * sizeof(glm::vec4))); gl::checkError();
        glEnableVertexAttribArray(6); gl::checkError();

        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (const void*)(4 * sizeof(glm::vec4))); gl::checkError();
        glEnableVertexAttribArray(7); gl::checkError();

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribDivisor(3, 1); gl::checkError();
        glVertexAttribDivisor(4, 1); gl::checkError();
        glVertexAttribDivisor(5, 1); gl::checkError();
        glVertexAttribDivisor(6, 1); gl::checkError();
        glVertexAttribDivisor(7, 1); gl::checkError();

        glBindBuffer(GL_ARRAY_BUFFER, 0); gl::checkError();
        glBindVertexArray(0); gl::checkError();

        return std::tuple{ VAO, VBO, instanceVBO };
    }

    void init_border(InstanceData& top, InstanceData& left, InstanceData& right, InstanceData& bottom, const glm::vec2& game_size)
    {
        top.color = { 0.2, 0.2, 0.2, 1 };
        top.transformation = glm::translate(glm::mat4(1.f), glm::vec3(float(game_size.x - 1) / 2, game_size.y, 0));
        top.transformation = glm::scale(top.transformation, glm::vec3(game_size.x, 1, 1));

        left.color = top.color;
        left.transformation = glm::translate(glm::mat4(1.f), glm::vec3(-1, float(game_size.y - 1) / 2, 0));
        left.transformation = glm::scale(left.transformation, glm::vec3(1, game_size.y, 1));

        right.color = top.color;
        right.transformation = glm::translate(glm::mat4(1.f), glm::vec3(game_size.x, float(game_size.y - 1) / 2, 0));
        right.transformation = glm::scale(right.transformation, glm::vec3(1, game_size.y, 1));

        bottom.color = top.color;
        bottom.transformation = glm::translate(glm::mat4(1.f), glm::vec3(float(game_size.x - 1) / 2, -1, 0));
        bottom.transformation = glm::scale(bottom.transformation, glm::vec3(game_size.x, 1, 1));
    }

    void update_shader(shader& lighting_shader, const glm::vec2& point_light_1, const glm::vec2& point_light_2, const glm::mat4& projection, const glm::mat4& view)
    {
            lighting_shader.setVec3("viewPos", cam.position());
            lighting_shader.setFloat("material.shininess", 32.0f);

            /*
               Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
               the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
               by defining light types as classes and set their values in there, or by using a more efficient uniform approach
               by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
            */
            // directional light
            lighting_shader.setVec3("dirLight.direction", -cam.position());
            lighting_shader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
            lighting_shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
            lighting_shader.setVec3("dirLight.specular", 0.75f, 0.75f, 0.75f);
            // point light 1
            lighting_shader.setVec3("pointLights[0].position", glm::vec3(point_light_1, 2.f));
            lighting_shader.setFloat("material.shininess", 32.0f);
            lighting_shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
            lighting_shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
            lighting_shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
            lighting_shader.setFloat("pointLights[0].constant", 1.0f);
            lighting_shader.setFloat("pointLights[0].linear", 0.09f);
            lighting_shader.setFloat("pointLights[0].quadratic", 0.032f);

            lighting_shader.setVec3("pointLights[1].position", glm::vec3(point_light_2, 1.f));
            lighting_shader.setVec3("pointLights[1].ambient", 1.0f, 0.0f, 0.0f);
            lighting_shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
            lighting_shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
            lighting_shader.setFloat("pointLights[1].constant", 1.0f);
            lighting_shader.setFloat("pointLights[1].linear", 0.9f);
            lighting_shader.setFloat("pointLights[1].quadratic", 0.032f);
            // spotLight
            //lighting_shader.setVec3("spotLight.position", camera.Position);
            //lighting_shader.setVec3("spotLight.direction", camera.Front);
            //lighting_shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            //lighting_shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            //lighting_shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            //lighting_shader.setFloat("spotLight.constant", 1.0f);
            //lighting_shader.setFloat("spotLight.linear", 0.09f);
            //lighting_shader.setFloat("spotLight.quadratic", 0.032f);
            //lighting_shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            //lighting_shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

            lighting_shader.setMat4("projection", projection);

            lighting_shader.setMat4("view", view);
    }

    void update_apple(InstanceData& apple, const glm::vec2& apple_pos)
    {
        apple.color = glm::vec4(1, 0, 0, 1);
        auto apple_transformation = glm::translate(glm::mat4(1.f), glm::vec3(apple_pos, 0));
        //apple_transformation = glm::rotate(apple_transformation, currentFrame, glm::vec3(currentFrame,currentFrame,currentFrame));
        apple_transformation = glm::scale(apple_transformation, glm::vec3(0.5, 0.5, 0.5));
        apple.transformation = apple_transformation;
    }
}

namespace stars
{
    constexpr auto instances_count = 169;
    auto init()
    {
        auto rd = std::random_device{};
        auto gen = std::mt19937(rd());
        auto distrib = std::uniform_real_distribution(-1., 1.);

        auto stars_buffer = std::array<glm::vec3, instances_count>{};

        for (auto& star : stars_buffer)
        {
            auto x = distrib(gen);
            auto y = std::abs(distrib(gen)) + 0.2;
            auto z = (distrib(gen) - 1) / 2;

            constexpr auto r = 110.;
            // TODO handle x == y == z == 0;
            const auto magnitude = r / std::sqrt(x * x + y * y + z * z);

            x *= magnitude;
            y *= magnitude;
            z *= magnitude;

            star = glm::vec3(x, y, z);
        }

        const auto VAO = gl::genVertexArray();
        const auto VBO = gl::genBuffer();
        const auto instanceVBO = gl::genBuffer();

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); gl::checkError();
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * instances_count, stars_buffer.data(), GL_STATIC_DRAW); gl::checkError();

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribDivisor(0, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0); gl::checkError();
        glBindVertexArray(0); gl::checkError();

        return std::tuple{ VAO, VBO, instanceVBO };
    }
}

int main()
{
    glm::u32vec2 game_size{ 10, 10 };
    game_logic::game_logic game(game_size);
    game_logic::bot bot(game_size);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "", NULL, NULL);
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
    glfwSwapInterval(1);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);

    {
        shader lighting_shader("D:/dev/snake-cpp/lighting.vs", "D:/dev/snake-cpp/lighting.fs");
        grid_shader = std::make_unique<shader>(std::get<0>(grid_shader_paths), std::get<1>(grid_shader_paths));
        sun_shader = std::make_unique<shader>(std::get<0>(sun_shader_paths), std::get<1>(sun_shader_paths));
        skybox_shader = std::make_unique<shader>(std::get<0>(skybox_shader_paths), std::get<1>(skybox_shader_paths));
        star_shader = std::make_unique<shader>(std::get<0>(star_shader_paths), std::get<1>(star_shader_paths));

        const auto cube_instances_count = game_size.x * game_size.y + 1 + 4;
        const auto [cube_VAO, cube_VBO, cube_instances_VBO] = cubes::init(cube_instances_count);
        const auto [stars_VAO, stars_VBO, stars_instances_VBO] = stars::init();
        const auto grid_VAO = gl::genVertexArray();

        unsigned int diffuseMap = loadTexture("D:/dev/snake-cpp/container2.png");
        unsigned int specularMap = loadTexture("D:/dev/snake-cpp/container2_specular.png");
        lighting_shader.use();
        lighting_shader.setInt("material.diffuse", 0);
        lighting_shader.setInt("material.specular", 1);

        auto instance_data = std::vector<InstanceData>(cube_instances_count);

        auto& apple = instance_data[0];

        cubes::init_border(instance_data[1], instance_data[2], instance_data[3], instance_data[4], game_size);

        float step = 0.f;
        bool updateBuffer = true;
        bool game_lost = false;
        auto title = std::array<char, 20>{};
        auto frame_times = std::array<float, 100>{};
        auto frame_times_index = std::size_t{ 0 };

        while (!glfwWindowShouldClose(window))
        {
            static float frame_start_ts = 0.f;
            frame_start_ts = glfwGetTime();
            static float last_frame_end_ts = 0.f;

            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);

            //glClearColor(155.f / 255.f, 196.f / 255.f, 226.f / 255.f, 1.0f);
            glClearColor(0, 0, 0, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 projection = cam.projection(SCR_WIDTH, SCR_HEIGHT);
            glm::mat4 view = cam.view();

            lighting_shader.use();
            cubes::update_shader(lighting_shader, game.snake().front(), game.apple(), projection, view);

            // bind diffuse map
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);
            // bind specular map
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularMap);

            cubes::update_apple(apple, game.apple());

            auto snakeIt = game.snake().begin();
            for (int i = 5; i < 5 + game.snake().size(); ++i)
            {
                auto transformation = glm::mat4(1.0);

                transformation = glm::translate(transformation, glm::vec3(snakeIt->x, snakeIt->y, 0));
                //transformation = glm::rotate(transformation, currentFrame, glm::vec3(0, 1, 0));
                const auto scale = glm::lerp(0.75f, 0.5f, float(i - 5) / game.snake().size());
                //const auto scale = 0.75f;
                transformation = glm::scale(transformation, glm::vec3(scale));

                instance_data[i].transformation = transformation;
                instance_data[i].color = glm::lerp(glm::vec4(0.25f, 0.5f, 1.f, 1.f), glm::vec4(0.f, 1.f, 0.f, 1.f), float(i) / (game_size.x * game_size.y));

                if (snakeIt++ == game.snake().end())
                    break;
            }

            if (updateBuffer)
            {
                const auto dataSize = sizeof(InstanceData) * (cube_instances_count);
                glBindBuffer(GL_ARRAY_BUFFER, cube_instances_VBO);
                gl::checkError();
                glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, instance_data.data());
                gl::checkError();
                updateBuffer = false;
            }

            //glBindVertexArray(cube_VAO);
            //gl::checkError();
            //glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cube_instances_count);
            //gl::checkError();

            //glDepthMask(GL_FALSE);
            skybox_shader->use();
            skybox_shader->setMat4("view", view);
            skybox_shader->setMat4("projection", projection);
            skybox_shader->setVec3("cameraPos", cam.position());
            glBindVertexArray(grid_VAO);
            glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 36, 1, 0);
            gl::checkError();
            //glDepthMask(GL_TRUE);

            sun_shader->use();
            sun_shader->setMat4("view", view);
            sun_shader->setMat4("projection", projection);
            sun_shader->setVec3("cameraPos", cam.position());
            sun_shader->setFloat("time", currentFrame);
            sun_shader->setFloat("scaleFactor", 10.f);
            sun_shader->setVec3("translation", glm::vec3(-25, 20, -100) + cam.position());
            glBindVertexArray(grid_VAO);
            glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
            gl::checkError();

            star_shader->use();
            star_shader->setMat4("view", view);
            star_shader->setMat4("projection", projection);
            star_shader->setVec3("cameraPos", cam.position());
            star_shader->setFloat("time", currentFrame);
            star_shader->setFloat("scaleFactor", 0.1);
            glBindVertexArray(stars_VAO); gl::checkError();
            glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, stars::instances_count, 0);
            gl::checkError();

            grid_shader->use();
            grid_shader->setMat4("view", view);
            grid_shader->setMat4("projection", projection);
            grid_shader->setVec3("cameraPos", cam.position());
            glBindVertexArray(grid_VAO);
            glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
            gl::checkError();

            glfwSwapBuffers(window);
            glfwPollEvents();

            if (currentFrame > step && !game.win() && !game_lost)
            {
                game.update(bot.next_move());
                progressGame = false;
                //if (next_move.empty())
                //{
                //    game_lost = game.update({ 0, 0 });
                //}
                //else
                //{
                //    game_lost = game.update(next_move.front());
                //    next_move.pop();
                //}
                step += 0.1;
                updateBuffer = true;
            }

            const auto frame_total_time = (glfwGetTime() - frame_start_ts);

            frame_times[frame_times_index++ % frame_times.size()] = frame_total_time;
            const auto avg_fps = 1./(std::accumulate(frame_times.begin(), frame_times.end(), 0.f) / frame_times.size());

            // TODO fmt
            constexpr auto title_format = std::string_view("fps: %f");
            const int title_written_chars = std::snprintf(title.data(), title.size(), title_format.data(), avg_fps);
            assert(title_written_chars < title.size());
            glfwSetWindowTitle(window, title.data());
        }

        glDeleteVertexArrays(1, &cube_VAO);
        glDeleteBuffers(1, &cube_VBO);
        glDeleteBuffers(1, &cube_instances_VBO);
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
    if (action == GLFW_PRESS)
        progressGame = true;
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        next_move.push({ 0, 1 });
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        next_move.push({ 0, -1 });
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        next_move.push({ -1, 0 });
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        next_move.push({ 1, 0 });
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
    {
        try
        {
            {
                grid_shader = std::make_unique<shader>(std::get<0>(grid_shader_paths), std::get<1>(grid_shader_paths));
            }
            {
                sun_shader = std::make_unique<shader>(std::get<0>(sun_shader_paths), std::get<1>(sun_shader_paths));
            }
            {
                skybox_shader = std::make_unique<shader>(std::get<0>(skybox_shader_paths), std::get<1>(skybox_shader_paths));
            }
            {
                star_shader = std::make_unique<shader>(std::get<0>(star_shader_paths), std::get<1>(star_shader_paths));
            }
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << e.what() << "\n";
        }
    }

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

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
