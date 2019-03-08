#include "renderer.h"

#include <malloc.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <remotegl/remotegl.h>

#include "util/esUtil.h"
#include "util/timespec.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static const GLfloat cube_vertices[] =
{
        // front
        -1.0f, -1.0f, +1.0f,
        +1.0f, -1.0f, +1.0f,
        -1.0f, +1.0f, +1.0f,
        +1.0f, +1.0f, +1.0f,
        // back
        +1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        +1.0f, +1.0f, -1.0f,
        -1.0f, +1.0f, -1.0f,
        // right
        +1.0f, -1.0f, +1.0f,
        +1.0f, -1.0f, -1.0f,
        +1.0f, +1.0f, +1.0f,
        +1.0f, +1.0f, -1.0f,
        // left
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, +1.0f,
        -1.0f, +1.0f, -1.0f,
        -1.0f, +1.0f, +1.0f,
        // top
        -1.0f, +1.0f, +1.0f,
        +1.0f, +1.0f, +1.0f,
        -1.0f, +1.0f, -1.0f,
        +1.0f, +1.0f, -1.0f,
        // bottom
        -1.0f, -1.0f, -1.0f,
        +1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, +1.0f,
        +1.0f, -1.0f, +1.0f,
};

static const GLfloat cube_colors[] =
{
        // front
        0.0f,  0.0f,  1.0f, // blue
        1.0f,  0.0f,  1.0f, // magenta
        0.0f,  1.0f,  1.0f, // cyan
        1.0f,  1.0f,  1.0f, // white
        // back
        1.0f,  0.0f,  0.0f, // red
        0.0f,  0.0f,  0.0f, // black
        1.0f,  1.0f,  0.0f, // yellow
        0.0f,  1.0f,  0.0f, // green
        // right
        1.0f,  0.0f,  1.0f, // magenta
        1.0f,  0.0f,  0.0f, // red
        1.0f,  1.0f,  1.0f, // white
        1.0f,  1.0f,  0.0f, // yellow
        // left
        0.0f,  0.0f,  0.0f, // black
        0.0f,  0.0f,  1.0f, // blue
        0.0f,  1.0f,  0.0f, // green
        0.0f,  1.0f,  1.0f, // cyan
        // top
        0.0f,  1.0f,  1.0f, // cyan
        1.0f,  1.0f,  1.0f, // white
        0.0f,  1.0f,  0.0f, // green
        1.0f,  1.0f,  0.0f, // yellow
        // bottom
        0.0f,  0.0f,  0.0f, // black
        1.0f,  0.0f,  0.0f, // red
        0.0f,  0.0f,  1.0f, // blue
        1.0f,  0.0f,  1.0f  // magenta
};

static const GLfloat cube_normals[] =
{
        // front
        +0.0f, +0.0f, +1.0f, // forward
        +0.0f, +0.0f, +1.0f, // forward
        +0.0f, +0.0f, +1.0f, // forward
        +0.0f, +0.0f, +1.0f, // forward
        // back
        +0.0f, +0.0f, -1.0f, // backward
        +0.0f, +0.0f, -1.0f, // backward
        +0.0f, +0.0f, -1.0f, // backward
        +0.0f, +0.0f, -1.0f, // backward
        // right
        +1.0f, +0.0f, +0.0f, // right
        +1.0f, +0.0f, +0.0f, // right
        +1.0f, +0.0f, +0.0f, // right
        +1.0f, +0.0f, +0.0f, // right
        // left
        -1.0f, +0.0f, +0.0f, // left
        -1.0f, +0.0f, +0.0f, // left
        -1.0f, +0.0f, +0.0f, // left
        -1.0f, +0.0f, +0.0f, // left
        // top
        +0.0f, +1.0f, +0.0f, // up
        +0.0f, +1.0f, +0.0f, // up
        +0.0f, +1.0f, +0.0f, // up
        +0.0f, +1.0f, +0.0f, // up
        // bottom
        +0.0f, -1.0f, +0.0f, // down
        +0.0f, -1.0f, +0.0f, // down
        +0.0f, -1.0f, +0.0f, // down
        +0.0f, -1.0f, +0.0f  // down
};

