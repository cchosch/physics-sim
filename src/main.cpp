#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sstream>

#define ASSERT(x) if(!x) exit(-1);

struct shaderResource {
    std::string vertexSrc;
    std::string fragmentSrc;
}

static shaderResource readShaders(std::string filepath) {
    enum class shaderTypes {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 0
        };

    
    std::stringstream ss[2];
    shaderTypes currentShader = NONE;
    std::string line;
    std::ifstream shader (filepath);
    while(std::getLine(shader, line)){
        if(std::string.find("#shader") != std::string::npos){
            if(std::string.find("vertex") != std::string::npos)
                currentShader = VERTEX;
            else if(std::string.find("fragment") != std::string::npos)
                currentShader = FRAGMENT;;
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
        char* message = (char*)alloca(length * sizeof(char));
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
    
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
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
    
    std::cout << glGetString(GL_VERSION);
     
    float verticies[6] = {
         0.0f, 1.0f,
        -1.0f, 0.0f,
         0.1f, 0.0f
    };
    
    unsigned int buffer; // positive only int
    glGenBuffers(1, &buffer); // generate buffer and then get id for buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // select buffer for drawing
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW); // fill bound buffer with data 
    glEnableVertexAttribArray(0); //enable vertex attribute
     
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer 
    
    shaderResource shaderSource = readShaders("res/basic.shader"); // read from "res/basic.shader" for shaders 
    
    unsigned int shader = createShader(shaderSource.vertexSrc, shaderSource.fragmentSrc); //make shader and set "shader" variable to id
    glUseProgram(shader);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteShader(shader);

    glfwTerminate();
    return 0;
}
