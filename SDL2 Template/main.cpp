/*
* Author: Tu Tong
* Date: Feb 10, 2021
* Class: Game Programming
* Project created by Nevin Flanagan on 12/22/20.
*
* Project base on http://www.sdltutorials.com/
* https://github.com/MetaCipher/sdl-2.0-textures
*
* The original draw source code from the course Learning OpenGL
* https://www.linkedin.com/learning/learning-opengl?u=67554802
*/

//==============================================================================
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/*-----------------------------------------------------------------------------
 *  MACRO
 *-----------------------------------------------------------------------------*/
#ifndef GLSL
#define GLSL(version, A) "#version " #version "\n" #A
#endif 

#ifndef GENVERTEXARRAYS
#define GENVERTEXARRAYS(n,id) if(GLEW_APPLE_vertex_array_object)glGenVertexArraysAPPLE(1,id);\
	else if (GLEW_ARB_vertex_array_object) glGenVertexArrays(n,id)
#endif

#ifndef BINDVERTEXARRAY
#define BINDVERTEXARRAY(id) if(GLEW_APPLE_vertex_array_object)glBindVertexArrayAPPLE(id);\
	else if (GLEW_ARB_vertex_array_object) glBindVertexArray(id)
#endif

 /*-----------------------------------------------------------------------------
  *  SHADER CODE
  *-----------------------------------------------------------------------------*/
const char* vert = GLSL(120,

    attribute vec4 position;
    attribute vec4 color;

    varying vec4 dstColor;

    uniform mat4 model;
    uniform mat4 view;                 //<-- 4x4 Transformation Matrices
    uniform mat4 projection;

    void main() {
        dstColor = color;
        gl_Position = projection * view * model * position;   //<-- Apply transformation 
    }

);

const char* frag = GLSL(120,

    varying vec4 dstColor;

    void main() {
        gl_FragColor = dstColor;
    }

);

class Shader {

    GLuint sID;

public:

    GLuint id() const { return sID; }

    Shader(const char* vert, const char* frag) {

        /*-----------------------------------------------------------------------------
         *  CREATE THE SHADER
         *-----------------------------------------------------------------------------*/

         //1. CREATE SHADER PROGRAM
        sID = glCreateProgram();
        GLuint vID = glCreateShader(GL_VERTEX_SHADER);
        GLuint fID = glCreateShader(GL_FRAGMENT_SHADER);

        //2. LOAD SHADER SOURCE CODE
        glShaderSource(vID, 1, &vert, NULL); //<-- Last argument specifies length of source string
        glShaderSource(fID, 1, &frag, NULL);

        //3. COMPILE
        glCompileShader(vID);
        glCompileShader(fID);

        //4. CHECK FOR COMPILE ERRORS
        compilerCheck(vID);
        compilerCheck(fID);

        //5. ATTACH SHADERS TO PROGRAM
        glAttachShader(sID, vID);
        glAttachShader(sID, fID);

        //6. LINK PROGRAM
        glLinkProgram(sID);

        //7. CHECK FOR LINKING ERRORS
        linkCheck(sID);

        //8. USE PROGRAM
        glUseProgram(sID);
    }

    void bind() { glUseProgram(sID); }
    void unbind() { glUseProgram(0); }


    /*-----------------------------------------------------------------------------
     *  FUNCION TO CHECK FOR SHADER COMPILER ERRORS
     *-----------------------------------------------------------------------------*/
    void compilerCheck(GLuint ID) {
        GLint comp;
        glGetShaderiv(ID, GL_COMPILE_STATUS, &comp);

        using namespace std;

        if (comp == GL_FALSE) {
            cout << "Shader Compilation FAILED" << endl;
            GLchar messages[256];
            glGetShaderInfoLog(ID, sizeof(messages), 0, &messages[0]);
            cout << messages;
        }
    }


    /*-----------------------------------------------------------------------------
     *  FUNCION TO CHECK FOR SHADER LINK ERRORS
     *-----------------------------------------------------------------------------*/
    void linkCheck(GLuint ID) {
        GLint linkStatus, validateStatus;
        glGetProgramiv(ID, GL_LINK_STATUS, &linkStatus);

        using namespace std;

        if (linkStatus == GL_FALSE) {
            cout << "Shader Linking FAILED" << endl;
            GLchar messages[256];
            glGetProgramInfoLog(ID, sizeof(messages), 0, &messages[0]);
            cout << messages;
        }

        glValidateProgram(ID);
        glGetProgramiv(ID, GL_VALIDATE_STATUS, &validateStatus);

        cout << "Link: " << linkStatus << "  Validate: " << validateStatus << endl;
        if (linkStatus == GL_FALSE) {
            cout << "Shader Validation FAILED" << endl;
            GLchar messages[256];
            glGetProgramInfoLog(ID, sizeof(messages), 0, &messages[0]);
            cout << messages;
        }

    }



};