static const char *vertex_shader_source =
    "#version 100                       \n"
    "uniform mat4 modelviewMatrix;      \n"
    "uniform mat4 modelviewprojectionMatrix;\n"
    "uniform mat3 normalMatrix;         \n"
    "                                   \n"
    "attribute vec4 in_position;        \n"
    "attribute vec3 in_normal;          \n"
    "attribute vec4 in_color;           \n"
    "\n"
    "vec4 lightSource = vec4(2.0, 2.0, 20.0, 0.0);\n"
    "                                   \n"
    "varying vec4 vVaryingColor;        \n"
    "                                   \n"
    "void main()                        \n"
    "{                                  \n"
    "    gl_Position = modelviewprojectionMatrix * in_position;\n"
    "    vec3 vEyeNormal = normalMatrix * in_normal;\n"
    "    vec4 vPosition4 = modelviewMatrix * in_position;\n"
    "    vec3 vPosition3 = vPosition4.xyz / vPosition4.w;\n"
    "    vec3 vLightDir = normalize(lightSource.xyz - vPosition3);\n"
    "    float diff = max(0.0, dot(vEyeNormal, vLightDir));\n"
    "    vVaryingColor = vec4(diff * in_color.rgb, 1.0);\n"
    "}                                  \n";

static const char *fragment_shader_source =
    "#version 100                       \n"
    "precision mediump float;           \n"
    "                                   \n"
    "varying vec4 vVaryingColor;        \n"
    "                                   \n"
    "void main()                        \n"
    "{                                  \n"
    "    gl_FragColor = vVaryingColor;  \n"
    "}                                  \n";

struct renderer
{
    struct
    {
        int id;
        int width;
        int height;
    } window;

    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;

    GLuint vbo;
    GLint modelviewmatrix;
    GLint modelviewprojectionmatrix;
    GLint normalmatrix;
    GLuint positionsoffset;
    GLuint colorsoffset;
    GLuint normalsoffset;

    struct timespec previoustime;
    uint64_t frameid;
};

static GLuint compile_shader(const char *source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int ret = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);
    if (!ret)
    {
        // TODO: needs a logging system
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &ret);
        if (ret > 1)
        {
            char* log = calloc(ret+1, sizeof(char));
            glGetShaderInfoLog(shader, ret, NULL, log);
            fprintf(stderr, "Error: failed to compile shader with message: %s\n", log);
            free(log);
        }
        else
        {
            fprintf(stderr, "Error: failed to compile shader\n");
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static GLuint link_program(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    int ret = 0;
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &ret);
    if (!ret)
    {
        // TODO: nmeeds a logging system
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &ret);
        if (ret > 1)
        {
            char* log = calloc(ret+1, sizeof(char));
            glGetProgramInfoLog(program, ret, NULL, log);
            fprintf(stderr, "Error: failed to link program with message: %s\n", log);
            free(log);
        }
        else
        {
            fprintf(stderr, "Error: failed to link program\n");
        }

        glDeleteProgram(program);
        return 0;
    }

    return program;
}

struct renderer* renderer_create(int windowId, int width, int height)
{
    struct renderer* renderer = calloc(1, sizeof(struct renderer));

    renderer->window.id = windowId;
    renderer->window.width = width;
    renderer->window.height = height;

    printf("GL Extensions: \"%s\"\n", glGetString(GL_EXTENSIONS));

    rglSwapInterval(windowId, 1);

    renderer->vertex_shader = compile_shader(vertex_shader_source, GL_VERTEX_SHADER);
    renderer->fragment_shader = compile_shader(fragment_shader_source, GL_FRAGMENT_SHADER);
    if (!renderer->vertex_shader || !renderer->fragment_shader)
    {
        fprintf(stderr, "Error: Failed to compile shaders\n");
        renderer_release(renderer);
        return NULL;
    }

