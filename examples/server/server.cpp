#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unordered_map>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <GLFW/glfw3.h>

#include <remotegl/remotegl-server.hpp>

class ServerImpl : public RemoteGLServerImpl
{
    inline int bumpGeneration() { return mGeneration++; }

    static void DoError(int code, const char* description) {
        fprintf(stderr, "glfw error: %s (%d)\n", description, code);
    }

public:
    ServerImpl();
    ~ServerImpl();

    void DoWindowHint(RGLenum hint, int value) override;
    int DoWindowCreate(const char* name, int width, int height) override;
    void DoWindowDestroy(int windowId) override;
    void DoSwapBuffers(int windowId) override;
    void DoSwapInterval(int windowId, int interval) override;

private:
    std::unordered_map<int, GLFWwindow*> mWindows;
    int mGeneration;
};

ServerImpl::ServerImpl() : mGeneration(0) {
    glfwSetErrorCallback(&ServerImpl::DoError);
    glfwInit();
}

ServerImpl::~ServerImpl() {
    glfwTerminate();
}

void ServerImpl::DoWindowHint(RGLenum hint, int value) {
    constexpr int hintmap[] = {
        [RGL_WINDOW_HINT_RESIZABLE] = GLFW_RESIZABLE,
        [RGL_WINDOW_HINT_RED_BITS] = GLFW_RED_BITS,
        [RGL_WINDOW_HINT_GREEN_BITS] = GLFW_GREEN_BITS,
        [RGL_WINDOW_HINT_BLUE_BITS] = GLFW_BLUE_BITS,
        [RGL_WINDOW_HINT_ALPHA_BITS] = GLFW_ALPHA_BITS,
        [RGL_WINDOW_HINT_DEPTH_BITS] = GLFW_DEPTH_BITS,
        [RGL_WINDOW_HINT_STENCIL_BITS] = GLFW_STENCIL_BITS,
        [RGL_WINDOW_HINT_SAMPLES] = GLFW_SAMPLES
    };

    glfwWindowHint(hintmap[hint], value);
}

int ServerImpl::DoWindowCreate(const char* name, int width, int height) {
    int windowId = bumpGeneration();
    GLFWwindow *window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
    }

    mWindows.emplace(windowId, window);
    glfwMakeContextCurrent(window);
    return windowId;
}

void ServerImpl::DoWindowDestroy(int windowId) {
    auto window = mWindows.find(windowId);
    if (window != mWindows.end()) {
        glfwDestroyWindow(window->second);
        mWindows.erase(window);
    }
}

void ServerImpl::DoSwapBuffers(int windowId) {
    auto window = mWindows.find(windowId);
    if (window != mWindows.end()) {
        glfwSwapBuffers(window->second);
    }
}

void ServerImpl::DoSwapInterval(int windowId, int interval) {
    auto window = mWindows.find(windowId);
    if (window != mWindows.end()) {
        glfwSwapInterval(interval);
    }
}

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <client> [client args...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // name file descriptors
    union {
        struct {
            int client;
            int server;
        } fd;
        int fds[2];
    };

    // Create socketpair for communication
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1) {
        fprintf(stderr, "Failed to create socket pair.\n");
        return EXIT_FAILURE;
    }

    // Start client
    pid_t child = fork();
    if (child == -1) {
        fprintf(stderr, "Failed to start child process.\n");
        return EXIT_FAILURE;
    }

    if (!child) {
        // build child process argv (/path/to/child <client socketpair fd> [client args...])
        const char* child_executable = argv[1];
        char id[8];
        snprintf(id, sizeof(id), "%d", fd.client);

        const char* child_argv[argc+1];
        memset((void*) &child_argv, 0, sizeof(child_argv));

        int childIdx = 0;
        child_argv[childIdx++] = child_executable;
        child_argv[childIdx++] = id;
        for (int i = 2; i < argc; i++) {
            child_argv[childIdx++] = argv[i];
        }

        // execute child process
        close(fd.server);
        execv(child_executable, (char * const *) child_argv);
    }

    // run the server
    close(fd.client);
    return rglDoServer(fd.server, new ServerImpl());
}