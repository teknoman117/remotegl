#include <remotegl/remotegl-client.h>

#include <unordered_map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

std::unordered_map<int, GLFWwindow*> mWindows;
int mGeneration;
inline int bumpGeneration() { return mGeneration++; }

void rglInitialize(int fd)
{
    glfwInit();
}

void rglWindowHint(RGLenum hint, int value)
{
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

int rglWindowCreate(const char* name, int width, int height)
{
    int windowId = bumpGeneration();
    GLFWwindow *window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
    }

    mWindows.emplace(windowId, window);
    glfwMakeContextCurrent(window);
    glewInit();
    return windowId;
}

void rglWindowDestroy(int windowId)
{
    auto window = mWindows.find(windowId);
    if (window != mWindows.end()) {
        glfwDestroyWindow(window->second);
        mWindows.erase(window);
    }
}

void rglSwapBuffers(int windowId)
{
    auto window = mWindows.find(windowId);
    if (window != mWindows.end()) {
        glfwSwapBuffers(window->second);
    }
}

void rglSwapInterval(int windowId, int interval)
{
    auto window = mWindows.find(windowId);
    if (window != mWindows.end()) {
        glfwSwapInterval(interval);
    }
}