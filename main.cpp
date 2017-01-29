#include <stdio.h>
#include <math.h>
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <GLFW/glfw3.h>

#include <string>

#include "glhelper.hpp"
#include "verts.hpp"

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

    glEnable(GL_DEPTH_TEST);

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    vector<Vert<float, 6>> vertices = {
        // X      Y     Z     R     G     B
        { 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f}, // Vertex 1: Red
        {-0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f}, // Vertex 2: Green
        { 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f}, // Vertex 3: Blue
        { 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f}, // Vertex 1: Red
        {-0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f}, // Vertex 2: Green
        {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f}, // Vertex 4: Blue

        {-0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f}, // Vertex 1: Red
        {-0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f}, // Vertex 2: Green
        {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f}, // Vertex 3: Blue
        {-0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f}, // Vertex 1: Red
        {-0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f}, // Vertex 2: Green
        {-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f}, // Vertex 4: Blue
    };

    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vert<float, 6>),
           &vertices[0], GL_STATIC_DRAW);


    // Create the vertex shader
    const char* vertexSource = GLSL(
        in vec3 position;
        in vec3 color;
        out vec3 Color;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 proj;

        void main() {
            Color = color;
            gl_Position = proj * view * model * vec4(position, 1.0);
        }
    );

    // Create the fragment shader
    const char* fragmentSource = GLSL(

        in vec3 Color;
        out vec4 outColor;

        void main() {
            outColor = vec4(Color, 1.0f);
        }
    );

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    makeShader(vertexSource, fragmentSource, shaderProgram);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
        6*sizeof(float), 0);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float),
        (void*)(3*sizeof(float)));

    // ---------------------------- RENDERING ------------------------------ //

    // Setup Projection
    glm::mat4 proj =
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.5f, 20.0f);
    glm::mat4 model = glm::mat4();
    glm::mat4 view = glm::lookAt(
            glm::vec3( 2.0f,  2.0f, 1.0f),
            glm::vec3( 0.0f,  0.0f, 0.5f),
            glm::vec3( 0.0f,  0.0f, 1.0f)
        );

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
            1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"),
            1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),
            1, GL_FALSE, glm::value_ptr(model));

    while(!glfwWindowShouldClose(window))
    {
        // Calculate new triangle alpha for current frame
        GLint uniColor = glGetUniformLocation(shaderProgram, "vertexColor");

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the vertices
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

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
