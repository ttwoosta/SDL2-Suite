//
//  main.cpp
//  gfxLite
//
//  Created by Nevin Flanagan on 12/21/20.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>

using namespace std::string_literals;

std::string file_contents(std::string filename)
{
    using namespace std;
    ifstream in{ filename, ios::in | ios::binary };
    if (in) {
        ostringstream contents;
        contents << in.rdbuf();
        return contents.str();
    }
    throw (errno);
}

#include <SDL2/SDL.h>
#include "glm/gtc/matrix_transform.hpp"

#include "Display.h"
#include "GL/Shader.h"
#include "GL/Vertex.h"
#include "GL/Camera.h"

namespace gfx = gl;

class Project {
public:
    Project(int w, int h);
    int operator()(const std::vector<std::string>& args);
protected:
    using seconds = std::chrono::duration<float>;
    void update(seconds frame, seconds total);
    void render() const;
    void getGLVersion() const;
private:
    sdl::Library SDL;
    Display output;
    bool running;
    gfx::Program renderer;
    gfx::Vertex::Array mesh;
    gfx::Vertex::Array mesh2;
    gfx::Camera cam;
};

std::vector<gfx::Float> mesh_data = {
     1,  0,  0,
     0,  1,  0,
     0,  0,  1,
    -1,  0,  0,
     0, -1,  0,
     0,  0, -1,
};

std::vector<gfx::Ushort> face_data = {
     0, 2, 1,
     0, 1, 5,
     0, 4, 2,
     0, 5, 4, 
     3, 1, 2, 
     3, 5, 1, 
     3, 2, 4,
     3, 4, 5,
};

std::vector<gfx::Float> triangle_mesh_data = {
     1,  0,  0,
     0,  1,  0,
     0,  0,  1,
    -1,  0,  0,
     0, -1,  0,
     0,  0, -1,
};

Project::Project(int w, int h)
:   SDL {sdl::Library::Video}, 
    output {SDL, w, h}, 
    running {true},
    renderer { 
        // using default 2 shader vFlat and fFlat
        gfx::Shader{ gfx::Shader::Vertex, shader::vFlat }, 
        gfx::Shader{ gfx::Shader::Fragment, shader::fFlat } 
    },
    cam{ glm::perspective(1.0f, 4.0f / 3.0f, 1.0f, 200.0f) }
{
    // Program setup
    // connect the position to channel 2 
    renderer.attributes["position"] = 2;

    // setup the color to 1
    renderer.attributes["color"] = 1;

    // specify the value of a generic vertex attribute
    // https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glVertexAttrib.xml
    glVertexAttrib4f(1, 1, 1, 1, 1);

    // camera setup
    // move the camera to a position (0,0,-5) 
    // (translate the camera to -5 on Z-axis that back the camera out)
    // move to the right and up 1 point
    cam << gfx::Vector3{ 1, 1, -5 };

    

    // copy mesh_data to Array buffer
    // using StaticDraw option to tell openGL how to store the data
    gfx::ArrayBuffer vertices;
    vertices.Load(vertices.StaticDraw, mesh_data);

    // copy face_data to Element Array buffer
    gfx::ElementArrayBuffer edges;
    edges.Load(edges.StaticDraw, face_data);

    // Linking 2 buffers to Vertex array "mesh"
    mesh
        << gfx::Vertex::ArrayAttribute<gfx::Float[3]>{renderer.attributes["position"], vertices}
        << edges
        << gfx::Deactivate;

    // Linking 2 buffers to Vertex array "mesh"
    mesh2
        << gfx::Vertex::ArrayAttribute<gfx::Float[3]>{5, vertices}
        << edges
        << gfx::Deactivate;

    this->getGLVersion();
}

int Project::operator()(const std::vector<std::string>& args)
{
    using std::chrono::duration_cast;

    sdl::Ticks previous = SDL.GetTicks();
    while (running) {
        sdl::Ticks now = SDL.GetTicks();
        update(duration_cast<seconds>(now - previous), duration_cast<seconds>(now));
        previous = now;
        render();
    }
    return 0;
}

void Project::update(seconds frame, seconds total)
{
    std::unordered_map<sdl::EventType, std::function<void(const sdl::Event&)>> responses;
    responses.emplace(sdl::EventType::Quit, [this](const sdl::Event&) { running = false; });
    SDL.ProcessEvents(responses);

}

void Project::render() const
{
    constexpr gfx::Ushort start = 0;
    const gfx::Size count = face_data.size();

    output.Clear();
    cam
        << renderer // this is the program to use to draw 
        << mesh     // draw instruction
        << std::make_pair(start, count); // which peice out of the element array

    cam
        << renderer // this is the program to use to draw 
        << mesh2     // draw instruction
        << std::make_pair(start, count); // which peice out of the element array

    output.Refresh();


}

void Project::getGLVersion() const 
{
    using namespace std;

    const GLubyte* p = glGetString(GL_VERSION);
    std::cout << "OpenGL Version: " << p << std::endl;

    const GLubyte* q = glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::cout << "GLSL Version: " << q << std::endl;

    if (GLEW_ARB_vertex_array_object) {
        printf("genVertexArrays supported\n");
    }
    else {
        printf("genVertexArrays not supported\n");
    }

    if (GLEW_APPLE_vertex_array_object) {
        printf("genVertexArraysAPPLE supported\n");
    }
    else {
        printf("genVertexArraysAPPLE not supported\n");
    }
}

int main(int argc, char* argv[])
{
    using namespace std;
    try {

        vector<string> param = vector<string>{ argv, argv + argc };
        return Project{640, 480}(param);

    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
