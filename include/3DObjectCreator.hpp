#pragma once
#include <vector>
#include <glm/glm.hpp>

struct ObjectGeneratorSettings 
{
    //all in mm
    float surfaceSize; // sideSize
    float maxHeight; // maximum Height
    float cubeSize; // side size of the cube
    float spacing; // spacing between cubes
};

class ObjectCreator {
    public:
        ObjectCreator()  = default;
        glm::vec3 getMin(std::vector<glm::vec3> vector_lst);
        glm::vec3 getMax(std::vector<glm::vec3> vector_lst);
        void generateModel(std::vector<glm::vec3> vector_lst, ObjectGeneratorSettings generationSettings, glm::vec3 min, glm::vec3 max);
        void centredCoordsSys(std::vector<glm::vec3> &vector_lst, glm::vec3 min, glm::vec3 max);
};