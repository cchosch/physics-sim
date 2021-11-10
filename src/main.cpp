#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <chrono>

#define ASSERT(x) if(!(x)) __debugbreak();
#define glSafeCall(x) clearErrors(); x; ASSERT(logErrors(#x, __FILE__, __LINE__))

char const* gl_error_string(GLenum const err) noexcept
{
    switch (err)
    {
        // opengl 2 errors (8)
    case GL_NO_ERROR:
        return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";

    case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";

    case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW";

    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";

    case GL_TABLE_TOO_LARGE:
        return "GL_TABLE_TOO_LARGE";

        // opengl 3 errors (1)
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";

        // gles 2, 3 and gl 4 error are handled by the switch above
    default:
        return nullptr;
    }
}

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

static void clearErrors() {
    while (glGetError());
}

static bool logErrors(std::string function, std::string file, int line) {
    while (unsigned int error = glGetError()) {
        std::cout << "OPENGL ERROR " << gl_error_string(error) << " in function " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
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

    unsigned int buffer; // positive only int
    glSafeCall(glGenBuffers(1, &buffer)); // generate buffer and then get id for buffer
    glSafeCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); // select buffer for drawing
    glSafeCall(glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW)); // fill bound buffer with data 

    glSafeCall(glEnableVertexAttribArray(0)); //enable vertex attribute
    glSafeCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));
    
    // CREATE IBO BUFFER AND SEND IT INDICIE DATA
    unsigned int ibo; // positive only int
    glSafeCall(glGenBuffers(1, &ibo)); // generate buffer and then get id for buffer
    glSafeCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)); // select buffer for drawing
    glSafeCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW)); // fill bound buffer with data 

    shaderResource shaderSource = readShaders("res/basic.shader"); // read from "res/basic.shader" for shaders 
    
    glSafeCall(unsigned int shader = createShader(shaderSource.vertexSrc, shaderSource.fragmentSrc)); //make shader and set "shader" variable to id
    glSafeCall(glUseProgram(shader)); // bind shader so that we use it when rendering 

    glSafeCall(int uniformId = glGetUniformLocation(shader, "u_Color"));
    glSafeCall(glUniform4f(uniformId, 0.3, 1.0, 0.6, 1.0));
    float increment = -0.05f;
    float b = 1.0;
    std::chrono::steady_clock::time_point timeStart = std::chrono::steady_clock::now();
    int fps = 0;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)){
        fps++;
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glSafeCall(glUniform4f(uniformId, 0.3f, 0.6f, b, 1.0f));
        glSafeCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); // ibo is already bound so we can use nullptr

        if (b >= 1)
            increment = -0.05f;
        else if (b <= 0)
            increment = 0.05f;

        b += increment;
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

    glfwTerminate();
    return 0;
}
