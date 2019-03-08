#ifndef RENDERER_H_
#define RENDERER_H_

#include <remotegl/remotegl-client.h>

struct renderer;

struct renderer* renderer_create(int windowId, int width, int height);
void renderer_release(struct renderer* renderer);
void renderer_draw(struct renderer* renderer);
void renderer_swap(struct renderer* renderer);

#endif
