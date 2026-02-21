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
#include <algorithm>
#include <stack>
#include <opencv2/opencv.hpp>
#include "../include/Function.hpp"
#include "../include/ImageProcessing.hpp"

using namespace std;
using namespace cv;

struct CameraMovement
{
    glm::vec3 angle;
    glm::vec3 cameraPos ;
    glm::vec3 cameraFront ;
    glm::vec3 cameraUp ;
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

float normalize(int MAX_Z, int MIN_Z, float value)
{
    return 0.0f;
}

void loadFunction(FunctionData& function)
{

    std::cout << "Enter your function (x,y)\n";

    // Clear leftover newline in the input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Use getline to read the whole line including spaces
    std::getline(std::cin, function.expression);

    // Remove all spaces
    function.expression.erase(
        std::remove(function.expression.begin(), function.expression.end(), ' '),
        function.expression.end()
    );

    std::cout << "You wrote : " << function.expression  << "\n";

    function.yMax = 0.0f;
    function.yMin = 0.0f;
}

void loadImage(ImageData& image)
{
    std::cout << "Enter the path of your image \n";

        // Clear leftover newline in the input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Use getline to read the whole line including spaces
    std::getline(std::cin, image.path);

    std::cout << "You wrote : " << image.path << "\n";
}

void processInput(GLFWwindow* window, CameraMovement& camera)
{
    const float cameraSpeed = 0.5f;
    const float incrementAngle = 1.0f * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        camera.angle.y += incrementAngle;

        if(camera.angle.y > 89.0f)
            camera.angle.y =  89.0f;
        if(camera.angle.y < -89.0f)
            camera.angle.y = -89.0f;

        float radius = sqrt(camera.cameraPos.x*camera.cameraPos.x + camera.cameraPos.y*camera.cameraPos.y + camera.cameraPos.z*camera.cameraPos.z  );
        

        float yaw   = glm::radians(camera.angle.x);
        float pitch = glm::radians(camera.angle.y);

        camera.cameraPos.x = cos(yaw) * cos(pitch);
        camera.cameraPos.z = sin(yaw) * cos(pitch);
        camera.cameraPos.y = sin(pitch);

        camera.cameraPos = camera.cameraPos * radius;

        camera.cameraFront = glm::normalize(-camera.cameraPos);

        camera.cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);



    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        camera.angle.y -= incrementAngle;

        if(camera.angle.y > 89.0f)
            camera.angle.y =  89.0f;
        if(camera.angle.y < -89.0f)
            camera.angle.y = -89.0f;

        float radius = sqrt(camera.cameraPos.x*camera.cameraPos.x + camera.cameraPos.y*camera.cameraPos.y + camera.cameraPos.z*camera.cameraPos.z  );
        
        float yaw   = glm::radians(camera.angle.x);
        float pitch = glm::radians(camera.angle.y);

        camera.cameraPos.x = cos(yaw) * cos(pitch);
        camera.cameraPos.z = sin(yaw) * cos(pitch);
        camera.cameraPos.y = sin(pitch);

        camera.cameraPos = camera.cameraPos * radius;

        camera.cameraFront = glm::normalize(-camera.cameraPos);

        camera.cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);



    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        camera.angle.x -= incrementAngle;

        float yaw = glm::radians(camera.angle.x);

        float radius = sqrt(pow(camera.cameraPos.x, 2.0) + pow(camera.cameraPos.z, 2.0) );

        camera.cameraPos.x = cos(yaw) * radius;
        camera.cameraPos.z = sin(yaw) * radius;

        camera.cameraFront = glm::normalize(-camera.cameraPos);

        camera.cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);



    }

    
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        camera.angle.x += incrementAngle;

        float yaw = glm::radians(camera.angle.x);

        float radius = sqrt(pow(camera.cameraPos.x, 2.0) + pow(camera.cameraPos.z, 2.0) );

        camera.cameraPos.x = cos(yaw) * radius;
        camera.cameraPos.z = sin(yaw) * radius;

        camera.cameraFront = glm::normalize(-camera.cameraPos);

        camera.cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.cameraPos += cameraSpeed * camera.cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.cameraPos -= cameraSpeed * camera.cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.cameraPos -= glm::normalize(
            glm::cross(camera.cameraFront, camera.cameraUp)
        ) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.cameraPos += glm::normalize(
            glm::cross(camera.cameraFront, camera.cameraUp)
        ) * cameraSpeed;
}

