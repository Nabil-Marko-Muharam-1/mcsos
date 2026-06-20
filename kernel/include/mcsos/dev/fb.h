#ifndef FB_H
#define FB_H
#include <stdint.h>

int fb_init();
void fb_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void fb_draw_string(const char *str, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg);

#endif
