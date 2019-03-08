#include <remotegl/remotegl-client.h>

#include <cstring>
#include <unordered_map>

#include <unistd.h>
#include <sys/uio.h>

#include <remotegl/remotegl.h>
#include "remotegl-protocol.hpp"

int clientFd = -1;
std::unordered_map<GLenum,std::string> CACHE_glGetString;

void rglInitialize(int fd)
{
    clientFd = fd;
}

void rglWindowHint(RGLenum hint, int value)
{
    auto function = RemoteGLFunction::WindowHint;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &hint, .iov_len = sizeof(hint) },
        { .iov_base = &value, .iov_len = sizeof(value) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

int rglWindowCreate(const char* name, int width, int height)
{
    auto function = RemoteGLFunction::WindowCreate;

    // compute string properties
    size_t name_size = strlen(name) + 1;
    
    // send message
    const struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &name_size, .iov_len = sizeof(name_size) },
        { .iov_base = &width, .iov_len = sizeof(width) },
        { .iov_base = &height, .iov_len = sizeof(height) },
        { .iov_base = const_cast<char*>(name), .iov_len = name_size },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));

    // receive message
    int windowId;
    read(clientFd, &windowId, sizeof(windowId));
    return windowId;
}

void rglWindowDestroy(int windowId)
{
    auto function = RemoteGLFunction::WindowDestroy;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &windowId, .iov_len = sizeof(windowId) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void rglSwapBuffers(int windowId)
{
    auto function = RemoteGLFunction::SwapBuffers;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &windowId, .iov_len = sizeof(windowId) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));

    // receive message (round trip)
    int generation;
    read(clientFd, &generation, sizeof(generation));}

void rglSwapInterval(int windowId, int interval)
{
    auto function = RemoteGLFunction::SwapInterval;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &windowId, .iov_len = sizeof(windowId) },
        { .iov_base = &interval, .iov_len = sizeof(interval) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glClear(GLbitfield mask)
{
    auto function = RemoteGLFunction::OGL_glClear;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &mask, .iov_len = sizeof(mask) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    auto function = RemoteGLFunction::OGL_glClearColor;

    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &red, .iov_len = sizeof(red) },
        { .iov_base = &green, .iov_len = sizeof(green) },
        { .iov_base = &blue, .iov_len = sizeof(blue) },
        { .iov_base = &alpha, .iov_len = sizeof(alpha) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

const GLubyte* glGetString(GLenum name)
{
    auto s = CACHE_glGetString.find(name);
    if (s != CACHE_glGetString.end()) {
        return (const GLubyte*) s->second.c_str();
    }

    auto function = RemoteGLFunction::OGL_glGetString;

    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &name, .iov_len = sizeof(name) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));

    // receive message
    size_t size;
    read(clientFd, &size, sizeof(size));
    char contents[size];
    read(clientFd, contents, size);

    // cache and return string
    return (const GLubyte*) CACHE_glGetString.emplace(name, std::string{contents, size})
            .first->second.c_str();
}

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    auto function = RemoteGLFunction::OGL_glViewport;

    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &x, .iov_len = sizeof(x) },
        { .iov_base = &y, .iov_len = sizeof(y) },
        { .iov_base = &width, .iov_len = sizeof(width) },
        { .iov_base = &height, .iov_len = sizeof(height) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glEnable(GLenum cap)
{
    auto function = RemoteGLFunction::OGL_glEnable;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &cap, .iov_len = sizeof(cap) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glDisable(GLenum cap)
{
    auto function = RemoteGLFunction::OGL_glDisable;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &cap, .iov_len = sizeof(cap) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

GLuint glCreateShader(GLenum type)
{
    auto function = RemoteGLFunction::OGL_glCreateShader;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &type, .iov_len = sizeof(type) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));

    // receive message
    int shader;
    read(clientFd, &shader, sizeof(shader));
    return shader;
}

void glDeleteShader(GLuint shader)
{
    auto function = RemoteGLFunction::OGL_glDeleteShader;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &shader, .iov_len = sizeof(shader) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glCompileShader(GLuint shader)
{
    auto function = RemoteGLFunction::OGL_glCompileShader;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &shader, .iov_len = sizeof(shader) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length)
{
    auto function = RemoteGLFunction::OGL_glShaderSource;

    // compute lengths if they aren't provided
    GLint length2[count];
    if (!length) {
        length = length2;
        for (int i = 0; i < count; i++) {
            length2[i] = strlen(string[i]);
        }
    }

    // send message
    struct iovec MSG[4+count] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &shader, .iov_len = sizeof(shader) },
        { .iov_base = &count, .iov_len = sizeof(count) },
        { .iov_base = const_cast<GLint*>(length), .iov_len = (size_t) count*sizeof(length[0]) },
    };
    for (int i = 0; i < count; i++) {
        MSG[i+4] = { .iov_base = const_cast<GLchar*>(string[i]), .iov_len = (size_t) length[i] };
    }
    writev(clientFd, MSG, 4+count);
}

void glGetShaderiv(GLuint shader, GLenum pname, GLint *params)
{
    auto function = RemoteGLFunction::OGL_glGetShaderiv;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &shader, .iov_len = sizeof(shader) },
        { .iov_base = &pname, .iov_len = sizeof(pname) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));

    // receive message
    read(clientFd, params, sizeof(*params));
}

void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *log)
{
    auto function = RemoteGLFunction::OGL_glGetShaderInfoLog;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &shader, .iov_len = sizeof(shader) },
        { .iov_base = &maxLength, .iov_len = sizeof(maxLength) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));

    // receive message
    read(clientFd, length, sizeof(*length));
    read(clientFd, log, *length);
}

