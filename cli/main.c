#include "lib/portf.h"

#include <stdio.h>
#include <stdlib.h>

/*
  -S amc
  -Sf amc
  -Q amc
  -P
  -h
*/
int main(int argc, char *argv[]) {
  printf("%s\n", get_version());
  printf("%s\n", get_api_key());

  int count = 2;
  share_t *out = malloc(count * sizeof(share_t));
  out->name = "AMC";
  out->value = -1;

  (out + 1)->name = "AAPL";
  (out + 1)->value = -1;

  get_share(out, count);

  printf("%f\n", out->value);
  printf("%f\n", (out + 1)->value);
  free(out);
  return 0;
}
