#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 600
#define HEIGHT 450
#define SEEDS_COUNT 10
#define SEED_RENDER_RADIUS 4

#define COLOR_WHITE     0xFFFFFFFF
#define COLOR_RED       0xFF0000FF
#define COLOR_BLUE      0xFFFF0000
#define COLOR_BLACK     0xFF000000

#define BACKGROUND_COLOR 0xFF181818

#define COLOR_PALLETE_1 0xFF00FF00
#define COLOR_PALLETE_2 0xFF00FF87
#define COLOR_PALLETE_3 0xFF00FFD3
#define COLOR_PALLETE_4 0xFF0ADEFF
#define COLOR_PALLETE_5 0xFF0AA1FF
#define COLOR_PALLETE_6 0xFF990AFF
#define COLOR_PALLETE_7 0xFFFF0AEF
#define COLOR_PALLETE_8 0xFFF5147D
#define COLOR_PALLETE_9 0xFFFF580A
#define COLOR_PALLETE_10 0xFFFFBE0A

typedef uint32_t u32;
typedef uint8_t u8;

u32 pixels[HEIGHT][WIDTH];

typedef struct {
    int x;
    int y;
} Point;

u32 PALETTE[] = {
    COLOR_PALLETE_1,
    COLOR_PALLETE_2,
    COLOR_PALLETE_3,
    COLOR_PALLETE_4,
    COLOR_PALLETE_5,
    COLOR_PALLETE_6,
    COLOR_PALLETE_7,
    COLOR_PALLETE_8,
    COLOR_PALLETE_9,
    COLOR_PALLETE_10,
};

#define PALETTE_SIZE (sizeof(PALETTE)/sizeof(PALETTE[0]))

int distance_squared(int x1, int y1, int x2, int y2) {
    int dx = x1 - x2;
    int dy = y1 - y2;

    return dx*dx + dy*dy;
}

void fill_color(u32 color) {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            pixels[y][x] = color;
        }
    }
}

void fill_circle(int cx, int cy, int r, u32 color) {
    for (int y = cy - r; y <= cy + r; ++y) {
        if (0 <= y && y <= HEIGHT) {
        for (int x = cx - r; x <= cx + r; ++x) {
            if (0 <= x && x < WIDTH) {
                if (distance_squared(x, y, cx, cy) <= r*r) {
                    pixels[y][x] = color;
                }
            }
        }
        }
    }
}

Point seeds[SEEDS_COUNT];

void create_seeds(void) {
    for (int i = 0; i < SEEDS_COUNT; i++) {
        seeds[i].x = rand() % WIDTH;
        seeds[i].y = rand() % HEIGHT;
    }
}

void save_to_ppm_file(const char* path) {
    FILE *f = fopen(path, "wb");

    fprintf(f, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            u32 color = pixels[y][x];

            u8 rgb[3] = {
                (color & 0x0000FF) >> (8*0),
                (color & 0x00FF00) >> (8*1),
                (color & 0xFF0000) >> (8*2),
            };

            fwrite(rgb, sizeof(rgb), 1, f);
        }
    }

    fclose(f);
}

void render_seeds(void) {
    for (int i = 0; i < SEEDS_COUNT; i++) {
        fill_circle(seeds[i].x, seeds[i].y, SEED_RENDER_RADIUS, COLOR_BLACK);
    }
}

void render_voronoi() {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int closest = 0;
            for (int i = 1; i < SEEDS_COUNT; i++) {
                if (distance_squared(x, y, seeds[i].x, seeds[i].y) < distance_squared(x, y, seeds[closest].x, seeds[closest].y)) {
                    closest = i;
                }
            }
            pixels[y][x] = PALETTE[closest % PALETTE_SIZE];
        }
    }
}

int main(void) {
    srand(time(0));
    fill_color(BACKGROUND_COLOR);    
    create_seeds();
    render_voronoi();
    render_seeds();
    save_to_ppm_file("test.ppm");

    return 0;
}