GLuint glCreateProgram(void)
{
    auto function = RemoteGLFunction::OGL_glCreateProgram;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));

    // receive message
    int program;
    read(clientFd, &program, sizeof(program));
    return program;
}

void glDeleteProgram(GLuint program)
{
    auto function = RemoteGLFunction::OGL_glDeleteProgram;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &program, .iov_len = sizeof(program) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glUseProgram(GLuint program)
{
    auto function = RemoteGLFunction::OGL_glUseProgram;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &program, .iov_len = sizeof(program) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glAttachShader(GLuint program, GLuint shader)
{
    auto function = RemoteGLFunction::OGL_glAttachShader;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &program, .iov_len = sizeof(program) },
        { .iov_base = &shader, .iov_len = sizeof(shader) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glLinkProgram(GLuint program)
{
    auto function = RemoteGLFunction::OGL_glLinkProgram;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &program, .iov_len = sizeof(program) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glGetProgramiv(GLuint program, GLenum pname, GLint *params)
{
    auto function = RemoteGLFunction::OGL_glGetProgramiv;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &program, .iov_len = sizeof(program) },
        { .iov_base = &pname, .iov_len = sizeof(pname) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));

    // receive message
    read(clientFd, params, sizeof(*params));
}

void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *log)
{
    auto function = RemoteGLFunction::OGL_glGetProgramInfoLog;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &program, .iov_len = sizeof(program) },
        { .iov_base = &maxLength, .iov_len = sizeof(maxLength) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));

    // receive message
    read(clientFd, length, sizeof(*length));
    read(clientFd, log, *length);
}

GLint glGetUniformLocation(GLuint program, const GLchar* name)
{
    auto function = RemoteGLFunction::OGL_glGetUniformLocation;

    // send message
    GLsizei size = (GLsizei) strlen(name) + 1;
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &program, .iov_len = sizeof(program) },
        { .iov_base = &size, .iov_len = sizeof(size) },
        { .iov_base = const_cast<GLchar*>(name), .iov_len = (size_t) size },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));

    // receive message
    GLint uniform;
    read(clientFd, &uniform, sizeof(uniform));
    return uniform;
}

void glGenBuffers(GLsizei n, GLuint* buffers)
{
    auto function = RemoteGLFunction::OGL_glGenBuffers;

    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &n, .iov_len = sizeof(n) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));

    // receive message
    read(clientFd, buffers, n*sizeof(buffers[0]));
}

void glBindBuffer(GLenum target, GLuint buffer)
{
    auto function = RemoteGLFunction::OGL_glBindBuffer;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &target, .iov_len = sizeof(target) },
        { .iov_base = &buffer, .iov_len = sizeof(buffer) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    auto function = RemoteGLFunction::OGL_glBufferData;

    // send message
    GLsizeiptr dsize = data ? size : 0;
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &target, .iov_len = sizeof(target) },
        { .iov_base = &usage, .iov_len = sizeof(usage) },
        { .iov_base = &size, .iov_len = sizeof(size) },
        { .iov_base = &dsize, .iov_len = sizeof(dsize) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));

    // send buffer data if it's present
    if (dsize) {
        write(clientFd, data, dsize);
    }
}

void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    auto function = RemoteGLFunction::OGL_glBufferSubData;

    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &target, .iov_len = sizeof(target) },
        { .iov_base = &offset, .iov_len = sizeof(offset) },
        { .iov_base = &size, .iov_len = sizeof(size) },
        { .iov_base = const_cast<GLvoid*>(data), .iov_len = (size_t) size },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized,
        GLsizei stride, const GLvoid * pointer)
{
    auto function = RemoteGLFunction::OGL_glVertexAttribPointer;

    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &index, .iov_len = sizeof(index) },
        { .iov_base = &size, .iov_len = sizeof(size) },
        { .iov_base = &type, .iov_len = sizeof(type) },
        { .iov_base = &normalized, .iov_len = sizeof(normalized) },
        { .iov_base = &stride, .iov_len = sizeof(stride) },
        { .iov_base = &pointer, .iov_len = sizeof(pointer) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));   
}

void glEnableVertexAttribArray(GLuint index)
{
    auto function = RemoteGLFunction::OGL_glEnableVertexAttribArray;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &index, .iov_len = sizeof(index) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    auto function = RemoteGLFunction::OGL_glUniformMatrix3fv;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &location, .iov_len = sizeof(location) },
        { .iov_base = &count, .iov_len = sizeof(count) },
        { .iov_base = &transpose, .iov_len = sizeof(transpose) },
        { .iov_base = const_cast<GLfloat*>(value), .iov_len = 9*count*sizeof(value[0]) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    auto function = RemoteGLFunction::OGL_glUniformMatrix4fv;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &location, .iov_len = sizeof(location) },
        { .iov_base = &count, .iov_len = sizeof(count) },
        { .iov_base = &transpose, .iov_len = sizeof(transpose) },
        { .iov_base = const_cast<GLfloat*>(value), .iov_len = 16*count*sizeof(value[0]) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}

void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    auto function = RemoteGLFunction::OGL_glDrawArrays;
    
    // send message
    struct iovec MSG[] = {
        { .iov_base = &function, .iov_len = sizeof(function) },
        { .iov_base = &mode, .iov_len = sizeof(mode) },
        { .iov_base = &first, .iov_len = sizeof(first) },
        { .iov_base = &count, .iov_len = sizeof(count) },
    };
    writev(clientFd, MSG, sizeof(MSG)/sizeof(MSG[0]));
}