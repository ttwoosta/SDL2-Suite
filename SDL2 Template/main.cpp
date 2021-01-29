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
private:
    sdl::Library SDL;
    Display output;
    bool running;
    gfx::Program renderer;
    gfx::Vertex::Array mesh;
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

Project::Project(int w, int h)
:   SDL {sdl::Library::Video}, 
    output {SDL, w, h}, 
    running {true},
    renderer { 
        gfx::Shader{ gfx::Shader::Vertex, file_contents("plain_vertex.glsl") }, 
        gfx::Shader{ gfx::Shader::Fragment, file_contents("plain_fragment.glsl") } 
    },
    cam{ glm::perspective(1.0f, 4.0f / 3.0f, 1.0f, 200.0f) }
{
    renderer.attributes["position"] = 0;
    renderer.attributes["color"] = 1;
    glVertexAttrib4f(1, 1, 1, 1, 1);

    cam << gfx::Vector3{ 0, 0, -5 };

    gfx::ArrayBuffer vs;
    vs.Load(vs.StaticDraw, mesh_data);

    gfx::ElementArrayBuffer es;
    es.Load(es.StaticDraw, face_data);

    mesh
        << gfx::Vertex::ArrayAttribute<gfx::Float[3]>{renderer.attributes["position"], vs}
        << es
        << gfx::Deactivate;
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
        << renderer
        << mesh
        << std::make_pair(start, count);
    output.Refresh();
}

int main(int argc, char* argv[])
{
    using namespace std;
    try {
        return Project{640, 480}(vector<string>{argv, argv + argc});
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
