#include <stdio.h>
#include <math.h>
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <GLFW/glfw3.h>

#include <string>

#include "glhelper.hpp"

int main()
{
    // -------------------------------- INIT ------------------------------- //

    // Init GLFW
    if (glfwInit() != GL_TRUE) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    // Create a rendering window with OpenGL 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);


    // ----------------------------- RESOURCES ----------------------------- //

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float vertices[] = {
         0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // Vertex 1: Red
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Vertex 2: Green
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Vertex 3: Green
         0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // Vertex 1: Red
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Vertex 2: Green
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f  // Vertex 3: Blue
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create and compile the fragment shader
    const char* fragmentSource = GLSL(
        uniform vec3 vertexColor;

        in vec3 Color;
        out vec4 outColor;

        void main() {
            outColor = vec4(vertexColor + Color, 1.0f);
        }
    );

    // Create and compile the vertex shader
    const char* vertexSource = GLSL(
        in vec2 position;
        in vec3 color;
        out vec3 Color;

        void main() {
            Color = color;
            gl_Position = vec4(position, 0.0, 1.0);
        }
    );

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    makeShader(vertexSource, fragmentSource, shaderProgram);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
        5*sizeof(float), 0);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float),
        (void*)(2*sizeof(float)));

    // ---------------------------- RENDERING ------------------------------ //

    while(!glfwWindowShouldClose(window))
    {
        // Calculate new triangle alpha for current frame
        GLint uniColor = glGetUniformLocation(shaderProgram, "vertexColor");
        float time = (float)glfwGetTime();
        float adjfactor = sin(time) - 0.5f;
        glUniform3f(uniColor, adjfactor, adjfactor, adjfactor);

		// Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw a triangle from the 3 vertices
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Swap buffers and poll window events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ---------------------------- CLEARING ------------------------------ //

    // Delete allocated resources
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    // ---------------------------- TERMINATE ----------------------------- //

    // Terminate GLFW
    glfwTerminate();

    return 0;
}
