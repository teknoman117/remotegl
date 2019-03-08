#ifndef REMOTEGL_CLIENT_H_
#define REMOTEGL_CLIENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <remotegl/remotegl-types.h>

void rglInitialize(int fd);

void rglWindowHint(RGLenum hint, int value);
int  rglWindowCreate(const char* name, int width, int height);
void rglWindowDestroy(int windowId);
void rglSwapBuffers(int windowId);
void rglSwapInterval(int windowId, int interval);

#ifdef __cplusplus
}
#endif

#endif /* REMOTEGL_CLIENT_H_ */