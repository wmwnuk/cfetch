#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <argp.h>
#include "drawframe.h"
#include "consolecolors.h"

const char *argp_program_version = "cfetch-0.1";
const char *argp_program_bug_address = "<laniusone@pm.me>";

static char doc[] = "Simple fetch program (that doesn't fetch anything)";
static char args_doc[] = "";

static struct argp_option options[] = {
  {"config",   'c', "FILE", 0, "Config FILE to use" },
  {"no-config",   'n', 0, 0, "No config file" },
  { 0 }
};

struct arguments {
    char* config;
    bool no_config;
};

struct config {
    console_color_t frame_color;
    console_color_t os_color;
    console_color_t shell_color;
    console_color_t term_color;
    char* os_icon;
    char* shell_icon;
    char* term_icon;
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'n':
      arguments->no_config = true;
      break;
    case 'c':
      arguments->config = arg;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

char *get_os()
{
    FILE *os_release;
    char line[1000];
    os_release = fopen("/etc/os-release", "r");
    static char os[1000];

    while (NULL != fgets(line, 1000, os_release)) {
        if (1 == sscanf(line, "PRETTY_NAME=\"%[^\"]", os)) {
            break;
        }
    }

    fclose(os_release);

    return os;
}

int main(int argc, char** argv)
{
    struct arguments arguments;
    struct config config;

    /* Default values */
    arguments.config = "";
    arguments.no_config = false;

    config.frame_color = red;
    config.os_color = blue;
    config.shell_color = orange;
    config.term_color = magenta;
    config.os_icon = "";
    config.shell_icon = "";
    config.term_icon = "";

    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    if (!arguments.no_config) {
        FILE *conf;
        char line[1000];

        if (strlen(arguments.config) > 0) {
            conf = fopen(arguments.config, "r");
        } else {
            conf = fopen(strcat(getenv("HOME"), "/.config/cfetch/config"), "r");
        }

        if (conf != NULL) {
            while (NULL != fgets(line, sizeof(line), conf)) {
                console_color_t value;
                static char *os_icon, *shell_icon, *term_icon;
                if (1 == sscanf(line, "frame_color=%u", &value)) {
                    config.frame_color = value;
                }
                if (1 == sscanf(line, "os_color=%u", &value)) {
                    config.os_color = value;
                }
                if (1 == sscanf(line, "shell_color=%u", &value)) {
                    config.shell_color = value;
                }
                if (1 == sscanf(line, "term_color=%u", &value)) {
                    config.term_color = value;
                }
                if (1 == sscanf(line, "os_icon=%s", os_icon)) {
                    config.os_icon = os_icon;
                }
                if (1 == sscanf(line, "shell_icon=%s", shell_icon)) {
                    config.shell_icon = shell_icon;
                }
                if (1 == sscanf(line, "term_icon=%s", term_icon)) {
                    config.term_icon = term_icon;
                }
            }
        } else if (strlen(arguments.config) > 0) {
            color_output_ln("Specified config file doesn't exist.", red, NONE, normal);

            return EXIT_FAILURE;
        }
    }

    struct utsname buf1;
    errno = 0;
    if (uname(&buf1) != 0) {
        perror("There was an error.");
        exit(EXIT_FAILURE);
    }
    int length = strlen(buf1.release) + strlen(buf1.machine) + 6;
    char *os = get_os();
    set_color_output_fg(config.frame_color, normal);
    printf("%s", frame_top_left);
    draw_horizontal_frame(2);
    printf("[");
    reset_color_output();
    printf("%s", os);
    set_color_output_fg(config.frame_color, normal);
    printf("]");
    draw_horizontal_frame(length - strlen(os) - 3);
    printf("%s", frame_top_right);
    printf("\n");

    // system
    draw_vertical_frame();
    set_color_output_fg(config.os_color, normal);
    printf(" %s ", config.os_icon);
    set_color_output_fg(darkgray, normal);
    printf("▐ ");
    reset_color_output();
    printf("%s %s ", buf1.release, buf1.machine);
    set_color_output_fg(config.frame_color, normal);
    draw_vertical_frame();
    printf("\n");

    // shell
    char * shell = getenv("SHELL");
    draw_vertical_frame();
    set_color_output_fg(config.shell_color, normal);
    printf(" %s ", config.shell_icon);
    set_color_output_fg(darkgray, normal);
    printf("▐ ");
    reset_color_output();
    printf("%s ", shell);

    for (int i = 0; i < length - strlen(shell) - 5; i++) {
        printf(" ");
    }

    set_color_output_fg(config.frame_color, normal);
    draw_vertical_frame();
    printf("\n");

    // terminal
    char * term = getenv("TERM");
    draw_vertical_frame();
    set_color_output_fg(config.term_color, normal);
    printf(" %s ", config.term_icon);
    set_color_output_fg(darkgray, normal);
    printf("▐ ");
    reset_color_output();
    printf("%s ", term);

    for (int i = 0; i < length - strlen(term) - 5; i++) {
        printf(" ");
    }

    set_color_output_fg(config.frame_color, normal);
    draw_vertical_frame();
    printf("\n");

    draw_bottom_frame(length + 3);
    printf("\n");

    return EXIT_SUCCESS;
}