/*-----------------------------------------------------------------------------
 *  CREATE A VERTEX OBJECT
 *-----------------------------------------------------------------------------*/
struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
};

class App {
private:
    static App Instance;

    bool Running = true;

    SDL_Window* Window = NULL;
    SDL_Surface* PrimarySurface = NULL;
    SDL_GLContext glcontext = NULL;

    static const int WindowWidth = 1024;
    static const int WindowHeight = 768;

    //A Container for Vertices
    std::vector<Vertex> triangle;

    Shader* shader;

    //ID of Vertex Attribute
    GLuint positionID, normalID, colorID;

    //A buffer ID
    GLuint bufferID, elementID;                   //<-- add an elementID
    //An array ID
    GLuint arrayID;

    //ID of Uniforms
    GLuint modelID, viewID, projectionID;

private:
    App();

    // Capture SDL Events
    void OnEvent(SDL_Event* Event);

    // Initialize our SDL game / app
    bool Init();

    // Logic loop
    void Loop();

    // Render loop (draw)
    void Render();

    // Free up resources
    void Cleanup();

    // Keyboard event handler
    void KeyboardKeyDown(SDL_KeyboardEvent kEvent);

public:
    int Execute(int argc, char* argv[]);
    void SetupVertex();

public:
    static App* GetInstance();

    static int GetWindowWidth();
    static int GetWindowHeight();
    static void getGLVersion();
};

App App::Instance;

//==============================================================================
App::App() {
}

//------------------------------------------------------------------------------
void App::OnEvent(SDL_Event* Event) {
}

void App::getGLVersion()
{
    const GLubyte* p = glGetString(GL_VERSION);
    printf("OpenGL Version: %s\n", p);

    const GLubyte* q = glGetString(GL_SHADING_LANGUAGE_VERSION);
    printf("GLSL Version: %s\n", q);

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


//------------------------------------------------------------------------------
bool App::Init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Unable to Init SDL: %s", SDL_GetError());
        return false;
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        printf("Unable to Init hinting: %s", SDL_GetError());
    }

    if ((Window = SDL_CreateWindow(
        "My SDL Game",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WindowWidth, WindowHeight, SDL_WINDOW_OPENGL)
        ) == NULL) {
        printf("Unable to create SDL Window: %s", SDL_GetError());
        return false;
    }

    PrimarySurface = SDL_GetWindowSurface(Window);

    // Create an OpenGL context associated with the window.
    // https://wiki.libsdl.org/SDL_GL_CreateContext
    glcontext = SDL_GL_CreateContext(Window);

    return true;
}

static const int kNumVertecies = 36;

