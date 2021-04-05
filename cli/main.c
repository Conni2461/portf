#include "lib/portf.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

share_t *create_shares(char **names, size_t count) {
  share_t *out = malloc(count * sizeof(share_t));
  for (int i = 0; i < count; i++) {
    (out + i)->name = names[i];
    (out + i)->value = -1;
  }
  return out;
}

typedef enum {
  /* main modes */
  SYNC_MODE,
  QUERY_MODE,
  PORTFOLIO_MODE,

  /* sub modes */
  FORCE_MODE,
  SEARCH_MODE,

  NONE
} mode;

void do_search(char **input, int count) {
  printf("TODO\n");
}

void print_info(char **input, int count) {
  share_t *out = create_shares(input, count);
  get_share(out, count);

  for (int i = 0; i < count; i++) {
    printf("%s: %f\n", (out + i)->name, (out + i)->value);
  }
  free(out);
}

void help(mode main, FILE *channel) {
  fprintf(channel, "HELP PAGE FOR SOME MODE; TODO\n");
}

void error(char bad, mode main) {
  if (bad == '\0') {
    fprintf(stderr, "NO INPUT IS NOT AN OPTION\n");
    help(main, stderr);
  } else {
    fprintf(stderr, "FAILURE with help for main (%c)\n", bad);
    help(main, stderr);
  }
}

/*
  -S amc
  -Sf amc
  -Q amc
  -P
  -h
*/
int main(int argc, char *argv[]) {
  mode main_mode = NONE;
  mode sub_mode = NONE;

  if (argc == 1) {
    error('\0', main_mode);
    exit(EXIT_FAILURE);
  }

  /* OPTION LOOP */
  size_t option_len = strlen(argv[1]);
  for (size_t stri = 1; stri < option_len; stri++) {
    switch (argv[1][stri]) {
    case 'S':
      main_mode = SYNC_MODE;
      break;
    case 'Q':
      main_mode = QUERY_MODE;
      break;
    case 'P':
      main_mode = PORTFOLIO_MODE;
      break;
    case 'f':
      sub_mode = FORCE_MODE;
      break;
    case 's':
      sub_mode = SEARCH_MODE;
      break;
    case 'h':
      help(main_mode, stdout);
      exit(EXIT_SUCCESS);
    case 'v':
      printf("portf v%s\n", get_version());
      exit(EXIT_SUCCESS);
    default:
      error(argv[1][stri], main_mode);
      exit(EXIT_FAILURE);
    }
  }

  /* INPUT ARGUMENT LOOP */
  int count = argc - 2;
  char **inputs = NULL;
  if (count > 0) {
    inputs = malloc(count * sizeof(char *));
    for (size_t i = 2; i < argc; i++) {
      inputs[i - 2] = malloc(strlen(argv[i]) * sizeof(char));
      strcpy(inputs[i - 2], argv[i]);
    }
  }

  if (main_mode == SYNC_MODE) {
    if (sub_mode == SEARCH_MODE) {
      do_search(inputs, count);
      for (int i = 0; i < count; i++) {
        free(inputs[i]);
      }
      free(inputs);
    }
    if (sub_mode == FORCE_MODE) {
      print_info(inputs, count);
      for (int i = 0; i < count; i++) {
        free(inputs[i]);
      }
      free(inputs);
    }
  } else {
    printf("TODO\n");
  }

  return 0;
}
