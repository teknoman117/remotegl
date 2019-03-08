#ifndef REMOTEGL_H_
#define REMOTEGL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <GL/gl.h>

// OpenGL functions which would have to be loaded via GetProcAddr

GLAPI GLuint GLAPIENTRY glCreateShader(GLenum type);
GLAPI void GLAPIENTRY glDeleteShader(GLuint shader);
GLAPI void GLAPIENTRY glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
GLAPI void GLAPIENTRY glCompileShader(GLuint shader);
GLAPI void GLAPIENTRY glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
GLAPI void GLAPIENTRY glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
GLAPI GLuint GLAPIENTRY glCreateProgram(void);
GLAPI void GLAPIENTRY glDeleteProgram(GLuint program);
GLAPI void GLAPIENTRY glUseProgram(GLuint program);
GLAPI void GLAPIENTRY glAttachShader(GLuint program, GLuint shader);
GLAPI void GLAPIENTRY glLinkProgram(GLuint program);
GLAPI void GLAPIENTRY glGetProgramiv(GLuint program, GLenum pname, GLint *params);
GLAPI void GLAPIENTRY glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
GLAPI GLint GLAPIENTRY glGetUniformLocation(GLuint program, const GLchar* name);
GLAPI void GLAPIENTRY glGenBuffers(GLsizei n, GLuint* buffers);
GLAPI void GLAPIENTRY glDeleteBuffers(GLsizei n, const GLuint* buffers);
GLAPI void GLAPIENTRY glBindBuffer(GLenum target, GLuint buffer);
GLAPI void GLAPIENTRY glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
GLAPI void GLAPIENTRY glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
GLAPI void GLAPIENTRY glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
GLAPI void GLAPIENTRY glEnableVertexAttribArray(GLuint index);
GLAPI void GLAPIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count);



GLAPI void GLAPIENTRY glGenVertexArrays(GLsizei n, GLuint* arrays);
GLAPI void GLAPIENTRY glDeleteVertexArrays(GLsizei n, const GLuint* arrays);
GLAPI void GLAPIENTRY glBindVertexArray(GLuint array);
GLAPI void GLAPIENTRY glDisableVertexAttribArray(GLuint index);
GLAPI void GLAPIENTRY glGenFramebuffers(GLsizei n, GLuint* framebuffers);
GLAPI void GLAPIENTRY glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers);
GLAPI void GLAPIENTRY glBindFramebuffer(GLenum target, GLuint framebuffer);
GLAPI void GLAPIENTRY glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GLAPI GLuint GLAPIENTRY glCheckFramebufferStatus(GLenum target);
GLAPI void GLAPIENTRY glGenRenderbuffers(GLsizei n, GLuint* renderbuffers);
GLAPI void GLAPIENTRY glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers);
GLAPI void GLAPIENTRY glBindRenderbuffer(GLenum target, GLuint renderbuffer);
GLAPI void GLAPIENTRY glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
GLAPI void GLAPIENTRY glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
GLAPI GLint GLAPIENTRY glGetAttribLocation(GLuint program, const GLchar* name);
GLAPI void GLAPIENTRY glBindAttribLocation(GLuint program, GLuint index, const GLchar* name);
GLAPI void GLAPIENTRY glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
GLAPI void GLAPIENTRY glGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
GLAPI void GLAPIENTRY glGetShaderSource(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
GLAPI void GLAPIENTRY glGenerateMipmap(GLenum target);
GLAPI void* GLAPIENTRY glMapBuffer(GLenum target, GLenum access);
GLAPI GLboolean GLAPIENTRY glUnmapBuffer(GLenum target);
GLAPI GLboolean GLAPIENTRY glIsFramebuffer(GLuint framebuffer);
GLAPI GLboolean GLAPIENTRY glIsRenderbuffer(GLuint renderbuffer);
GLAPI GLboolean GLAPIENTRY glIsBuffer(GLuint buffer);
GLAPI void GLAPIENTRY glUniform1i(GLint location, GLint v0);
GLAPI void GLAPIENTRY glUniform2i(GLint location, GLint v0, GLint v1);
GLAPI void GLAPIENTRY glUniform3i(GLint location, GLint v0, GLint v1, GLint v2);
GLAPI void GLAPIENTRY glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
GLAPI void GLAPIENTRY glUniform1iv(GLint location, GLsizei count, const GLint *value);
GLAPI void GLAPIENTRY glUniform2iv(GLint location, GLsizei count, const GLint *value);
GLAPI void GLAPIENTRY glUniform3iv(GLint location, GLsizei count, const GLint *value);
GLAPI void GLAPIENTRY glUniform4iv(GLint location, GLsizei count, const GLint *value);
GLAPI void GLAPIENTRY glUniform1f(GLint location, GLfloat v0);
GLAPI void GLAPIENTRY glUniform2f(GLint location, GLfloat v0, GLfloat v1);
GLAPI void GLAPIENTRY glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
GLAPI void GLAPIENTRY glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLAPI void GLAPIENTRY glUniform1fv(GLint location, GLsizei count, const GLfloat *value);
GLAPI void GLAPIENTRY glUniform2fv(GLint location, GLsizei count, const GLfloat *value);
GLAPI void GLAPIENTRY glUniform3fv(GLint location, GLsizei count, const GLfloat *value);
GLAPI void GLAPIENTRY glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
GLAPI void GLAPIENTRY glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI void GLAPIENTRY glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI void GLAPIENTRY glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#ifdef __cplusplus
}
#endif

#endif /* REMOTEGL_H_ */