std::vector<float> createCube()
{
    float size = 0.5f;

    return {
        // back face
        -size, -size, -size, 1.0f, 0.0f, 0.0f,
         size, -size, -size, 0.0f, 1.0f, 0.0f,
         size,  size, -size, 0.0f, 0.0f, 1.0f,
         size,  size, -size, 1.0f, 0.0f, 0.0f,
        -size,  size, -size, 0.0f, 1.0f, 0.0f,
        -size, -size, -size, 0.0f, 0.0f, 1.0f,

        // front face
        -size, -size,  size, 1.0f, 0.0f, 0.0f,
         size, -size,  size, 0.0f, 1.0f, 0.0f,
         size,  size,  size, 0.0f, 0.0f, 1.0f,
         size,  size,  size, 1.0f, 0.0f, 0.0f,
        -size,  size,  size, 0.0f, 1.0f, 0.0f,
        -size, -size,  size, 0.0f, 0.0f, 1.0f,

        // left face
        -size,  size,  size, 1.0f, 0.0f, 0.0f,
        -size,  size, -size, 0.0f, 1.0f, 0.0f,
        -size, -size, -size, 0.0f, 0.0f, 1.0f,
        -size, -size, -size, 1.0f, 0.0f, 0.0f,
        -size, -size,  size, 0.0f, 1.0f, 0.0f,
        -size,  size,  size, 0.0f, 0.0f, 1.0f,

        // right face
         size,  size,  size, 1.0f, 0.0f, 0.0f,
         size,  size, -size, 0.0f, 1.0f, 0.0f,
         size, -size, -size, 0.0f, 0.0f, 1.0f,
         size, -size, -size, 1.0f, 0.0f, 0.0f,
         size, -size,  size, 0.0f, 1.0f, 0.0f,
         size,  size,  size, 0.0f, 0.0f, 1.0f,

        // bottom face
        -size, -size, -size, 1.0f, 0.0f, 0.0f,
         size, -size, -size, 0.0f, 1.0f, 0.0f,
         size, -size,  size, 0.0f, 0.0f, 1.0f,
         size, -size,  size, 1.0f, 0.0f, 0.0f,
        -size, -size,  size, 0.0f, 1.0f, 0.0f,
        -size, -size, -size, 0.0f, 0.0f, 1.0f,

        // top face
        -size,  size, -size, 1.0f, 0.0f, 0.0f,
         size,  size, -size, 0.0f, 1.0f, 0.0f,
         size,  size,  size, 0.0f, 0.0f, 1.0f,
         size,  size,  size, 1.0f, 0.0f, 0.0f,
        -size,  size,  size, 0.0f, 1.0f, 0.0f,
        -size,  size, -size, 0.0f, 0.0f, 1.0f
    };
}

