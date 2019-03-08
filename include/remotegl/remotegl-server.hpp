#ifndef REMOTEGL_SERVER_H_
#define REMOTEGL_SERVER_H_

#include <remotegl/remotegl-types.h>

class RemoteGLServerImpl
{
public:
    virtual ~RemoteGLServerImpl() = default;

    virtual void DoWindowHint(RGLenum hint, int value) = 0;
    virtual int DoWindowCreate(const char* name, int width, int height) = 0;
    virtual void DoWindowDestroy(int windowId) = 0;
    virtual void DoSwapBuffers(int windowId) = 0;
    virtual void DoSwapInterval(int windowId, int interval) = 0;
};

int rglDoServer(int fd, RemoteGLServerImpl* impl);

#endif /* REMOTEGL_SERVER_H_ */