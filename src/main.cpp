#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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


std::vector<float> createCube()
{
    float size = 0.5f;

    return {
        // back face
        -size, -size, -size,
         size, -size, -size,
         size,  size, -size,
         size,  size, -size,
        -size,  size, -size,
        -size, -size, -size,

        // front face
        -size, -size,  size,
         size, -size,  size,
         size,  size,  size,
         size,  size,  size,
        -size,  size,  size,
        -size, -size,  size,

        // left face
        -size,  size,  size,
        -size,  size, -size,
        -size, -size, -size,
        -size, -size, -size,
        -size, -size,  size,
        -size,  size,  size,

        // right face
         size,  size,  size,
         size,  size, -size,
         size, -size, -size,
         size, -size, -size,
         size, -size,  size,
         size,  size,  size,

        // bottom face
        -size, -size, -size,
         size, -size, -size,
         size, -size,  size,
         size, -size,  size,
        -size, -size,  size,
        -size, -size, -size,

        // top face
        -size,  size, -size,
         size,  size, -size,
         size,  size,  size,
         size,  size,  size,
        -size,  size,  size,
        -size,  size, -size
    };
}



void creatSquare(unsigned int shaderProgram, unsigned int VAO, unsigned int VBO,  float xPos,  float yPos)
{
    float size = 0.15f;
    float square_vertice[] = {
    (-size + size*xPos), (size + size*yPos), 0.0f, //Face 1 begin
    (size + size*xPos), (size + size*yPos), 0.0f,
    (size + size*xPos),  (2*size + size*yPos), 0.0f,
    (-size + size*xPos), (size + size*yPos), 0.0f,
    (-size + size*xPos), (2*size + size*yPos), 0.0f,
    (size + size*xPos),  (2*size + size*yPos), 0.0f //Face 1 end

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
    //glViewport(0, 0, 1600, 900);

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

    glfwSwapInterval(1);

    //Load shaders 
    std::string fragmentCode = loadShaderSource("from-pixels-to-blocks/shaders/fragment.glsl");
    const char* fragmentShaderSource = fragmentCode.c_str();

    std::string vertexCode = loadShaderSource("from-pixels-to-blocks/shaders/vertex.glsl");
    const char* vertexShaderSource = vertexCode.c_str();

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

    


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

    if (vertexCode.empty()) std::cout << "Vertex shader empty!\n";
    if (fragmentCode.empty()) std::cout << "Fragment shader empty!\n";

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glEnable(GL_DEPTH_TEST);

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

    std::vector<float> vertices = createCube();

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // 1. Lier le Vertex Array Object (VAO)
    glBindVertexArray(VAO);
    // 2. Copier les sommets dans un tampon pour qu’OpenGL les utilise
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    // 3. Initialiser les pointeurs d’attributs de sommets
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    

    float angle = 0.0f;
    float distance = 10.0f;


    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        angle += 0.01f;
        distance -= 0.01f;

        if (distance <= 0.0){distance = 10.0f;}

        //glm::mat4 model = glm::rotate(
        //    glm::mat4(1.0f),
        //    angle,
        //    glm::vec3(1.0f, 1.0f, 0.0f)
        //);

        glm::mat4 model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, 0.0f, -distance)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "model"),
            1, GL_FALSE,
            glm::value_ptr(model)
        );
        glm::mat4 view = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)   // move camera backward
        );

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            1600.0f / 900.0f,
            0.1f,
            100.0f
        );

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
            1, GL_FALSE, glm::value_ptr(view));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
            1, GL_FALSE, glm::value_ptr(projection));

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
