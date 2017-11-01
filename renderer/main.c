#include <float.h>
#include <math.h>
#include <stdlib.h>
#include "geometry.h"
#include "graphics.h"
#include "image.h"
#include "model.h"
#include "platform.h"

#include <stdio.h>

#define WIDTH 800
#define HEIGHT 800

float zbuffer[WIDTH * HEIGHT];

void draw_model(model_t *model, image_t *texture, image_t *image) {
    int num_faces = model_get_num_faces(model);
    int width = image->width;
    int height = image->height;
    int i, j;
    int tex_width = texture->width;
    int tex_height = texture->height;

    vec3f_t light = {0, 0, -1};

    for (i = 0; i < num_faces; i++) {
        vec3i_t points[3];
        color_t colors[3];
        vec3f_t coords[3];
        vec3f_t normal;
        float intensity;
        for (j = 0; j < 3; j++) {
            vec3f_t vertex = model_get_vertex(model, i, j);
            vec2f_t uv = model_get_uv(model, i, j);
            int tex_x = (int)(uv.x * (tex_width - 1));
            int tex_y = (int)(uv.y * (tex_height - 1));
            tex_y = (tex_height - 1) - tex_y;
            colors[j]  = image_get_color(texture, tex_y, tex_x);

            points[j].x = (int)((vertex.x + 1) / 2 * (width - 1));
            points[j].y = (int)((vertex.y + 1) / 2 * (height - 1));
            points[j].y = (height - 1) - points[j].y;
            points[j].z = (int)((vertex.z + 0.5) * 128);

            coords[j] = vertex;
        }
        normal = vec3f_cross(
            vec3f_sub(coords[2], coords[0]),
            vec3f_sub(coords[1], coords[0])
        );
        normal = vec3f_normalize(normal);

        intensity = vec3f_dot(normal, light);
        if (intensity > 0) {
            color_t color;
            color.b = (unsigned char)(intensity * 255);
            color.g = (unsigned char)(intensity * 255);
            color.r = (unsigned char)(intensity * 255);
            gfx_fill_triangle(image, points[0], points[1], points[2],
                colors[0], colors[1], colors[2], zbuffer, intensity);
        }
    }
}

int main(void) {
    window_t *window;
    image_t *image;
    model_t *model;
    const char *title = "Viewer";
    int width = WIDTH;
    int height = HEIGHT;
    int i;
    image_t *texture;

    window = window_create(title, width, height);
    image = image_create(width, height, 3);

    for (i = 0; i < WIDTH * HEIGHT; i++) {
        zbuffer[i] = FLT_MIN;
    }

    model = model_load("resources/african_head.obj");
    texture = image_load("resources/african_head_diffuse.tga");


    draw_model(model, texture, image);

    while (!window_should_close(window)) {
        window_draw_image(window, image);
        input_poll_events();
    }

    model_free(model);
    image_release(image);
    window_destroy(window);
    return 0;
}
