#include "3DObjectCreator.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <fstream>
#include <limits>

std::fstream fileio;

glm::vec3 ObjectCreator::getMax(std::vector<glm::vec3> vector_lst){
    float maxI = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    float maxJ = std::numeric_limits<float>::lowest();

    for (glm::vec3 vect : vector_lst){
        if (vect[0] > maxI){
            maxI = vect[0];
        }
        if (vect[1] > maxY){
            maxY = vect[1];
        }
        if (vect[2] > maxJ){
            maxJ = vect[2];
        }
    }

    // Assuming convention: x = i, y = j, z = y
    return glm::vec3(maxI, maxY, maxJ);
}


glm::vec3 ObjectCreator::getMin(std::vector<glm::vec3> vector_lst){
    // reperes i,j, y ou y = f(i,j)
    float minI = std::numeric_limits<float>::max();
    float minJ = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();

    for (glm::vec3 vect : vector_lst){
        if (vect[0] < minI){
            minI = vect[0];
        }
        if (vect[1] < minY){
            minY = vect[1];
        }
        if (vect[2] < minJ){
            minJ = vect[2];
        }
    }

    return glm::vec3(minI, minY, minJ);
}

void normalize(std::vector<glm::vec3> &vector_lst, float maxY, glm::vec3 min, glm::vec3 max){
    for (glm::vec3& vect : vector_lst){
        vect[1] = ((vect[1] - min[1]) / (max[1] - min[1]) )* maxY;
    }
};

void ObjectCreator::centredCoordsSys(std::vector<glm::vec3> &vector_lst, glm::vec3 min, glm::vec3 max){  //pass by ref
    //and make it only positive
    glm::vec3 center = (max + min) / 2.0f;

    for (glm::vec3& vector : vector_lst){
        if (vector[0] < 0) {
            vector[0] += abs(center[0]);
        } else {
            vector[0] -= abs(center[0]);
        }
        if (vector[1] < 0) {
            vector[1] += abs(center[1]);
        } else {
            vector[1] -= abs(center[1]);
        }
        if (vector[2] < 0) {
            vector[2] += abs(center[2]);
        } else {
            vector[2] -= abs(center[2]);
        }

    }
};

void createBase(std::vector<float> &VModel, ObjectGeneratorSettings generationSettings){
    // Centred around 0;
    float size = generationSettings.surfaceSize;
    float height = 5.0f;
    float half = size/2;
    std::vector<float> base = {

        //front
        -half, 0.0f, -half,
        half, 0.0f, -half,
        half, height, -half,
        -half, 0.0f, -half,
        -half, height, -half,
        half, height, -half,

        //back
        -half, 0.0f, half,
        half, 0.0f, half,
        half, height, half,
        -half, 0.0f, half,
        -half, height, half,
        half, height, half,

        //left side 
        -half, 0.0f, -half,
        -half, 0.0f, half,
        -half, height, half,
        -half, height, half,
        -half, height, -half,
        -half, 0.0f, -half,

        //right side 
        half, 0.0f, -half,
        half, 0.0f, half,
        half, height, half,
        half, height, half,
        half, height, -half,
        half, 0.0f, -half,

        //bottom
        -half, 0.0f, -half,
        half, 0.0f, -half,
        half, 0.0f, half,
        half, 0.0f, half,
        -half, 0.0f, half,
        -half, 0.0f, -half,

        //top
        -half, height, -half,
        half, height, -half,
        half, height, half,
        half, height, half,
        -half, height, half,
        -half, height, -half,




        };
    copy(base.begin(), base.end(), back_inserter(VModel));
}

void createCube(glm::vec3 coords, std::vector<float> &VModel, ObjectGeneratorSettings generationSettings){
    //Si on utilise un autre cube size , nous perdons les ecart correctes entre les bloques , nous pouvons alors dire que toutes les auytres mesures sont en mm
    float size = generationSettings.cubeSize;
    float i = coords[0];
    float h = coords[1];
    float j = coords[2];
    float half = size/2;

    std::vector<float> cube = 
    {
        //back
        i-half, 0.0f, j-half,
        i+half, 0.0f, j-half,
        i+half, h, j-half,
        i+half, h, j-half,
        i-half, h, j-half,
        i-half, 0, j-half,

        //front
        i-half, 0.0f, j+half,
        i+half, 0.0f, j+half,
        i+half, h, j+half,
        i+half, h, j+half,
        i-half, h, j+half,
        i-half, 0.0f, j+half,

        //right side
        i+half, 0.0f, j+half,
        i+half, 0.0f, j-half,
        i+half, h, j-half,
        i+half, h, j+half,
        i+half, h, j-half,
        i+half, 0.0f, j+half,

        //left side
        i-half, 0.0f, j+half,
        i-half, 0.0f, j-half,
        i-half, h, j-half,
        i-half, h, j+half,
        i-half, h, j-half,
        i-half, 0.0f, j+half,

        //bottom
        i-half, 0.0f, j-half,
        i+half, 0.0f, j-half,
        i+half, 0.0f, j+half,
        i+half, 0.0f, j+half,
        i-half, 0.0f, j+half,
        i-half, 0.0f, j-half,

        //bottom
        i-half, h, j-half,
        i+half, h, j-half,
        i+half, h, j+half,
        i+half, h, j+half,
        i-half, h, j+half,
        i-half, h, j-half,

    };

    copy(cube.begin(), cube.end(), back_inserter(VModel));
}

void GenerateSTL(const std::vector<float>& VModel){
    std::ofstream MyFile("test.stl");
    MyFile << "solid name"<< "\n";
    // Write to the file
    for (int i=0; i<VModel.size(); i+=9) {
        glm::vec3 P1 = glm::vec3(VModel[i], VModel[i+1], VModel[i+2]);
        glm::vec3 P2 = glm::vec3(VModel[i+3], VModel[i+4], VModel[i+5]);
        glm::vec3 P3 = glm::vec3(VModel[i+6], VModel[i+7], VModel[i+8]);
        float Nx = ((P2[1]-P1[1])*(P3[2]-P1[2])) - ((P2[2]-P1[2])*(P3[1]-P1[1]));
        float Ny = ((P2[2]-P1[2])*(P3[0]-P1[0])) - ((P2[0]-P1[0])*(P3[2]-P1[2]));
        float Nz = ((P2[0]-P1[0])*(P3[1]-P1[1])) - ((P2[1]-P1[1])*(P3[0]-P1[0]));


        MyFile << "facet normal " << Nx << " " << Ny << " " << Nz << "\n";
        MyFile << "    outer loop" << "\n";
        MyFile << "        vertex " << P1[0] << " " << P1[1] << " " << P1[2] << "\n";
        MyFile << "        vertex " << P2[0] << " " << P2[1] << " " << P2[2] << "\n";
        MyFile << "        vertex " << P3[0] << " " << P3[1] << " " << P3[2] << "\n";
        MyFile << "    endloop" << "\n";
        MyFile << "endfacet" << "\n";
    }
    MyFile << "endsolid name";

    // Close the file
    MyFile.close();
}

void ObjectCreator::generateModel(std::vector<glm::vec3> vector_lst, ObjectGeneratorSettings generationSettings, glm::vec3 min, glm::vec3 max){
    float maxY = generationSettings.maxHeight;
    normalize(vector_lst, maxY, min, max);
    std::vector<float> VModel ;

    createBase(VModel, generationSettings);
    for ( glm::vec3 coords : vector_lst ) {
        createCube(coords, VModel, generationSettings);
    }
    GenerateSTL(VModel);
}