void App::SetupVertex() {

    using namespace glm;

    //Specify the 8 VERTICES of A Cube
    Vertex cube[] = {
        {glm::vec3(1, -1,  1), glm::vec4(1,0,0,1)},  // red
        {glm::vec3(1,  1,  1), glm::vec4(0,1,0,1)},  // green
        {glm::vec3(-1,  1,  1), glm::vec4(0,0,1,1)}, // blue 
        {glm::vec3(-1, -1,  1), glm::vec4(0,1,1,1)}, // cyan

        {glm::vec3(1, -1, -1), glm::vec4(1,0,1,1)},  // magenta
        {glm::vec3(1,  1, -1), glm::vec4(1,1,0,1)},  // yellow
        {glm::vec3(-1,  1, -1), glm::vec4(1,1,1,1)}, // white
        {glm::vec3(-1, -1, -1), glm::vec4(0,0,0,1)}  // black
    };


    //6-------------/5
  //  .           // |
//2--------------1   |
//    .          |   |
//    .          |   |
//    .          |   |
//    .          |   |
//    7.......   |   /4
//               | //
//3--------------/0    

    GLubyte indices_triangluate[kNumVertecies] = {
        0, 1, 2, // front
        0, 3, 2,
        
        4, 5, 6, // back
        4, 7, 6,

        3, 2, 6, // left
        3, 7, 6,

        0, 1, 5, // right
        0, 4, 5,

        1, 2, 6, // top
        1, 5, 6,

        0, 3, 7, // bottom
        0, 4, 7,
    };
    

    /*-----------------------------------------------------------------------------
     *  CREATE THE SHADER
     *-----------------------------------------------------------------------------*/

    shader = new Shader(vert, frag);

    // With Shader bound, get attribute and uniform locations:

    // Get attribute locations
    positionID = glGetAttribLocation(shader->id(), "position");
    colorID = glGetAttribLocation(shader->id(), "color");

    // Get uniform locations
    modelID = glGetUniformLocation(shader->id(), "model");
    viewID = glGetUniformLocation(shader->id(), "view");
    projectionID = glGetUniformLocation(shader->id(), "projection");


    /*-----------------------------------------------------------------------------
     *  CREATE THE VERTEX ARRAY OBJECT
     *-----------------------------------------------------------------------------*/
    GENVERTEXARRAYS(1, &arrayID);
    BINDVERTEXARRAY(arrayID);

    /*-----------------------------------------------------------------------------
     *  CREATE THE VERTEX BUFFER OBJECT
     *-----------------------------------------------------------------------------*/
     // Generate one buffer
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vertex), cube, GL_STATIC_DRAW);


    /*-----------------------------------------------------------------------------
    *  CREATE THE ELEMENT ARRAY BUFFER OBJECT
    *-----------------------------------------------------------------------------*/
    glGenBuffers(1, &elementID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, kNumVertecies * sizeof(GLubyte), indices_triangluate, GL_STATIC_DRAW);


    /*-----------------------------------------------------------------------------
     *  ENABLE VERTEX ATTRIBUTES
     *-----------------------------------------------------------------------------*/
    glEnableVertexAttribArray(positionID);
    glEnableVertexAttribArray(colorID);

    // Tell OpenGL how to handle the buffer of data that is already on the GPU
    //                      attrib    num   type     normalize   stride     offset
    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));

    // Unbind Everything (NOTE: unbind the vertex array object first)
    BINDVERTEXARRAY(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//------------------------------------------------------------------------------
void App::Loop() {
}

glm::vec3 m_vector = glm::vec3(0);
bool spinning = false;
SDL_Keycode keySym = SDLK_0;

static float m_time = 0.0;

//------------------------------------------------------------------------------
void App::Render() {
    m_time += .01;

    BINDVERTEXARRAY(arrayID);

    using namespace glm;

    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
    // https://glm.g-truc.net/0.9.9/api/a00665.html#ga747c8cf99458663dd7ad1bb3a2f07787
    glm::mat4 view = glm::lookAt(glm::vec3(0, 2, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 proj = glm::perspective(3.14f / 3.f, (float)WindowWidth / WindowHeight, 0.1f, -10.f);

    // https://open.gl/transformations
    glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionID, 1, GL_FALSE, glm::value_ptr(proj));

    if (spinning)
    {
        glm::mat4 model = glm::rotate(glm::mat4(), m_time, m_vector);
        glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(model));
    }
    else {
        glm::mat4 model = glm::rotate(glm::mat4(), glm::radians(30.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(model));
    }

    glDrawElements(GL_TRIANGLES, kNumVertecies, GL_UNSIGNED_BYTE, 0);

    BINDVERTEXARRAY(0);
}

//------------------------------------------------------------------------------
void App::KeyboardKeyDown(SDL_KeyboardEvent kEvent)
{
    using namespace std;

    if (keySym == SDLK_LEFT || keySym == SDLK_RIGHT || keySym == SDLK_UP || keySym == SDLK_DOWN)
    {
        spinning = true;
        m_vector = glm::vec3(0, 0, 0);

        if (keySym == SDLK_LEFT)
            m_vector.y = -1;
        else if (keySym == SDLK_RIGHT)
            m_vector.y = 1;

        if (keySym == SDLK_DOWN)
            m_vector.x = 1;
        else if (keySym == SDLK_UP)
            m_vector.x = -1;
    }
    else {
        spinning = false;
    }
}

//------------------------------------------------------------------------------
void App::Cleanup() {

    if (Window) {
        SDL_DestroyWindow(Window);
        Window = NULL;
    }
    if (glcontext != NULL) {
        SDL_GL_DeleteContext(glcontext);
        glcontext = NULL;
    }

    SDL_Quit();
}

//------------------------------------------------------------------------------
int App::Execute(int argc, char* argv[]) {
    if (!Init()) return 0;

    SDL_Event Event;

    glewInit();
    getGLVersion();
    SetupVertex();

    using namespace std;

    while (Running) {
        while (SDL_PollEvent(&Event) != 0) {
            OnEvent(&Event);

            if (Event.type == SDL_QUIT) Running = false;

            if (Event.type == SDL_MOUSEBUTTONUP) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                //printf("Mouse up at position: (%d, %d)\n", x, y);
                printf("Time %f | rotate (%f, %f)\n", m_time, m_vector.x, m_vector.y);
            }

            if (Event.type == SDL_KEYDOWN) {
                keySym = Event.key.keysym.sym;
                KeyboardKeyDown(Event.key);
            }

        }

        // Set viewport size and position every frame of animation
        glViewport(0, 0, WindowWidth, WindowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Loop();
        Render();

        // https://wiki.libsdl.org/SDL_GL_CreateContext
        SDL_GL_SwapWindow(Window);

        SDL_Delay(1); // Breath
    }

    Cleanup();

    return 1;
}

//==============================================================================
App* App::GetInstance() { return &App::Instance; }

int App::GetWindowWidth() { return WindowWidth; }
int App::GetWindowHeight() { return WindowHeight; }

//==============================================================================

int main(int argc, char* argv[])
{
    std::cout << "Author: Tu Tong (0262620)\n";
    std::cout << "Use arrow keys to rotate the cube or\n";
    std::cout << "Press any key to reset position\n";
    return App::GetInstance()->Execute(argc, argv);
}