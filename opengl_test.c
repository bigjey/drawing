#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glad_gl.c>

#define WIDTH 600
#define HEIGHT 600
#define SEEDS_COUNT 10
#define SEED_RENDER_RADIUS 4

#define COLOR_WHITE     0xFFFFFFFF
#define COLOR_RED       0xFF0000FF
#define COLOR_BLUE      0xFFFF0000
#define COLOR_BLACK     0xFF000000

#define COLOR_PALLETE_1 0xFF00FF00
#define COLOR_PALLETE_2 0xFF00FF87
#define COLOR_PALLETE_3 0xFF00FFD3
#define COLOR_PALLETE_4 0xFF0ADEFF
#define COLOR_PALLETE_5 0xFF0AA1FF
#define COLOR_PALLETE_6 0xFF990AFF
#define COLOR_PALLETE_7 0xFFFF0AEF
#define COLOR_PALLETE_8 0xFFF5147D
#define COLOR_PALLETE_9 0xFFFF580A
#define COLOR_PALLETE_10 0xFFFFBE0A

#define M_PI           3.14159265358979323846

typedef uint32_t u32;
typedef uint8_t u8;

typedef struct {
    float x, y;
} vec2;

typedef struct {
    float x, y, z, w;
} vec4;

u32 PALETTE[] = {
    COLOR_PALLETE_1,
    COLOR_PALLETE_2,
    COLOR_PALLETE_3,
    COLOR_PALLETE_4,
    COLOR_PALLETE_5,
    COLOR_PALLETE_6,
    COLOR_PALLETE_7,
    COLOR_PALLETE_8,
    COLOR_PALLETE_9,
    COLOR_PALLETE_10,
};

#define PALETTE_SIZE (sizeof(PALETTE)/sizeof(PALETTE[0]))

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static const char* vertex_shader_text =
"#version 330\n"
"precision mediump float;\n"

"layout(location = 0) in vec3 pos;\n"

"out vec2 uv;\n"
"void main(void)\n"
"{\n"
"    gl_Position = vec4(pos, 1.0);\n"
"    uv = pos.xy / 2.0 + 0.5;\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 330\n"

"in vec2 uv;\n"

"uniform vec3 u_color;\n"
"uniform vec2 u_seed_pos;\n"

"void main()\n"
"{\n"
"    float d = length(uv - u_seed_pos) / sqrt(2);\n"
"    if (d < 0.004) {\n"
"    gl_FragDepth = 0.0;\n"
"    gl_FragColor = vec4(u_color, 1.0);\n"
"    } else if (d < 0.008) {\n"
"    gl_FragDepth = 0.0;\n"
"    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
"    } else {\n"
"    gl_FragDepth = d;\n"
"    gl_FragColor = vec4(u_color, 1.0);\n"
"    };\n"
"}\n";

vec2 seed_positions[SEEDS_COUNT];
vec4 seed_colors[SEEDS_COUNT];
vec2 seed_velocities[SEEDS_COUNT];

void create_seeds(void) {
    for (int i = 0; i < SEEDS_COUNT; i++) {
        seed_positions[i].x = (rand() % WIDTH) / (float) WIDTH;
        seed_positions[i].y = rand() % HEIGHT / (float) HEIGHT;

        u32 color = PALETTE[i % PALETTE_SIZE];
        seed_colors[i].x = (color & 0x0000ff) / 255.0;
        seed_colors[i].y = ((color & 0x00ff00) >> 8) / 255.0;
        seed_colors[i].z = ((color & 0xff0000) >> 16) / 255.0;
        seed_colors[i].w = 1.0;

        int angle = rand() % 360;
        seed_velocities[i].x = cosf(angle);
        seed_velocities[i].y = sinf(angle);
    }
}

int main(void) {
    srand(time(0));
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (glfwInit() != GLFW_TRUE)
    {
        printf("GL Init Failed\n");
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(WIDTH, HEIGHT, "C + OpenGL = Fun?", NULL, NULL);
    if (!window)
    {
        printf("Window Init Failed\n");
        exit(1);
    }


    int gl_ver_major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    int gl_ver_minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    printf("OpenGL %d.%d\n", gl_ver_major, gl_ver_minor);

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        printf("gladLoadGL Failed\n");
        exit(1);
    }

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
 
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
 
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,

        -1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
    };

    create_seeds();

    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    glUseProgram(program);            

    GLint u_color = glGetUniformLocation(program, "u_color");
    // assert(u_color != -1);
    
    GLint u_seed_pos = glGetUniformLocation(program, "u_seed_pos");
    // assert(u_seed_pos != -1);

    while (!glfwWindowShouldClose(window))
    {        
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);  
        
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        for (int i = 0; i < SEEDS_COUNT; i++) {
            glUniform3f(u_color, seed_colors[i].x, seed_colors[i].y, seed_colors[i].z);
            glUniform2f(u_seed_pos, seed_positions[i].x, seed_positions[i].y);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            seed_positions[i].x += 0.003 * seed_velocities[i].x;
            if (seed_positions[i].x <= 0.0 || seed_positions[i].x >= 1.0) {
                seed_velocities[i].x = -seed_velocities[i].x;
            }
            seed_positions[i].y += 0.003 * seed_velocities[i].y;
            if (seed_positions[i].y <= 0.0 || seed_positions[i].y >= 1.0) {
                seed_velocities[i].y = -seed_velocities[i].y;
            }
        }

        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);    
}