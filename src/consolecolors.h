#ifndef CONSOLE_COLORS_H
#define CONSOLE_COLORS_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
  black,
  red,
  green,
  orange,
  blue,
  magenta,
  cyan,
  lightgray,
  darkgray,
  lightred,
  lightgreen,
  yellow,
  lightblue,
  lightmagenta,
  lightcyan,
  white,
  NONE
} console_color_t;

typedef enum { normal, bold, faint, italic, underline } console_style_t;

static const char *console_colors_foreground[] = {
    "30", "31", "32", "33", "34", "35", "36", "37", "90",
    "91", "92", "93", "94", "95", "96", "97", "",
};

static const char *console_colors_background[] = {
    "40", "41", "42", "43", "44", "45", "46", "47", "100",
    "101", "102", "103", "104", "105", "106", "107", "",
};

void set_color_output_fg(console_color_t fg, console_style_t style)
{
    printf("\033[%d;%sm", style, console_colors_foreground[fg]);
}

void set_color_output_bg(console_color_t bg)
{
    printf("\033[%sm", console_colors_background[bg]);
}

void reset_color_output()
{
    printf("\033[0m");
}

void color_output(char* text, console_color_t fg, console_color_t bg, console_style_t style)
{
    bool should_reset = false;
    if (strlen(console_colors_foreground[fg]) > 0) {
        set_color_output_fg(fg, style);
        should_reset = true;
    }

    if (strlen(console_colors_background[bg]) > 0) {
        set_color_output_bg(bg);
        should_reset = true;
    }

    printf("%s", text);

    if (should_reset) {
        reset_color_output();
    }
}

void color_output_ln(char* text, console_color_t fg, console_color_t bg, console_style_t style)
{
    color_output(text, fg, bg, style);
    printf("\n");
}

#endif // CONSOLE_COLORS_H
