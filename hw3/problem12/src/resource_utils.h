#ifndef RESOURCE_UTILS_H
#define RESOURCE_UTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "opengl_utils.h"
using namespace std;
VAO* loadSplineControlPoints(string path)
{
	//TODO: load spline control point data and return VAO
	//You can make use of getVAOFromAttribData in opengl_utils.h
	vector<unsigned int> attrib_size;
	vector<float> attrib_data;

	ifstream ifs(path);
	int inputSize = 0;
    ifs >> inputSize;

	attrib_size.push_back(3);

    float dataIn;
    while(!ifs.eof()){
        ifs >> dataIn;
        attrib_data.push_back(dataIn);
    }
    ifs.close();

	VAO* vao;
	vao = getVAOFromAttribData(attrib_data, attrib_size);
	return vao;
}

VAO* loadBezierSurfaceControlPoints(string path)
{
	//(Optional)TODO: load surface control point data and return VAO.
	//You can make use of getVAOFromAttribData in opengl_utils.h
	vector<unsigned int> attrib_size;
	vector<float> attrib_data;

	ifstream ifs(path);
	int inputSize = 0;
    ifs >> inputSize;

	attrib_size.push_back(3);

    float dataIn;
	int numLine = 0;
    while(!ifs.eof()){
		if(numLine%17 == 0){
			ifs >> dataIn;
			ifs >> dataIn;
		}
		else{
			for(int i = 0; i < 3; i++){
				ifs >> dataIn;
				attrib_data.push_back(dataIn);
			}
		}
		numLine++;
    }
    ifs.close();

	VAO* vao;
	vao = getVAOFromAttribData(attrib_data, attrib_size);
	return vao;
	
}
#endif