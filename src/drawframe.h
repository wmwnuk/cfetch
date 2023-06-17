#ifndef DRAWFRAME_H
#define DRAWFRAME_H

#include <stdio.h>
#include <string.h>

static char frame_top_left[] = "╭";
static char frame_top_right[] = "╮";
static char frame_vertical[] = "│";
static char frame_horizontal[] = "─";
static char frame_bottom_left[] = "╰";
static char frame_bottom_right[] = "╯";
static char frame_cross_left[] = "┤";
static char frame_cross_right[] = "├";
static char frame_cross_up[] = "┴";
static char frame_cross_down[] = "┬";
static char frame_cross[] = "┼";

void draw_vertical_frame() {
    printf("%s", frame_vertical);
}

void draw_horizontal_frame(int width) {
    for (int i = 0; i < width; i++) {
        printf("%s", frame_horizontal);
    }
}

void draw_top_frame(int width) {
    printf("%s", frame_top_left);

    draw_horizontal_frame(width - 2);

    printf("%s", frame_top_right);
}


void draw_bottom_frame(int width) {
    printf("%s", frame_bottom_left);

    draw_horizontal_frame(width - 2);

    printf("%s", frame_bottom_right);
}

#endif // DRAWFRAME_H
