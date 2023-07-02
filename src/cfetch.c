#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <argp.h>
#include "drawframe.h"
#include "consolecolors.h"
#include "../tomlc99/toml.h"
#include "../tomlc99/toml.c"

const char *argp_program_version = "cfetch-0.2";
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
    char os_icon[10];
    char shell_icon[10];
    char term_icon[10];
};

static void error(const char* msg, const char* msg1)
{
    fprintf(stderr, "ERROR: %s%s\n", msg, msg1 ? msg1 : "");
    exit(EXIT_FAILURE);
}

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
  return EXIT_SUCCESS;
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
    strcpy(config.os_icon, "");
    strcpy(config.shell_icon, "");
    strcpy(config.term_icon, "");

    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    if (!arguments.no_config) {
        FILE *conf;
        char errbuf[200];

        if (strlen(arguments.config) > 0) {
            conf = fopen(arguments.config, "r");
        } else {
            conf = fopen(strcat(getenv("HOME"), "/.config/cfetch/config.toml"), "r");
        }

        if (conf != NULL) {
            toml_table_t* config_toml = toml_parse_file(conf, errbuf, sizeof(errbuf));
            fclose(conf);

            if (!config_toml) {
                error("Cannot parse - ", errbuf);
            }

            toml_table_t* colors = toml_table_in(config_toml, "colors");
            toml_table_t* icons = toml_table_in(config_toml, "icons");

            if (colors) {
                toml_datum_t frame = toml_int_in(colors, "frame");
                toml_datum_t os = toml_int_in(colors, "os");
                toml_datum_t shell = toml_int_in(colors, "shell");
                toml_datum_t term = toml_int_in(colors, "term");

                if (frame.ok) {
                    config.frame_color = (int)frame.u.i;
                }
                if (os.ok) {
                    config.os_color = (int)os.u.i;
                }
                if (shell.ok) {
                    config.shell_color = (int)shell.u.i;
                }
                if (term.ok) {
                    config.term_color = (int)term.u.i;
                }
            }

            if (icons) {
                toml_datum_t os = toml_string_in(icons, "os");
                toml_datum_t shell = toml_string_in(icons, "shell");
                toml_datum_t term = toml_string_in(icons, "term");

                if (os.ok) {
                    strcpy(config.os_icon, os.u.s);
                    FREE(os.u.s);
                }
                if (shell.ok) {
                    strcpy(config.shell_icon, shell.u.s);
                    FREE(shell.u.s);
                }
                if (term.ok) {
                    strcpy(config.term_icon, term.u.s);
                    FREE(term.u.s);
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
    char *os_name = get_os();
    set_color_output_fg(config.frame_color, normal);
    printf("%s", frame_top_left);
    draw_horizontal_frame(2);
    printf("[");
    reset_color_output();
    printf("%s", os_name);
    set_color_output_fg(config.frame_color, normal);
    printf("]");
    draw_horizontal_frame(length - strlen(os_name) - 3);
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
