#define GLM_ENABLE_EXPERIMENTAL
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))  
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))  
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "gameobjects.h"
#include "geometry_primitives.h"
#include <iostream>
#include <vector>
#include <string>
#include "text_renderer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, Bar& bar, float dt);

// setting
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 600;

bool previousKeyState[1024];

float previousKey = glfwGetTime();

int main()
{
    
    // glfw: initialize and configure
    // ------------------------------'
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
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("../shaders/shader.vs", "../shaders/shader.fs"); // you can name your shader files however you like


    /////////////////////////////////////////////////////
    // TODO : Define VAO and VBO for triangle and quad(bar).
    /////////////////////////////////////////////////////
    unsigned int VBO[2], VAO[2], EBO[2];

    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, EBO);

    //triangle
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_position_colors), triangle_position_colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    
    
    //rectangle
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_positions_colors), quad_positions_colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    

    std::vector<Entity> entities;

    TextRenderer  *Text;
    Text = new TextRenderer(SCR_WIDTH, SCR_HEIGHT);
    Text->Load("../fonts/OCRAEXT.TTF", 24);

    Bar bar{ 0, 0, 0 };

    // render loop
    // -----------
    float generationInterval = 0.3f;
    float dt = 0.05f;
    int score = 0;

    float barWidth = 0.3f;
    float barHeight = 0.05f;
    float barYPosition = -0.8f;

    bar.length = barWidth;
    bar.speed = barWidth/4;
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float pastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, bar, dt);

        // clear background
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        /////////////////////////////////////////////////////
        // TODO : Main Game Loop
        /////////////////////////////////////////////////////

        // (1) generate new triangle at the top of the screen for each time interval
        // (2) make triangles fall and rotate!!!!!
        // (3) Every triangle has different shape, falling speed, rotating speed.
        // (4) Render a red box
        // (5) Implement simple collision detection. Details are on the pdf file.
        // (6) Visualize the score & print score (you can use TextRenderer)
        float presentTime = glfwGetTime();
        if(presentTime - pastTime > generationInterval) {
            entities.push_back(getRandomEntity());
            pastTime = presentTime;
        }

        ourShader.use();
        for(auto itr = std::begin(entities); itr != std::end(entities); itr++){
            itr->rotate += itr->rotateSpeed/5;     //adjusted speed
            itr->position[1] -= itr->dropSpeed/30;
            
            ourShader.setVec3("Scale", itr->scale * glm::vec3(1.0f, 1.0f, 1.0f));
            ourShader.setFloat("Rotate", itr->rotate);
            ourShader.setVec3("Position", itr->position);
            glBindVertexArray(VAO[0]);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            ourShader.setVec3("Scale", glm::vec3(1.0f, 1.0f, 1.0f));
            ourShader.setFloat("Rotate", 0.0f);
            ourShader.setVec3("Position", glm::vec3(0.0f, 0.0f, 0.0f));
        }
        

        ourShader.setVec3("Scale", glm::vec3(barWidth, barHeight, 1.0f));
        ourShader.setVec3("Position", glm::vec3(bar.xPosition, barYPosition, 1.0f));
        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        ourShader.setVec3("Scale", glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setVec3("Position", glm::vec3(0.0f, 0.0f, 0.0f));


        while(true){
            auto itr = std::begin(entities);
            while(itr != std::end(entities)){
                float triXPosition = itr->position[0];
                float triYPosition = itr->position[1];
                if(triXPosition > bar.xPosition - barWidth/2 && 
                triXPosition < bar.xPosition + barWidth/2 &&
                triYPosition > barYPosition - barHeight/2 && 
                triYPosition < barYPosition + barHeight/2){
                    score++;
                    break;
                }
                if(triYPosition < -1.0f) break;
                itr++;
            }    
            if(itr == std::end(entities)) break;
            entities.erase(itr);
        }
        

        
        for(int i = 0; i < score; i++){
            ourShader.setVec3("Scale", 0.1f * glm::vec3(1.0f, 1.0f, 1.0f));
            ourShader.setFloat("Rotate", 0.0f);
            ourShader.setVec3("Position", glm::vec3(-0.9f + 0.1f * i, -0.9f, 0.0f));
            glBindVertexArray(VAO[0]);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            ourShader.setVec3("Scale", glm::vec3(1.0f, 1.0f, 1.0f));
            ourShader.setFloat("Rotate", 0.0f);
            ourShader.setVec3("Position", glm::vec3(0.0f, 0.0f, 0.0f));
        }
        
        if(score >= 20) score = 0;

        Text->RenderText("Score : " + std::to_string(score), 25.0f, 25.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, Bar& bar, float dt)
{
    float presentKey = glfwGetTime();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //////////////////////////////////
    // TODO : make the bar movable (consider interval time dt!)
    //////////////////////////////////
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && presentKey - previousKey > dt) {
        if(bar.xPosition + bar.length/2 + bar.speed <= 1.0f){
            bar.xPosition += bar.speed;
        }
        previousKey = presentKey;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && presentKey - previousKey > dt) {
        if(bar.xPosition - bar.length/2 - bar.speed >= -1.0f){
            bar.xPosition -= bar.speed;
        }
        previousKey = presentKey;
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
