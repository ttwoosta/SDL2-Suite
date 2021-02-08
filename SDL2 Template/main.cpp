//==============================================================================
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <cstdio>
#include <iostream>
#include <vector>

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

    void main() {
        dstColor = color;
        gl_Position = position;
    }
);

const char* frag = GLSL(120,
    varying vec4 dstColor;

    void main() {
        gl_FragColor = dstColor;
    }
);


/*-----------------------------------------------------------------------------
 *  FUNCION TO CHECK FOR SHADER COMPILER ERRORS
 *-----------------------------------------------------------------------------*/
void shaderCompilerCheck(GLuint ID) {
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
void shaderLinkCheck(GLuint ID) {
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

    if (linkStatus == GL_FALSE) {
        cout << "Shader Validation FAILED" << endl;
        GLchar messages[256];
        glGetProgramInfoLog(ID, sizeof(messages), 0, &messages[0]);
        cout << messages;
    }

}


/*-----------------------------------------------------------------------------
 *  CREATE A PLAIN-OLD-DATA ("POD") Container for 2D Coordinates
 *-----------------------------------------------------------------------------*/
struct vec2 {
    vec2(float _x = 0, float _y = 0) : x(_x), y(_y) {}
    float x, y;
};

/*-----------------------------------------------------------------------------
 *  CREATE A PLAIN-OLD-DATA ("POD") Container for RGBA values
 *-----------------------------------------------------------------------------*/
struct vec4 {
    vec4(float _r = 1, float _g = 1, float _b = 1, float _a = 1) : r(_r), g(_g), b(_b), a(_a) {}
    float r, g, b, a;
};

/*-----------------------------------------------------------------------------
 *  CREATE A VERTEX OBJECT
 *-----------------------------------------------------------------------------*/
struct Vertex {
    vec2 position;
    vec4 color;
};

class App {
private:
    static App Instance;

    bool Running = true;

    SDL_Window* Window = NULL;
    SDL_Renderer* Renderer = NULL;
    SDL_Surface* PrimarySurface = NULL;
    SDL_GLContext glcontext = NULL;

    static const int WindowWidth = 1024;
    static const int WindowHeight = 768;

    //A Container for Vertices
    std::vector<Vertex> triangle;

    //ID of shader
    GLuint sID;

    //ID of Vertex Attribute
    GLuint positionID, colorID;
    //A buffer ID
    GLuint arrayID, bufferID;

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

void App::SetupVertex() {

    //Specify the 3 VERTICES of A Triangle
    Vertex v1 = { vec2(-1,-.5), vec4(1,0,0,1) };
    Vertex v2 = { vec2(0,1),    vec4(0,1,0,1) };
    Vertex v3 = { vec2(1,-.5),  vec4(0,0,1,1) };
    triangle.push_back(v1);
    triangle.push_back(v2);
    triangle.push_back(v3);

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
    shaderCompilerCheck(vID);
    shaderCompilerCheck(fID);

    //5. ATTACH SHADERS TO PROGRAM
    glAttachShader(sID, vID);
    glAttachShader(sID, fID);

    //6. LINK PROGRAM
    glLinkProgram(sID);

    //7. CHECK FOR LINKING ERRORS
    shaderLinkCheck(sID);

    //8. USE PROGRAM
    glUseProgram(sID);

    positionID = glGetAttribLocation(sID, "position");
    colorID = glGetAttribLocation(sID, "color");

    glUseProgram(0);

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
    // Bind Array Buffer 
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    // Send data over buffer to GPU
    glBufferData(GL_ARRAY_BUFFER, triangle.size() * sizeof(Vertex), &(triangle[0]), GL_STATIC_DRAW);


    /*-----------------------------------------------------------------------------
     *  ENABLE VERTEX ATTRIBUTES
     *-----------------------------------------------------------------------------*/
     // Enable Position Attribute
    glEnableVertexAttribArray(positionID);
    // Enable Color Attribute
    glEnableVertexAttribArray(colorID);

    // Tell OpenGL how to handle the buffer of data that is already on the GPU
    glVertexAttribPointer(positionID, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec2));

    BINDVERTEXARRAY(0);
}

//------------------------------------------------------------------------------
void App::Loop() {
}

//------------------------------------------------------------------------------
void App::Render() {
    glUseProgram(sID);
    BINDVERTEXARRAY(arrayID);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    BINDVERTEXARRAY(0);
    glUseProgram(0);
}

//------------------------------------------------------------------------------
void App::Cleanup() {
    if (Renderer) {
        SDL_DestroyRenderer(Renderer);
        Renderer = NULL;
    }

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

    while (Running) {
        while (SDL_PollEvent(&Event) != 0) {
            OnEvent(&Event);

            if (Event.type == SDL_QUIT) Running = false;
        }

        // Set viewport size and position every frame of animation
        glViewport(0, 0, WindowWidth, WindowHeight);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

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

    std::cout << "Hello World!\n";
    return App::GetInstance()->Execute(argc, argv);
}