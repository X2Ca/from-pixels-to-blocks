#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


float vertices[] = {
    -0.5f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f,  0.5f, 0.0f,
    -0.5f, 0.0f, 0.0f,
    -0.5, 0.5f, 0.0f,
    0.0f,  0.5f, 0.0f
};

std::string loadShaderSource(const char* filepath)
{
    std::ifstream file(filepath);
    std::stringstream buffer;

    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    buffer << file.rdbuf();
    return buffer.str();
}



void creatSquare(unsigned int shaderProgram, unsigned int VAO, unsigned int VBO,  float xPos,  float yPos)
{
    float size = 0.15f;
    float square_vertice[] = {
    (-2*size + size*xPos), (size + size*yPos), 0.0f,
    (size + size*xPos), (size + size*yPos), 0.0f,
    (size + size*xPos),  (2*size + size*yPos), 0.0f,
    (-2*size + size*xPos), (size + size*yPos), 0.0f,
    (-2*size + size*xPos), (2*size + size*yPos), 0.0f,
    (size + size*xPos),  (2*size + size*yPos), 0.0f
    };
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertice), square_vertice, GL_DYNAMIC_DRAW);
    // 3. Initialiser les pointeurs d’attributs de sommets
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 6);    
}

void creatGrid(unsigned int shaderProgram, unsigned int VAO, unsigned int VBO)
{
    for (int i = -10; i<10; i++){
        for (int j = -10; j<10; j++){
            creatSquare(shaderProgram, VAO, VBO, i, j);
        }
    }
}


int main()
{

    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1600, 900, "Volumic vizualisation", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glfwSwapInterval(50);

    //Load shaders 
    std::string fragmentCode = loadShaderSource("shaders/fragment.glsl");
    const char* fragmentShaderSource = fragmentCode.c_str();

    std::string vertexCode = loadShaderSource("shaders/vertex.glsl");
    const char* vertexShaderSource = vertexCode.c_str();
    


    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::VERTEX_SHADER\n" << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::FRAGMENT_SHADER\n" << infoLog << std::endl;
    }

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM\n" << infoLog << std::endl;
    }


    //glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // 1. Lier le Vertex Array Object (VAO)
    glBindVertexArray(VAO);
    // 2. Copier les sommets dans un tampon pour qu’OpenGL les utilise
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. Initialiser les pointeurs d’attributs de sommets
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    float i = -10.0;
    float j = -10.0;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        creatGrid(shaderProgram, VAO, VBO);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        i++;
        if (i>=9.0){i=-10.0; j++;}
        if (j>=9.0){j=-10.0;}
    }

    glfwTerminate();
    return 0;
}
