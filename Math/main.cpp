/*
 * =====================================================================================
 *
 *       Filename:  Texture.cpp
 *
 *    Description:  apply textures to a rectangular slab
 *
 *        Version:  1.0
 *        Created:  06/11/2014 18:41:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *
 * =====================================================================================
 */

#include "glfw_app.hpp"
#include "gl_shader.hpp"
#include "gl_macros.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

using namespace std;
//using namespace glm;



using namespace lynda;
using namespace std;

const char* vert = GLSL(120,
    attribute vec4 position;
    attribute vec2 textureCoordinate;              //<-- Texture Coordinate Attribute

    uniform mat4 model;
    uniform mat4 view;                 //<-- 4x4 Transformation Matrices
    uniform mat4 projection;

    varying vec2 texCoord;                         //<-- To be passed to fragment shader

    void main(void) {
        texCoord = textureCoordinate;

        gl_Position = projection * view * model * position;
    }

);

const char* frag = GLSL(120,

    uniform sampler2D texture;                       //<-- The texture itself

    varying vec2 texCoord;                           //<-- coordinate passed in from vertex shader

    void main(void) {
        gl_FragColor = texture2D(texture, texCoord); //<-- look up the coordinate's value
    }

);

struct vec2 {
    vec2(float _x = 0, float _y = 0) : x(_x), y(_y) {}
    float x, y;
};

struct vec4 {
    vec4(float _x = 0, float _y = 0, float _z = 0, float _w = 0) : x(_x), y(_y), z(_z), w(_w) {}
    float x, y, z, w;
};


struct Vertex {
    vec2 position;
    vec2 textureCoordinate;
};


struct MyApp : App {

    int th, tw;

    Shader* shader;
    int keyCode;                          //<-- to store last key pressed
    GLuint tID;
    GLuint arrayID;
    GLuint bufferID;
    GLuint positionID;
    GLuint textureCoordinateID;
    GLuint samplerID;

    GLuint modelID;
    GLuint viewID;
    GLuint projectionID;


    MyApp() : App() { init(); }

