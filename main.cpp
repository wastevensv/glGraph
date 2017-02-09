#include <stdio.h>
#include <math.h>
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#include "glhelper.hpp"
#include "graph.hpp"
#include "verts.hpp"

using namespace std;

float sinfunc(float x, float y) {
    return sin(x) * sin(y);
}

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
    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Generate verticies
    int size = 32;
    float res  = 0.5f;
    vector<Vert<float, 4>> vertices;
    calcSurface(-(size*res)/2, -(size*res)/2, res, res,
            size, size, sinfunc, &vertices);
    vector<int> elements;
    genIndicies(size,size,&elements);

    // Load buffers
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vert<float, 4>),
           &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(int),
           &elements[0], GL_STATIC_DRAW);

    // Find limits
    Vert<float, 4> max;
    maximum(vertices, max);
    Vert<float, 4> min;
    minimum(vertices, min);

    float dim[3] = {max[0]-min[0], max[1]-min[1], max[2]-min[2]};
    float center[3] = {(dim[0]/2)+min[0], (dim[1]/2)+min[1], (dim[2]/2)+min[2]};

    float maxdim;
    if(dim[0] > dim[1] && dim[0] > dim[2]) {
      maxdim = dim[0];
    } else {
      maxdim = dim[1];
    }

    cout << max[0] << ',' << max[1] << ',' << max[2] << endl;
    cout << min[0] << ',' << min[1] << ',' << min[2] << endl;
    cout << dim[0] << ',' << dim[1] << ',' << dim[2] << endl;
    cout << center[0] << ',' << center[1] << ',' << center[2] << endl;
    cout << maxdim << endl;

    // Create the vertex shader
    const char* vertexSource = GLSL(
        in vec3 position;
        in float color;
        out vec3 Color;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 proj;

        void main() {
            Color = vec3(color,0.5f,0.5f);
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
        4*sizeof(float), 0);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 1, GL_FLOAT, GL_FALSE,
        4*sizeof(float), (void*)(3*sizeof(float)));

    // ---------------------------- RENDERING ------------------------------ //

    // Setup Projection
    glm::mat4 proj =
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.5f, 100.0f);
    glm::mat4 model = glm::mat4();
    glm::mat4 view = glm::lookAt(
            glm::vec3( maxdim,  maxdim, maxdim),
            glm::vec3( 0.0f,  0.0f, 1.0f),
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

        glm::mat4 model = glm::mat4();
        model = glm::scale(model, glm::vec3(1.0f,1.0f,10.0f/dim[2]));
        model = glm::rotate(model, (GLfloat)fmod((glfwGetTime()),2.0f*3.14f), glm::vec3(0.0f,0.0f,1.0f));
        model = glm::translate(model, glm::vec3(-center[0],-center[1],0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),
            1, GL_FALSE, glm::value_ptr(model));

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the vertices
        glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);

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
