#include <remotegl/remotegl-server.hpp>

#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <unistd.h>
#include <sys/uio.h>

#include <GL/glew.h>

#include "remotegl-protocol.hpp"

int serverFd = -1;
RemoteGLServerImpl* mImpl;
bool run = true;
int generation = 0;

void InvokeWindowHint()
{
    struct __attribute__((packed)) {
        RGLenum hint;
        int value;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    mImpl->DoWindowHint(MSG.hint, MSG.value);
}

void InvokeWindowCreate()
{
    struct __attribute__((packed)) {
        size_t name_size;
        int width;
        int height;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    char name[MSG.name_size];
    read(serverFd, name, sizeof(name));

    int windowId = mImpl->DoWindowCreate(name, MSG.width, MSG.height);
    write(serverFd, &windowId, sizeof(windowId));

    // TODO: fixme
    glewInit();
}

void InvokeWindowDestroy()
{
    struct __attribute__((packed)) {
        int windowId;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    mImpl->DoWindowDestroy(MSG.windowId);
}

void InvokeSwapBuffers()
{
    struct __attribute__((packed)) {
        int windowId;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    mImpl->DoSwapBuffers(MSG.windowId);

    // round trip
    ++generation;
    write(serverFd, &generation, sizeof(generation));
}

void InvokeSwapInterval()
{
    struct __attribute__((packed)) {
        int windowId;
        int interval;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    mImpl->DoSwapInterval(MSG.windowId, MSG.interval);
}

void InvokeOGL_glGetString()
{
    struct __attribute__((packed)) {
        GLenum name;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    const GLubyte* ret = glGetString(MSG.name);
    size_t ret_size = strlen((const char*) ret)+1;
    
    // send message
    struct iovec RSP[] = {
        { .iov_base = &ret_size, .iov_len = sizeof(ret_size) },
        { .iov_base = const_cast<GLubyte*>(ret), .iov_len = ret_size },
    };
    writev(serverFd, RSP, sizeof(RSP)/sizeof(RSP[0]));
}

void InvokeOGL_glShaderSource()
{
    struct __attribute__((packed)) {
        GLuint shader;
        GLsizei count;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    GLint length[MSG.count];
    read(serverFd, length, sizeof(length));

    // space for strings
    GLint totalLength = 0;
    for (GLsizei i = 0; i < MSG.count; i++) {
        totalLength += length[i];
    }

    GLint terminatedLength = totalLength + MSG.count; // ensure space for null-terminator
    GLchar data[terminatedLength];
    memset(data, 0, sizeof(data));
    
    // receive strings
    GLchar* ptr = data;
    struct iovec STRINGS[MSG.count];
    const GLchar *strings[MSG.count];
    for (GLsizei i = 0; i < MSG.count; i++) {
        strings[i] = ptr;
        STRINGS[i] = { .iov_base = ptr, .iov_len = (size_t) length[i] };
        ptr += length[i] + 1;
    }
    readv(serverFd, STRINGS, MSG.count);

    glShaderSource(MSG.shader, MSG.count, strings, length);
}

void InvokeOGL_glGetShaderiv()
{
    struct __attribute__((packed)) {
        GLuint shader;
        GLenum pname;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    // send message
    GLint param;
    glGetShaderiv(MSG.shader, MSG.pname, &param);
    write(serverFd, &param, sizeof(param));
}

void InvokeOGL_glGetShaderInfoLog()
{
    struct __attribute__((packed)) {
        GLuint shader;
        GLsizei maxLength;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    GLchar log[MSG.maxLength];
    GLsizei length;
    glGetShaderInfoLog(MSG.shader, MSG.maxLength, &length, log);

    // send message
    struct iovec RSP[] = {
        { .iov_base = &length, .iov_len = sizeof(length) },
        { .iov_base = log, .iov_len = (size_t) length },
    };
    writev(serverFd, RSP, sizeof(RSP)/sizeof(RSP[0]));
}

void InvokeOGL_glGetProgramiv()
{
    struct __attribute__((packed)) {
        GLuint program;
        GLenum pname;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    // send message
    GLint param;
    glGetProgramiv(MSG.program, MSG.pname, &param);
    write(serverFd, &param, sizeof(param));
}

void InvokeOGL_glGetProgramInfoLog()
{
    struct __attribute__((packed)) {
        GLuint program;
        GLsizei maxLength;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    GLchar log[MSG.maxLength];
    GLsizei length;
    glGetProgramInfoLog(MSG.program, MSG.maxLength, &length, log);

    // send message
    struct iovec RSP[] = {
        { .iov_base = &length, .iov_len = sizeof(length) },
        { .iov_base = log, .iov_len = (size_t) length },
    };
    writev(serverFd, RSP, sizeof(RSP)/sizeof(RSP[0]));
}

void InvokeOGL_glGetUniformLocation()
{
    struct __attribute__((packed)) {
        GLuint program;
        GLsizei size;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    GLchar name[MSG.size];
    read(serverFd, name, sizeof(name));

    // send message
    GLint uniform = glGetUniformLocation(MSG.program, name);
    write(serverFd, &uniform, sizeof(uniform));
}

void InvokeOGL_glGenBuffers()
{
    struct __attribute__((packed)) {
        GLsizei n;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    GLuint buffers[MSG.n];
    glGenBuffers(MSG.n, buffers);

    // send message
    write(serverFd, buffers, sizeof(buffers));
}

void InvokeOGL_glBufferData()
{
    struct __attribute__((packed)) {
        GLenum target;
        GLenum usage;
        GLsizeiptr size;
        GLsizeiptr dsize;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    // (TODO: use glMapBuffer instead, or SHM, even better...)
    GLvoid* data = nullptr;
    if (MSG.dsize) {
        data = malloc(MSG.dsize);
        read(serverFd, data, MSG.size);
    }

    glBufferData(MSG.target, MSG.size, data, MSG.usage);

    if (MSG.dsize) {
        free(data);
    }
}

void InvokeOGL_glBufferSubData()
{
    struct __attribute__((packed)) {
        GLenum target;
        GLintptr offset;
        GLsizeiptr size;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    // (TODO: use glMapBuffer instead, or SHM, even better...)
    GLvoid* data = malloc(MSG.size);
    ssize_t b = read(serverFd, data, MSG.size);
    glBufferSubData(MSG.target, MSG.offset, MSG.size, data);
    free(data);
}

void InvokeOGL_glUniformMatrix3fv()
{
    struct __attribute__((packed)) {
        GLint location;
        GLsizei count;
        GLboolean transpose;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    GLfloat v[9*MSG.count];
    read(serverFd, v, sizeof(v));
    glUniformMatrix3fv(MSG.location, MSG.count, MSG.transpose, v);
}

void InvokeOGL_glUniformMatrix4fv()
{
    struct __attribute__((packed)) {
        GLint location;
        GLsizei count;
        GLboolean transpose;
    } MSG;
    read(serverFd, &MSG, sizeof(MSG));

    GLfloat v[16*MSG.count];
    read(serverFd, v, sizeof(v));
    glUniformMatrix4fv(MSG.location, MSG.count, MSG.transpose, v);
}

// handles functions which use purely scalar types (no pointers to things)
#define RECEIVER_IMPL_FUNC_SCALAR(a) case RemoteGLFunction::OGL_##a: \
    { receiver_impl<decltype(a)>::invoke(a, serverFd); break;  }

// template metaprogramming compile-time map
using FunctionItem = std::pair<RemoteGLFunction, void (*)(void)>;
constexpr FunctionItem functions[] = {
    { RemoteGLFunction::WindowHint, InvokeWindowHint },
    { RemoteGLFunction::WindowCreate, InvokeWindowCreate },
    { RemoteGLFunction::WindowDestroy, InvokeWindowDestroy },
    { RemoteGLFunction::SwapBuffers, InvokeSwapBuffers },
    { RemoteGLFunction::SwapInterval, InvokeSwapInterval },

    { RemoteGLFunction::OGL_glGetString, InvokeOGL_glGetString },
    { RemoteGLFunction::OGL_glShaderSource, InvokeOGL_glShaderSource },
    { RemoteGLFunction::OGL_glGetShaderiv, InvokeOGL_glGetShaderiv },
    { RemoteGLFunction::OGL_glGetShaderInfoLog, InvokeOGL_glGetShaderInfoLog },
    { RemoteGLFunction::OGL_glGetProgramiv, InvokeOGL_glGetProgramiv },
    { RemoteGLFunction::OGL_glGetProgramInfoLog, InvokeOGL_glGetProgramInfoLog },
    { RemoteGLFunction::OGL_glGetUniformLocation, InvokeOGL_glGetUniformLocation },
    { RemoteGLFunction::OGL_glGenBuffers, InvokeOGL_glGenBuffers },
    { RemoteGLFunction::OGL_glBufferData, InvokeOGL_glBufferData },
    { RemoteGLFunction::OGL_glBufferSubData, InvokeOGL_glBufferSubData },
    { RemoteGLFunction::OGL_glUniformMatrix3fv, InvokeOGL_glUniformMatrix3fv },
    { RemoteGLFunction::OGL_glUniformMatrix4fv, InvokeOGL_glUniformMatrix4fv },
};

static constexpr void invokeFunction(RemoteGLFunction f,
        int range = (sizeof functions/sizeof functions[0]))
{
    return
            (range == 0) ? throw "unknown function":
            (functions[range - 1].first == f) ? functions[range - 1].second():
            invokeFunction(f, range - 1);
};


int rglDoServer(int fd, struct RemoteGLServerImpl* impl)
{
    //static_assert(std::is_same<signature<decltype(glUniform1iv)>::concrete_arguments_type,signature<decltype(glUniform1iv)>::arguments_type>::value,
        //"oopsy doopsy");

    serverFd = fd;
    mImpl = impl;

    while (run) {
        RemoteGLFunction function = RemoteGLFunction::SYNC;
        if (read(serverFd, &function, sizeof(function)) != sizeof(function)) {
            fprintf(stderr, "failed to read function.\n");
            return EXIT_FAILURE;
        }

        switch(function) {
            RECEIVER_IMPL_FUNC_SCALAR(glClear);
            RECEIVER_IMPL_FUNC_SCALAR(glClearColor);
            RECEIVER_IMPL_FUNC_SCALAR(glViewport);
            RECEIVER_IMPL_FUNC_SCALAR(glEnable);
            RECEIVER_IMPL_FUNC_SCALAR(glDisable);
            RECEIVER_IMPL_FUNC_SCALAR(glDrawArrays);
            RECEIVER_IMPL_FUNC_SCALAR(glCreateShader);
            RECEIVER_IMPL_FUNC_SCALAR(glDeleteShader);
            RECEIVER_IMPL_FUNC_SCALAR(glCompileShader);
            RECEIVER_IMPL_FUNC_SCALAR(glCreateProgram);
            RECEIVER_IMPL_FUNC_SCALAR(glDeleteProgram);
            RECEIVER_IMPL_FUNC_SCALAR(glUseProgram);
            RECEIVER_IMPL_FUNC_SCALAR(glLinkProgram);
            RECEIVER_IMPL_FUNC_SCALAR(glAttachShader);
            RECEIVER_IMPL_FUNC_SCALAR(glBindBuffer);
            RECEIVER_IMPL_FUNC_SCALAR(glEnableVertexAttribArray);
            RECEIVER_IMPL_FUNC_SCALAR(glDisableVertexAttribArray);
            RECEIVER_IMPL_FUNC_SCALAR(glVertexAttribPointer);
            RECEIVER_IMPL_FUNC_SCALAR(glUniform1i);
            RECEIVER_IMPL_FUNC_SCALAR(glUniform2i);
            RECEIVER_IMPL_FUNC_SCALAR(glUniform3i);
            RECEIVER_IMPL_FUNC_SCALAR(glUniform4i);
            RECEIVER_IMPL_FUNC_SCALAR(glUniform1f);
            RECEIVER_IMPL_FUNC_SCALAR(glUniform2f);
            RECEIVER_IMPL_FUNC_SCALAR(glUniform3f);
            RECEIVER_IMPL_FUNC_SCALAR(glUniform4f);
            
            default:
                invokeFunction(function);
        }
    }

    return EXIT_SUCCESS;
}