    void init() {

        /*-----------------------------------------------------------------------------
         *  A slab is just a rectangle with texture coordinates
         *-----------------------------------------------------------------------------*/
         //                  position      texture coord
        Vertex slab[] = {
                          {vec2(-.4,-.4), vec2(0,0)}, //bottom-left
                          {vec2(-.4, .4), vec2(0,0.5)}, //top-left
                          {vec2(.4, .4), vec2(.5,.5)}, //top-right
                          {vec2(.4,-.4), vec2(.5,0)}  //bottom-right
        };


        /*-----------------------------------------------------------------------------
         *  Make some rgba data (can also load a file here)
         *-----------------------------------------------------------------------------*/
        tw = 40;
        th = 40;
        vector<vec4> data;

        bool checker = false;
        for (int i = 0; i < tw; ++i) {
            float tu = (float)i / tw;
            for (int j = 0; j < th; ++j) {
                float tv = (float)j / th;
                data.push_back(vec4(tu, 0, tv, checker));
                checker = !checker;
            }
            checker = !checker;
        }

        /*-----------------------------------------------------------------------------
         *  Create Shader
         *-----------------------------------------------------------------------------*/
        shader = new Shader(vert, frag);

        /*-----------------------------------------------------------------------------
         *  Get Attribute Locations
         *-----------------------------------------------------------------------------*/
        positionID = glGetAttribLocation(shader->id(), "position");
        textureCoordinateID = glGetAttribLocation(shader->id(), "textureCoordinate");

        modelID = glGetUniformLocation(shader->id(), "model");
        viewID = glGetUniformLocation(shader->id(), "view");
        projectionID = glGetUniformLocation(shader->id(), "projection");

        //  samplerID = glGetUniformLocation( shader->id(), "texture" );  
        //<-- unnecessary if only using one texture    

        glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(transform));
        glUniformMatrix4fv(projectionID, 1, GL_FALSE, glm::value_ptr(transform));
        glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(transform));
        

         /*-----------------------------------------------------------------------------
          *  Generate And Bind Vertex Array Object
          *-----------------------------------------------------------------------------*/
        GENVERTEXARRAYS(1, &arrayID);
        BINDVERTEXARRAY(arrayID);

        /*-----------------------------------------------------------------------------
         *  Generate Vertex Buffer Object
         *-----------------------------------------------------------------------------*/
        glGenBuffers(1, &bufferID);
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), slab, GL_STATIC_DRAW);

        /*-----------------------------------------------------------------------------
         *  Enable Vertex Attributes and Point to them
         *-----------------------------------------------------------------------------*/
        glEnableVertexAttribArray(positionID);
        glEnableVertexAttribArray(textureCoordinateID);
        glVertexAttribPointer(positionID, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

        glVertexAttribPointer(textureCoordinateID,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void*)sizeof(vec2));

        /*-----------------------------------------------------------------------------
         *  Unbind Vertex Array Object and the Vertex Array Buffer
         *-----------------------------------------------------------------------------*/
        BINDVERTEXARRAY(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        /*-----------------------------------------------------------------------------
         *  Generate Texture and Bind it
         *-----------------------------------------------------------------------------*/
        glGenTextures(1, &tID);
        glBindTexture(GL_TEXTURE_2D, tID);

        /*-----------------------------------------------------------------------------
         *  Allocate Memory on the GPU
         *-----------------------------------------------------------------------------*/
         // target | lod | internal_format | width | height | border | format | type | data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_FLOAT, NULL);

        /*-----------------------------------------------------------------------------
         *  Load data onto GPU
         *-----------------------------------------------------------------------------*/
         // target | lod | xoffset | yoffset | width | height | format | type | data
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tw, th, GL_RGBA, GL_FLOAT, &(data[0]));

        //Mipmaps are good -- the regenerate the texture at various scales
        // and are necessary to avoid black screen if texParameters below are not set
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set these parameters to avoid a black screen
        // caused by improperly mipmapped textures
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        /*-----------------------------------------------------------------------------
         *  Unbind texture
         *-----------------------------------------------------------------------------*/
        glBindTexture(GL_TEXTURE_2D, 0);

    }

    glm::vec3 m_vector = glm::vec3(0);
    GLfloat m_speed = 0.005f;
    GLfloat m_angle = 0.0f;

    void onDraw() {

        glUseProgram(shader->id());          //<-- 1. Bind Shader
        glBindTexture(GL_TEXTURE_2D, tID);   //<-- 2. Bind Texture

        // create transformations
        if (keyCode == GLFW_KEY_KP_0) 
        {
            glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(transform));
            glUniformMatrix4fv(projectionID, 1, GL_FALSE, glm::value_ptr(transform));
            glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(transform));

        }
        else if (keyCode == GLFW_KEY_KP_1)
        {
            m_angle = (float)glfwGetTime();

            glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            transform = glm::translate(transform, m_vector);
            transform = glm::rotate(transform, m_angle, glm::vec3(0.0f, 0.0f, 1.0f));

            glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(transform));
        }
        else if (keyCode >= GLFW_KEY_RIGHT && keyCode <= GLFW_KEY_UP)
        {

            if (keyCode == GLFW_KEY_LEFT && m_vector.x > -0.5f)
                m_vector.x -= m_speed;
            else if (keyCode == GLFW_KEY_RIGHT && m_vector.x < 0.5f)
                m_vector.x += m_speed;
            else if (keyCode == GLFW_KEY_DOWN && m_vector.y > -0.5f)
                m_vector.y -= m_speed;
            else if (keyCode == GLFW_KEY_UP && m_vector.y < 0.5f)
                m_vector.y += m_speed;

            glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            transform = glm::translate(transform, m_vector);
            transform = glm::rotate(transform, m_angle, glm::vec3(0.0f, 0.0f, 1.0f));

            glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(transform));
        }

        BINDVERTEXARRAY(arrayID);              //<-- 3. Bind VAO
        glDrawArrays(GL_QUADS, 0, 4);         //<-- 4. Draw the four slab vertices
        BINDVERTEXARRAY(0);                    //<-- 5. Unbind the VAO

        glBindTexture(GL_TEXTURE_2D, 0);      //<-- 6. Unbind the texture
        glUseProgram(0);                     //<-- 7. Unbind the shader

    }

    void onKeyDown(int key, int action) {
        if (action == GLFW_PRESS) {
            keyCode = key;
            if (keyCode == GLFW_KEY_KP_0)
                cout << "Revert Transform" << endl;
            else
                cout << "Apply tranform" << endl;
        }

        if (action == GLFW_RELEASE) {
            keyCode = -1;
        }
    }
};


int main(int argc, const char* argv[]) {

    MyApp app;
    app.start();

    return 0;
}



/*-----------------------------------------------------------------------------
 *  tba...
 *-----------------------------------------------------------------------------*/
 /* tw = 40; */
 /* th = 40; */
 /* vector<vec4> data; */

 /* bool checker = false; */
 /* for (int i=0;i<tw;++i){ */
 /*   float tu = (float)i/tw; */
 /*   for (int j=0;j<th;++j){ */
 /*     float tv = (float)j/th; */
 /*     data.push_back( vec4(tu,RAND,tv,RAND) ); */
 /*     checker = !checker; */
 /*   } */
 /*   checker = !checker; */
 /* } */

   /* glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tw, th, GL_RGBA, GL_FLOAT, &(data[0]) ); */
 /* glGenerateMipmap(GL_TEXTURE_2D); */
 /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //<-- or GL_LINEAR; */

