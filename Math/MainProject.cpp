#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace std;
using namespace glm;

glm::vec4 translatePoint(vec4 vec) {
	
	

	// Initialize a mat4 (matrix 4x4) to the indentity matrix
	glm::mat4 trans = glm::mat4(1.0f);

	// define vec4 with its homogeneous coordicate set to 1.0
	trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	vec = trans * vec;

	printf("translated: {%f, %f, %f}\n", vec.x, vec.y, vec.z);
	return vec;
}

glm::vec4 scalePoint(vec4 vec) {

	glm::mat4 trans = glm::mat4(1.0f);
	// rotate 90 degree around Z-axis
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));

	// scale the container by 0.5 on each axis
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	vec = trans * vec;

	printf("scale and rotated: {%f, %f, %f}\n", vec.x, vec.y, vec.z);
	return vec;
}


void main1() {
	
	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	printf("original point: {%f, %f, %f}\n\n", vec.x, vec.y, vec.z);

	vec = translatePoint(vec);
	vec = scalePoint(vec);
}

