#define GLM_ENABLE_EXPERIMENTAL
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "geometry_primitives.h"
#include <iostream>
#include <vector>
#include "camera.h"
#include "texture.h"
#include "texture_cube.h"
#include "math_utils.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool isWindowed = true;
bool isKeyboardProcessed[1024] = {0};

// setting
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(4.0f, 1.0f, -4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float dayFactor = 0.5;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
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

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // define normal shader and skybox shader.
    Shader shader("../shaders/shader.vs", "../shaders/shader.fs"); // you can name your shader files however you like
    Shader skyboxShader("../shaders/shader_skybox.vs","../shaders/shader_skybox.fs");


    // TODO : define required VAOs(textured cube, skybox, quad)
    // data are defined in geometry_primitives.h
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_positions_textures), &cube_positions_textures, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_positions), &skybox_positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    unsigned int quadVAO, quadVBO, quadEBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_positions_textures), &quad_positions_textures, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    


    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  1.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f,  2.2f, -2.5f),
        glm::vec3(-3.8f,  2.0f, -12.3f),
        glm::vec3( 2.4f,  1.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  1.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    const int n_grass = 1000;
    float grassGroundSize = 20;
    glm::vec3 grassPositions[n_grass];

    // positions of our grasses
    for(int i=0; i<n_grass; i++){
        float s = grassGroundSize/2;
        float x = getRandomValueBetween(-s, s);
        float z = getRandomValueBetween(-s, s);
        grassPositions[i].x = x;
        grassPositions[i].y = 0.5f;
        grassPositions[i].z = z;
    }


    // TODO : define textures (container, grass, grass_ground) & cubemap textures (day, night)
    Texture cubeTexture("../resources/container.jpg");

    std::vector<std::string> dayCube {
        "../resources/Sky Textures/right.jpg",
        "../resources/Sky Textures/left.jpg",
        "../resources/Sky Textures/top.jpg",
        "../resources/Sky Textures/bottom.jpg",
        "../resources/Sky Textures/front.jpg",
        "../resources/Sky Textures/back.jpg"
    };
    std::vector<std::string> nightCube {
        "../resources/Night Sky Textures/nightRight.png",
        "../resources/Night Sky Textures/nightLeft.png",
        "../resources/Night Sky Textures/nightTop.png",
        "../resources/Night Sky Textures/nightBottom.png",
        "../resources/Night Sky Textures/nightFront.png",
        "../resources/Night Sky Textures/nightBack.png"
    };
    CubemapTexture cubeMapTexture(dayCube);
    CubemapTexture nightTexture(nightCube);

    Texture grassTexture("../resources/grass.png");
    Texture groundTexture("../resources/grass_ground.jpg");

    // TODO : set texture & skybox texture uniform value (initialization)
    // e.g) shader.use(), shader.setInt("texture", 0);
    shader.use();
    shader.setInt("texture1", 0);

    skyboxShader.use();
    skyboxShader.setInt("day", 0);
    skyboxShader.setInt("night", 1);
    skyboxShader.setFloat("dayFactor", dayFactor);

    // render loop
    // -----------

    while (!glfwWindowShouldClose(window)){
        
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        /////////////////////////////////////////////////////
        // TODO : Main Rendering Loop
        /////////////////////////////////////////////////////

        // (1) render boxes(cube) using normal shader.
        // (2) render ground(quad) using normal shader.
        // (3) render billboard grasses(quad) using normal shader.
        // (4) render skybox using skybox shader.
  
        // rendering pseudo-code

        // update projection, view matrix to shader
        // for each model:
        //      bind VAO, texture
        //      for each entity that belongs to the model:
        //          update model(transformation) matrix to shader
        //          glDrawArrays or glDrawElements

		glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture.ID);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 23.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture.ID);
        for (int i = 0; i < n_grass; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, grassPositions[i]);
            glm::vec2 relativePosXZ = glm::normalize(glm::vec2(camera.Front.x, camera.Front.z));
            glm::vec2 zAxis = glm::vec2(0.0f, 1.0f);
            float angle = glm::acos(glm::dot(relativePosXZ, zAxis));
            if (relativePosXZ.x < 0) angle = -angle;
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
            shader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture.ID);
        glm::mat4 groundModel = glm::mat4(1.0f);
        groundModel = glm::rotate(groundModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        groundModel = glm::scale(groundModel, glm::vec3(grassGroundSize));
        shader.setMat4("model", groundModel);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        skyboxShader.setFloat("dayFactor", dayFactor);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture.textureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, nightTexture.textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteBuffers(1, &quadVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // TODO : make camera movable (WASD) & increase or decrease dayFactor(press O: increase, press P: decrease)

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        dayFactor += 0.02f;
        if (dayFactor > 1.0f)
            dayFactor = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        dayFactor -= 0.02f;
        if (dayFactor < 0.0f)
            dayFactor = 0.0f;
    }    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // TODO: calculate how much cursor have moved, rotate camera proportional to the value, using ProcessMouseMovement.
    float xpos_float = static_cast<float>(xpos);
    float ypos_float = static_cast<float>(ypos);

    if(firstMouse){
        lastX = xpos_float;
        lastY = ypos_float;
        firstMouse = false;
    }
    float xoffset = xpos_float - lastX;
    float yoffset = lastY - ypos_float;
    lastX = xpos_float;
    lastY = ypos_float;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
