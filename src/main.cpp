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
#include "../include/3DObjectCreator.hpp"
#include <stdlib.h>

using namespace std;
using namespace cv;

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aBary;
layout (location = 2) in vec3 instanceOffset;

//in vec3 a_dist;
out vec3 dist;
out vec4 position;
out vec4 minmax;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 MinMax;



void main()
{
    vec3 worldPos = aPos + instanceOffset;
    dist = aBary;
    gl_Position = projection * view * vec4(worldPos, 1.0);
    position = vec4(worldPos, 1.0);
    minmax = MinMax;
})";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 dist;
in vec4 position;
in vec4 minmax;
out vec4 FragColor;
void main()
{
    float d = min(dist.x, min(dist.y, dist.z));


    if (d < 0.02f){
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    } else {
        //FragColor = vec4(1.0f, 0.5f, 0.0f, 1.0f);
        // Normalisation :
        float h = (position.y - minmax[0]) / (minmax[1] - minmax[0]);
        vec3 color = mix(vec3(0.26,0.0,0.32), vec3(1.0,1.0,0.15), h);
        FragColor = vec4(color, 1.0f);
    }
    
}

)"; 

struct CameraMovement
{
    glm::vec3 angle;
    glm::vec3 cameraPos ;
    glm::vec3 cameraFront ;
    glm::vec3 cameraUp ;
};

struct GeneralSettings
{
    float cubeSize; //1
    float spacingCube; //0.5
    float scaleFactor; 
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
    std::cout << "Example: x^2+y^2 \n";

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

    std::cout << "\033[31m"; // red text

    std::cout << R"(
==============================================================================
TIPS: IF YOU DON'T SEE YOUR FUNCTION, USE YOUR KEYBOARD ARROWS & ZQSD CONTROLS
==============================================================================
)" << '\n';

    std::cout << "\033[0m"; // reset color

    function.yMax = 0.0f;
    function.yMin = 0.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
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

