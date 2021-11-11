#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <chrono>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct shaderResource {
    std::string vertexSrc;
    std::string fragmentSrc;
};

static shaderResource readShaders(std::string filepath) {
    enum class shaderTypes {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
        };

    
    std::stringstream ss[2];
    shaderTypes currentShader = shaderTypes::NONE;
    std::string line;
    std::ifstream shader (filepath);
    while(std::getline(shader, line)){
        if(line.find("#shader") != std::string::npos){
            if(line.find("vertex") != std::string::npos)
                currentShader = shaderTypes::VERTEX;
            else if(line.find("fragment") != std::string::npos)
                currentShader = shaderTypes::FRAGMENT;
        }
        else ss[(int) currentShader] << line << "\n";
    }
    shader.close();
    return { ss[0].str(), ss[1].str()};
}



static unsigned int CompileShader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type); // create shader and return id 
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr); // set source of shader with id of id
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        //shader did not compile properly
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader." << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader){
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;
    
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window){
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    GLenum glew_init = glewInit();
    if (glew_init != GLEW_OK) {
        std::cout << glewGetErrorString(glew_init);
        return -1;
    }
     
    std::cout << glGetString(GL_VERSION) << std::endl;
    {
        float verticies[8] = {
             0.0f, 1.0f,
            -1.0f, 0.0f,
             0.0f, 0.0f, 
            -1.0f, 1.0f 
        };
    
        unsigned int indicies[] = {
            1, 2, 0,
            1, 0, 3
        };
        unsigned int vao; // vertex array object
        glSafeCall(glGenVertexArrays(1, &vao)); // get id for vertex array
        glSafeCall(glBindVertexArray(vao)); // bind vertex array 
    
        VertexBuffer vb(verticies, sizeof(verticies));

        glSafeCall(glEnableVertexAttribArray(0)); //enable vertex attribute
        glSafeCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));
    
        IndexBuffer ib(indicies, 6);

        shaderResource shaderSource = readShaders("res/basic.shader"); // read from "res/basic.shader" for shaders 
    
        glSafeCall(unsigned int shader = createShader(shaderSource.vertexSrc, shaderSource.fragmentSrc)); //make shader and set "shader" variable to id
        glSafeCall(glUseProgram(shader)); // bind shader so that we use it when rendering 

        glSafeCall(int uniformId = glGetUniformLocation(shader, "u_Color"));
        glSafeCall(glUniform4f(uniformId, 0.3f, 1.0f, 0.6f, 1.0f));
        float increment = -0.05f;
        float b = 1.0;
        std::chrono::steady_clock::time_point timeStart = std::chrono::steady_clock::now();
        int fps = 0;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window)){
            fps++;
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            if (b >= 1)
                increment = -0.05f;
            else if (b <= 0)
                increment = 0.05f;
    
            b += increment;
    
            glSafeCall(glUniform4f(uniformId, 0.3f, 0.6f, b, 1.0f));


            glSafeCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr)); // ibo is already bound so we can use nullptr
    
            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
            if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-timeStart).count() > 1000){
               glfwSetWindowTitle(window, std::to_string(fps).c_str());
                timeStart = std::chrono::steady_clock::now();
                fps = 0;
            }
        }
        glDeleteShader(shader);
    }
    glfwTerminate();
    return 0;
}