void renderCube( glm::mat4& model, glm::mat4 projection, unsigned int shaderProgram, float xPos, float yPos, float zPos)
{
    model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(xPos, yPos, zPos)
        );

    glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "model"),
            1, GL_FALSE,
            glm::value_ptr(model)
        );

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main()
{

    // Init the user vars
    int WIDTH;
    int HEIGHT;
    int MODE;
    bool finished = false;

    FunctionData UserFunction;
    Function func; //unction class object
    ImageData UserImage;

    ImageProcessing iprocess;

    while (!finished)
    {
        std::cout << "======================================\n";
        std::cout << "Welcome to the visualisation program\n";
        std::cout << "======================================\n";
        std::cout << "Please select the dimension of your visualisation grid with positive integer ;) \n";

        std::cout << "Width? :\n";
        std::cin >> WIDTH;

        if( WIDTH <=0 ){
            std::cout << "Please enter a positive number...\n";
            continue;
        }

        std::cout << "Height? :\n";
        std::cin >> HEIGHT;

        if (HEIGHT <=0){
            std::cout << "Please enter a positive number...\n";
            continue;
        }

    finished = true ;
    }

    std::cout << WIDTH << " * " << HEIGHT << "\n";


    finished = false;

    system("cls");

    while (!finished)
    {
        std::cout << "======================================\n";
        std::cout << "Choose your mode\n";
        std::cout << "======================================\n";
        std::cout << "1 : Function in 3D Space 2: Image 3: Video\n";

        std::cout << "Enter the number:\n";
        std::cin >> MODE;

        if( MODE != 1 && MODE != 2 && MODE != 3 ){
            std::cout << "Please enter a positive number...\n";
            MODE = NULL;
            continue;
        }

    finished = true ;
    }
    system("cls");


    if (MODE == 1)
    {
        std::cout << "MODE : Function\n"; 
        loadFunction(UserFunction);
        func.ShuntingYard(UserFunction);
        //std::cout << RPNCalculator(UserFunction, 1.0f, 1.0f);
    }
    if (MODE == 2){
        std::cout <<"MODE : Image\n"; 
        loadImage(UserImage);
        UserImage.grid = iprocess.GrayScaleGridConverter(UserImage, HEIGHT, WIDTH);
        //int a = iprocess.SobelEdgeDetection(UserImage);
        //iprocess.CannyEdgeDetection(UserImage);
        //iprocess.HoughLineTransform(UserImage);
        //iprocess.ParallelogramFinder(iprocess.ParallelLineDetector(UserImage));
        //iprocess.Intersection(UserImage);
        //iprocess.creatDepthScale(UserImage);
    }
    if (MODE == 3){std::cout <<"MODE : Video\n" << "This is not implemented yet...\n";}




    //init glfw and window

    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1600, 1000, "Volumic vizualisation", NULL, NULL);


    //Init movement var
    CameraMovement camera;

    camera.angle       = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.cameraPos   = glm::vec3(0.0f, ((float) HEIGHT/2),  3.0f);
    camera.cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    camera.cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    glm::mat4 view;
    glm::mat4 model;
    glm::mat4 projection;

    //Init our key callback

    //glfwSetKeyCallback(window, key_callback);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 4. Initialiser les pointeurs d’attributs de barycentrics.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);





    


    float angle = 0.0f;
    float wave = 0.0f;
    //init model 
    model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f)
    );

    glm::vec4 MinMax = glm::vec4(-HEIGHT, HEIGHT, 0.0f, 0.0f);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // Give the minmax for color normalisation





        // Call the keyboard interpreter
        processInput(window, camera);

        

        // Set view lookat
        view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
            1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            1600.0f / 1000.0f,
            0.1f,
            100.0f
        );

        angle += 0.01f;


        if ((int) MODE == 1)
        {
            glm::vec4 MinMax = glm::vec4(UserFunction.yMin, UserFunction.yMax, 0.0f, 0.0f);
            glUniform4fv(glGetUniformLocation(shaderProgram, "MinMax"), 1, glm::value_ptr(MinMax));
            for (float i = -((float) (WIDTH*1.5)/2); i<((float) (WIDTH*1.5)/2); i+=1.5f){
                for (float j = -((float) (HEIGHT*1.5)/2); j<((float) (HEIGHT*1.5)/2); j+=1.5f){
                    renderCube(model, projection, shaderProgram, i, func.RPNCalculator(UserFunction, i, j) , j);
                }
            }
        } else if (MODE == 2) {
            int counter = 0;
            //Compute minma
            glm::vec4 MinMax = glm::vec4(*std::min_element(UserImage.grid.begin(), UserImage.grid.end()), *std::max_element(UserImage.grid.begin(), UserImage.grid.end()), 0.0f, 0.0f);
            glUniform4fv(glGetUniformLocation(shaderProgram, "MinMax"), 1, glm::value_ptr(MinMax));
            for (float i = -((float) (UserImage.rows*1.5)/2); i<((float) (UserImage.rows*1.5)/2); i+=1.5f){
                for (float j = -((float) (UserImage.columns*1.5)/2); j<((float) (UserImage.columns*1.5)/2); j+=1.5f){
                    renderCube(model, projection, shaderProgram, i, UserImage.grid[counter] , j);
                    counter++;
                }
            }
        } else if (MODE == 3){
            ;
        }

        projection = glm::perspective(
            glm::radians(45.0f),
            1600.0f / 900.0f,
            0.1f,
            100.0f
        );


        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
                1, GL_FALSE, glm::value_ptr(projection));


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