    renderer->program = link_program(renderer->vertex_shader, renderer->fragment_shader);
    if (!renderer->program)
    {
        fprintf(stderr, "Error: Failed to link program\n");
        renderer_release(renderer);
        return NULL;
    }

    glUseProgram(renderer->program);

    renderer->modelviewmatrix = glGetUniformLocation(renderer->program, "modelviewMatrix");
    renderer->modelviewprojectionmatrix = glGetUniformLocation(renderer->program, "modelviewprojectionMatrix");
    renderer->normalmatrix = glGetUniformLocation(renderer->program, "normalMatrix");

    renderer->positionsoffset = 0;
    renderer->colorsoffset = sizeof(cube_vertices);
    renderer->normalsoffset = sizeof(cube_vertices) + sizeof(cube_colors);

    glGenBuffers(1, &renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices) + sizeof(cube_colors) + sizeof(cube_normals), 0, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, renderer->positionsoffset, sizeof(cube_vertices), &cube_vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, renderer->colorsoffset, sizeof(cube_colors), &cube_colors[0]);
    glBufferSubData(GL_ARRAY_BUFFER, renderer->normalsoffset, sizeof(cube_normals), &cube_normals[0]);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)renderer->positionsoffset);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)renderer->normalsoffset);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)renderer->colorsoffset);
    glEnableVertexAttribArray(2);

    return renderer;
}

void renderer_release(struct renderer* renderer)
{
    assert(renderer != NULL);
    free(renderer);
}

void renderer_draw(struct renderer* renderer)
{
    assert(renderer != NULL);

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    struct timespec delta = timespec_diff(renderer->previoustime, now);
    renderer->previoustime = now;

    printf("Frame %lu in %f\n", renderer->frameid++, timespec_to_float(delta));
    ESMatrix modelview;

    // clear the color buffer
    glViewport(0, 0, renderer->window.width, renderer->window.height);
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    esMatrixLoadIdentity(&modelview);
    esTranslate(&modelview, 0.0f, 0.0f, -8.0f);
    esRotate(&modelview, 45.0f + (0.25f * renderer->frameid), 1.0f, 0.0f, 0.0f);
    esRotate(&modelview, 45.0f - (0.5f * renderer->frameid), 0.0f, 1.0f, 0.0f);
    esRotate(&modelview, 10.0f + (0.15f * renderer->frameid), 0.0f, 0.0f, 1.0f);

    GLfloat aspect = (GLfloat)(renderer->window.height) / (GLfloat)(renderer->window.width);

    ESMatrix projection;
    esMatrixLoadIdentity(&projection);
    esFrustum(&projection, -2.8f, +2.8f, -2.8f * aspect, +2.8f * aspect, 6.0f, 10.0f);

    ESMatrix modelviewprojection;
    esMatrixLoadIdentity(&modelviewprojection);
    esMatrixMultiply(&modelviewprojection, &modelview, &projection);

    float normal[9];
    normal[0] = modelview.m[0][0];
    normal[1] = modelview.m[0][1];
    normal[2] = modelview.m[0][2];
    normal[3] = modelview.m[1][0];
    normal[4] = modelview.m[1][1];
    normal[5] = modelview.m[1][2];
    normal[6] = modelview.m[2][0];
    normal[7] = modelview.m[2][1];
    normal[8] = modelview.m[2][2];

    glUniformMatrix4fv(renderer->modelviewmatrix, 1, GL_FALSE, &modelview.m[0][0]);
    glUniformMatrix4fv(renderer->modelviewprojectionmatrix, 1, GL_FALSE, &modelviewprojection.m[0][0]);
    glUniformMatrix3fv(renderer->normalmatrix, 1, GL_FALSE, normal);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
    
    //sleep(1);
}

void renderer_swap(struct renderer* renderer)
{
    rglSwapBuffers(renderer->window.id);
}
