#include "mcsos/dev/fb.h"

extern const uint8_t font[128][8];

struct limine_framebuffer {
    void *address; uint64_t width; uint64_t height; uint64_t pitch;
    uint16_t bpp; uint8_t memory_model; uint8_t red_mask_size, red_mask_shift;
    uint8_t green_mask_size, green_mask_shift; uint8_t blue_mask_size, blue_mask_shift;
    uint8_t unused[7]; uint64_t edid_size; void *edid;
};
struct limine_framebuffer_response {
    uint64_t revision; uint64_t framebuffer_count;
    struct limine_framebuffer **framebuffers;
};

// [PERBAIKAN MUTLAK]: Array id harus berjumlah 4!
struct limine_framebuffer_request {
    uint64_t id[4]; uint64_t revision;
    struct limine_framebuffer_response *response;
};

__attribute__((used, section(".data"), aligned(8)))
volatile struct limine_framebuffer_request fb_request = {
    // [PERBAIKAN MUTLAK]: 2 kode pertama adalah Common Magic Limine, 2 kode terakhir adalah Framebuffer Magic!
    .id = {0xc7b1dd30df4c8b88, 0x0a82e883a194f07b, 0x9d5827dcd881dd75, 0xa3148604f6fab11b},
    .revision = 0
};

static struct limine_framebuffer *fb = 0;

int fb_init() {
    if (fb_request.response == 0) return -1; 
    if (fb_request.response->framebuffer_count < 1) return -2;
    fb = fb_request.response->framebuffers[0];
    return 1;
}

void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!fb || x >= fb->width || y >= fb->height) return;
    uint32_t *pixel = (uint32_t *)((uint8_t *)fb->address + y * fb->pitch + x * 4);
    *pixel = color;
}

void fb_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    for (uint32_t i = 0; i < h; i++) {
        for (uint32_t j = 0; j < w; j++) fb_put_pixel(x + j, y + i, color);
    }
}

void fb_draw_string(const char *str, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg) {
    uint32_t cur_x = x;
    while (*str) {
        char c = *str;
        if (c < 0 || c > 127) c = ' '; 
        for (int cy = 0; cy < 8; cy++) {
            for (int cx = 0; cx < 8; cx++) {
                uint32_t color = (font[(int)c][cy] & (1 << (7 - cx))) ? fg : bg;
                fb_put_pixel(cur_x + (cx*2), y + (cy*2), color);
                fb_put_pixel(cur_x + (cx*2)+1, y + (cy*2), color);
                fb_put_pixel(cur_x + (cx*2), y + (cy*2)+1, color);
                fb_put_pixel(cur_x + (cx*2)+1, y + (cy*2)+1, color);
            }
        }
        cur_x += 16;
        str++;
    }
}
