#define GLM_ENABLE_EXPERIMENTAL
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))  
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))  
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "resource_utils.h"
#include <iostream>
#include <vector>
#include "camera.h"
#include "math_utils.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool isKeyboardDone[1024] = {0};
bool outerLine = false;  
bool glFill = false;

// setting
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 1.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2016-14043 LEE JUNYEONG", NULL, NULL);
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
    

    // build and compile our shader program
    // ------------------------------------
    // TODO: define 3 shaders
    // (1) geometry shader for spline render.
    // (2) simple shader for spline's outer line render.
    // (optional) (3) tessellation shader for bezier surface.
    Shader splineShader("../shaders/splines/spline_shader.vs", "../shaders/splines/spline_shader.fs", "../shaders/splines/spline_shader.gs");
    Shader outerShader("../shaders/outer_line_shader.vs", "../shaders/outer_line_shader.fs");
    Shader bezierShader("../shaders/bezier_surface/tess.vs", "../shaders/bezier_surface/tess.fs", "../shaders/bezier_surface/tess.gs", "../shaders/bezier_surface/TCS.glsl", "../shaders/bezier_surface/TES.glsl");

    // TODO : load requied model and save data to VAO. 
    // Implement and use loadSplineControlPoints/loadBezierSurfaceControlPoints in resource_utils.h

    VAO *simpleVAO, *uVAO, *complexVAO, *gumboVAO, *teapotVAO, *sphereVAO, *heartVAO;
    simpleVAO = loadSplineControlPoints("../resources/spline_control_point_data/spline_simple.txt");
    uVAO = loadSplineControlPoints("../resources/spline_control_point_data/spline_u.txt");
    complexVAO = loadSplineControlPoints("../resources/spline_control_point_data/spline_complex.txt");
    gumboVAO = loadBezierSurfaceControlPoints("../resources/bezier_surface_data/gumbo.bpt");
    teapotVAO = loadBezierSurfaceControlPoints("../resources/bezier_surface_data/teapot.bpt");
    sphereVAO = loadBezierSurfaceControlPoints("../resources/bezier_surface_data/sphere.bpt");
    heartVAO = loadBezierSurfaceControlPoints("../resources/bezier_surface_data/heart.bpt");

    glm::mat4 BZ = glm::mat4(glm::vec4(-1,3,-3,1), glm::vec4(3,-6,3,0), glm::vec4(-3,3,0,0), glm::vec4(1,0,0,0));
    glm::mat4 CR = glm::mat4(glm::vec4(-0.5,1.5,-1.5,0.5), glm::vec4(1,-2.5,2,-0.5), glm::vec4(-0.5,0,0.5,0), glm::vec4(0,1,0,0));
    glm::mat4 BS = glm::mat4(glm::vec4(-0.167,0.5,-0.5,0.167), glm::vec4(0.5,-1,0.5,0), glm::vec4(-0.5,0,0.5,0), glm::vec4(0.167,0.667,0.167,0));

    // render loop
    // -----------
    float oldTime = 0;
    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        float dt = currentTime - oldTime;
        deltaTime = dt;
        oldTime = currentTime;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO : render splines
        // (1) render simple spline with 4 control points for Bezier, Catmull-Rom and B-spline.
        // (2) render 'u' using Bezier spline
        // (3) render loop using Catmull-Rom spline and B-spline.
        // You have to also render outer line of control points!
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    
        splineShader.use();
        splineShader.setMat4("view", view);
        splineShader.setMat4("projection", projection);

        glBindVertexArray(simpleVAO->ID);
        splineShader.setMat4("B", BZ);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
        splineShader.setMat4("model", model);
        glDrawArrays(GL_LINES_ADJACENCY, 0, 4);

        splineShader.setMat4("B", BS);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
        splineShader.setMat4("model", model);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 4);

        splineShader.setMat4("B", CR);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
        splineShader.setMat4("model", model);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 4);
        glBindVertexArray(0);

        //draw u
        glBindVertexArray(uVAO->ID);
        splineShader.setMat4("B", BZ);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.8f, -0.2f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        splineShader.setMat4("model", model);
        glDrawArrays(GL_LINES_ADJACENCY, 0, uVAO->dataSize);
        glBindVertexArray(0);

        //draw complex
        glBindVertexArray(complexVAO->ID);
        splineShader.setMat4("B", CR);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        splineShader.setMat4("model", model);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, complexVAO->dataSize);
        
        splineShader.setMat4("B", BS);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        splineShader.setMat4("model", model);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, complexVAO->dataSize);
        glBindVertexArray(0);


        if(outerLine) {
            outerShader.use();
            outerShader.setMat4("view", view);
            outerShader.setMat4("projection", projection);
            
            glBindVertexArray(simpleVAO->ID);
            for (unsigned int i = 0; i < 3; i++)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-4.0f + (float)i, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
                outerShader.setMat4("model", model);
                glDrawArrays(GL_LINE_STRIP, 0, 4);
            }
            glBindVertexArray(0);
            
            glBindVertexArray(uVAO->ID);
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-0.8f, -0.2f, 0.0f));
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
            outerShader.setMat4("model", model);
            glDrawArrays(GL_LINE_STRIP, 0, uVAO->dataSize);
            glBindVertexArray(0);

            glBindVertexArray(complexVAO->ID);
            for (unsigned int i = 0; i < 2; i++)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(1.0f + (float)i, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
                outerShader.setMat4("model", model);
                glDrawArrays(GL_LINE_STRIP, 0, complexVAO->dataSize);
            }
            glBindVertexArray(0);
        }

        // (Optional) TODO : render Bezier surfaces using tessellation shader.
        bezierShader.use();
        bezierShader.setMat4("view", view);
        bezierShader.setMat4("projection", projection);
        bezierShader.setVec3("cameraPosition", camera.Position);
        bezierShader.setFloat("scale", 1.0f);

        glBindVertexArray(gumboVAO->ID);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-6.0f, -1.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        bezierShader.setMat4("model", model);
        glPatchParameteri(GL_PATCH_VERTICES, 16);
        glDrawArrays(GL_PATCHES, 0, gumboVAO->dataSize);
        glBindVertexArray(0);
        
        glBindVertexArray(sphereVAO->ID);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        bezierShader.setMat4("model", model);
        glPatchParameteri(GL_PATCH_VERTICES, 16);
        glDrawArrays(GL_PATCHES, 0, sphereVAO->dataSize);
        glBindVertexArray(0);

        glBindVertexArray(teapotVAO->ID);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        bezierShader.setMat4("model", model);
        glPatchParameteri(GL_PATCH_VERTICES, 16);
        glDrawArrays(GL_PATCHES, 0, teapotVAO->dataSize);
        glBindVertexArray(0);

        glBindVertexArray(heartVAO->ID);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        bezierShader.setMat4("model", model);
        glPatchParameteri(GL_PATCH_VERTICES, 16);
        glDrawArrays(GL_PATCHES, 0, heartVAO->dataSize);
        glBindVertexArray(0);

        if(glFill) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1,&VAO);
    //glDeleteBuffers(1, &VBO);

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
    // TODO : 
    // (1) (for spline) if we press key 9, toggle whether to render outer line.
    // (2) (Optional, for Bezier surface )if we press key 0, toggle GL_FILL and GL_LINE.   
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && !isKeyboardDone[GLFW_KEY_9]) {
        isKeyboardDone[GLFW_KEY_9] = true;
        if(outerLine) outerLine = false;
        else outerLine = true;
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && !isKeyboardDone[GLFW_KEY_0]) {
        isKeyboardDone[GLFW_KEY_0] = true;
        if(glFill) glFill = false;
        else glFill = true;
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE) {
        isKeyboardDone[GLFW_KEY_9] = false;
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE) {
        isKeyboardDone[GLFW_KEY_0] = false;
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}