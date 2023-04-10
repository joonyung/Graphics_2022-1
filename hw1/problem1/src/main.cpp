#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
glm::vec4 move(0.0f, 0.0f, 0.0f, 0.0f);
float angle = 0;

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

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions
         0.5f, -0.25f, 0.0f, // bottom right
        -0.5f, -0.25f, 0.0f, // bottom left
         0.0f,  0.5f, 0.0f  // top
    };

    unsigned int VBO, VAO;
    /**********Fill in the blank*********/
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /*************************************/


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render the triangle
        ourShader.use();
        float timeValue = glfwGetTime();

        // /**********Fill in the blank*********/
        float blueValue = 0.0f;
        if(static_cast<float>(sin(6 * timeValue) > 0.0f)) blueValue = 1.0f;
        ourShader.setVec4("ourColor", 0.0f, 0.0f, blueValue, 1.0f);

        ourShader.setVec4("moveCoord", move);
        
        float cosAngle = static_cast<float>(cos(angle));
        float sinAngle = static_cast<float>(sin(angle));
        ourShader.setVec2("CosSin", glm::vec2(cosAngle, sinAngle));
        /*************************************/

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    /**********Fill in the blank*********/
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) move[1] += 0.05f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) move[1] -= 0.05f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) move[0] += 0.05f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) move[0] -= 0.05f;
    //set to initial position
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        move[0] = 0.0f;
        move[1] = 0.0f;
        angle = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        if(angle > 360) angle -= 360;
        angle = angle + 0.1;
    }
    /*************************************/
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