std::vector<float> createCube(GeneralSettings settings)
{
    float size = settings.cubeSize/2.0f;

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

    GeneralSettings settings;


    FunctionData UserFunction;
    Function func; //unction class object
    ImageData UserImage;

    ImageProcessing iprocess;
    ObjectCreator object;
    ObjectGeneratorSettings ObjSettings;

    while (!finished)
    {

    std::cout << "\033[34m";
    std::cout << R"(

    =======================================================================
   
     __     ___      _                 _         _   _                 
     \ \   / (_)___ (_)___  ___  _ __ (_) ___   | | | | ___  _ __ ___ 
      \ \ / /| / __|| / __|/ _ \| '_ \| |/ __|  | |_| |/ _ \| '__/ _ \
       \ V / | \__ \| \__ \ (_) | | | | | (__   |  _  | (_) | | |  __/
        \_/  |_|___/_|_|___/\___/|_| |_|_|\___| |_| |_|\___/|_|  \___|

    =======================================================================

                        [ ROTATION CONTROLS ]
    -----------------------------------------------------------------------
            <--  /  -->            Rotate Left  /  Rotate Right
        Up Arrow  / Down Arrow     Rotate Up    /  Rotate Down
                        [ MOVEMENT CONTROLS ]
    -----------------------------------------------------------------------
            Z              Move Forward / Zoom
            S              Move Backward / Zoom out
            Q              Move Left
            D              Move Right

    =======================================================================
                        GRID INITIALISATION
    -----------------------------------------------------------------------
            Enter a positive integer to generate your world

> )";
    std::cout << "\033[0m"; // Reset


        std::cout << "Width? :\n";
        std::cin >> WIDTH;
        if( WIDTH <=0 ){
            std::cout << "Please enter a positive number...\n";
            WIDTH = NULL;
            continue;
        }

        std::cout << "Height? :\n";
        std::cin >> HEIGHT;

        if (HEIGHT <=0){
            std::cout << "Please enter a positive number...\n";
            HEIGHT = NULL;
            continue;
        }

        std::cout <<"Do you want to export the 3D model ? (y/n) :\n";
        char exportChoice;
        std::cin >> exportChoice;
        if (exportChoice != 'y' && exportChoice != 'n'){
            std::cout << "Please enter y or n...\n";
            exportChoice = 'n';
            continue;
        }
        if (exportChoice == 'y') {
            ObjSettings.exportModel = true;

            std::cout << "Cube size (in milimeter 5 recommended) :\n";
            std::cin >> settings.cubeSize;

            if (settings.cubeSize <=0){
                std::cout << "Please enter a positive number...\n";
                settings.cubeSize = NULL;
                continue;
            }

            std::cout << "Spacing between cube (in milimeter 2 recommended) :\n";
            std::cin >> settings.spacingCube;
            if (settings.spacingCube <0){
                std::cout << "Please enter a positive number...\n";
                continue;
            }

            std::cout << "Dimensions of the side of the square support plate (in milimeter " << (settings.cubeSize+settings.spacingCube)*max(HEIGHT, WIDTH)+2 << " recommended) :\n";
            std::cin >> ObjSettings.surfaceSize;
            if (ObjSettings.surfaceSize <=0){
                std::cout << "Please enter a positive number...\n";
                ObjSettings.surfaceSize = NULL;
                continue;
            }

            std::cout << "Maximum height of the model (in milimeter 100 recommended) :\n";
            std::cin >> ObjSettings.maxHeight;
            if (ObjSettings.maxHeight <=0){
                std::cout << "Please enter a positive number...\n";
                ObjSettings.maxHeight = NULL;
                continue;
            }


        } else {
            ObjSettings.exportModel = false;
            settings.cubeSize = 0.5f;
            settings.spacingCube = 0.5f;
            ObjSettings.maxHeight = 100.0f;
            ObjSettings.surfaceSize = 100.0f;
        }
        



    finished = true ;
    }
    float temp1 = settings.cubeSize;
    float temp2 = settings.spacingCube;

    settings.scaleFactor = (temp1 + temp2)*2;

        // Generate 3D model :



    ObjSettings.cubeSize = settings.cubeSize;
    ObjSettings.spacing = settings.spacingCube;



    std::cout << WIDTH << " * " << HEIGHT << "\n";


    finished = false;

    system("cls");

    while (!finished)
    {
        std::cout << "======================================\n";
        std::cout << "Choose your mode\n";
        std::cout << "======================================\n";
        std::cout << "1 : Function in 3D Space 2: Image\n";

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
    else if (MODE != 2 && MODE !=1){
        std::cout <<"MODE : Image\n";
        loadImage(UserImage);
        UserImage.grid = iprocess.GrayScaleGridConverter(UserImage, HEIGHT, WIDTH);
    }




    //init glfw and window

    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Volumetric Visualization", NULL, NULL);


    //Init movement var
    CameraMovement camera;

    camera.angle       = glm::vec3(10.0f, 0.0f, 0.0f);
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
    //std::string fragmentCode = loadShaderSource("from-pixels-to-blocks/shaders/fragment.glsl");
    //const char* fragmentShaderSource = fragmentCode.c_str();

    //std::string vertexCode = loadShaderSource("from-pixels-to-blocks/shaders/vertex.glsl");
    //const char* vertexShaderSource = vertexCode.c_str();

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

    //if (vertexCode.empty()) std::cout << "Vertex shader empty!\n";
    //if (fragmentCode.empty()) std::cout << "Fragment shader empty!\n";

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

    std::vector<float> vertices = createCube(settings);

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


    std::vector<glm::vec3> instancePositions;
    glm::vec4 MinMax = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

    if (MODE == 1) {
        // Pre-evaluate all function values and build instance positions
        for (float i = -((float)(WIDTH * settings.scaleFactor) / 4); i < ((float)(WIDTH * settings.scaleFactor) / 4); i += settings.scaleFactor/2) {
            for (float j = -((float)(HEIGHT * settings.scaleFactor) / 4); j < ((float)(HEIGHT * settings.scaleFactor) / 4); j += settings.scaleFactor/2) {
                float y = func.RPNCalculator(UserFunction, i, j);
                instancePositions.push_back(glm::vec3(i, y, j));

                // Track min/max for color gradient
                if (y < UserFunction.yMin) UserFunction.yMin = y;
                if (y > UserFunction.yMax) UserFunction.yMax = y;
            }
        }

        MinMax = glm::vec4(UserFunction.yMin, UserFunction.yMax, 0.0f, 0.0f);
    }
    if (MODE == 2){
        int counter = 0;
        int gridSize = (int)UserImage.grid.size();
        MinMax = glm::vec4(*std::min_element(UserImage.grid.begin(), UserImage.grid.end()), *std::max_element(UserImage.grid.begin(), UserImage.grid.end()), 0.0f, 0.0f);
        // Build instance data once before the loop
        for (float i = -((float) (UserImage.rows*settings.scaleFactor)/4); i<((float) (UserImage.rows*settings.scaleFactor)/4); i+=settings.scaleFactor/2){
            for (float j = -((float) (UserImage.columns*settings.scaleFactor)/4); j<((float) (UserImage.columns*settings.scaleFactor)/4); j+=settings.scaleFactor/2){
                if (counter >= gridSize) {
                    std::cerr << "Grid overrun at counter " << counter << "\n";
                    break;
                }
                instancePositions.push_back(glm::vec3(i, UserImage.grid[counter++], j));
            }
        }

        //std::cout << "Instance count: " << instancePositions.size() << "\n";
    }


    if (ObjSettings.exportModel) {
        std::vector<glm::vec3> coords;
        copy(instancePositions.begin(), instancePositions.end(), back_inserter(coords)); 

        glm::vec3 min(object.getMin(coords));
        glm::vec3 max(object.getMax(coords));
        object.centredCoordsSys(coords, min, max);
        min = object.getMin(coords);
        max = object.getMax(coords);
        object.generateModel(coords, ObjSettings, min, max);
    }


    // Upload to a second VBO
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instancePositions.size() * sizeof(glm::vec3),
                instancePositions.data(), GL_STATIC_DRAW);

    // Bind as instanced attribute (location 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1); // advance once per instance, not per vertex

    glUseProgram(shaderProgram);
    glUniform4fv(glGetUniformLocation(shaderProgram, "MinMax"), 1, glm::value_ptr(MinMax));


    //init model 
    model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f)
    );

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    float farPlane = std::min((float)HEIGHT * settings.scaleFactor, 1000.0f);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // Call the keyboard interpreter
        processInput(window, camera);


        // Set view lookat
        view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
            1, GL_FALSE, glm::value_ptr(view));

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        projection = glm::perspective(
            glm::radians(45.0f),
            (float)width / (float)height,
            0.1f,
            farPlane
        );


        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
                1, GL_FALSE, glm::value_ptr(projection));



        if (MODE == 1 || MODE == 2) {
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instancePositions.size());
        } else if (MODE == 3){
            ;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
