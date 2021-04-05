#include "portf.h"

#include <cjson/cJSON.h>
#include <curl/curl.h>

#include <stdlib.h>
#include <string.h>

const char *SEPARATOR = "%2C";

const char *get_version() {
  return "0.1";
}

char *read_file(char *filename) {
  char *buffer = NULL;
  int string_size, read_size;
  FILE *handler = fopen(filename, "r");

  if (handler) {
    fseek(handler, 0, SEEK_END);
    string_size = ftell(handler);
    rewind(handler);

    buffer = (char *)malloc(sizeof(char) * (string_size + 1));
    read_size = fread(buffer, sizeof(char), string_size, handler);

    buffer[string_size] = '\0';

    if (string_size != read_size) {
      free(buffer);
      buffer = NULL;
    }
    fclose(handler);
  }

  return buffer;
}

const char *get_api_key() {
  char *key = read_file("/home/conni/.config/portf");
  key[strlen(key) - 1] = 0;
  return key;
}

typedef struct {
  char *ptr;
  size_t len;
} string;

void init_string(string *s) {
  s->len = 0;
  s->ptr = malloc(s->len + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, string *s) {
  size_t new_len = s->len + size * nmemb;
  s->ptr = realloc(s->ptr, new_len + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr + s->len, ptr, size * nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size * nmemb;
}

/* TODO(conni2461): request */
int get_share(share_t *out, int count) {
  CURL *curl;
  CURLcode res = 1;

  curl = curl_easy_init();
  if (curl) {
    string s;
    init_string(&s);

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    /* TODO(conni2461): Array separated symbols with %2 */
    char url[1024];
    strcpy(url, "https://apidojo-yahoo-finance-v1.p.rapidapi.com/market/v2/"
                "get-quotes?region=US&symbols=");
    for (int i = 0; i < count; i++) {
      strncat(url, (out + i)->name, strlen((out + i)->name));
      if (i != count - 1) {
        strncat(url, SEPARATOR, strlen(SEPARATOR));
      }
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* "https://apidojo-yahoo-finance-v1.p.rapidapi.com/stock/"
    "v2/get-suummary?region=US&symbol=AMC" */

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    struct curl_slist *headers = NULL;
    char key[64];
    strcpy(key, "x-rapidapi-key:");
    const char *api_key = get_api_key();
    strncat(key, api_key, strlen(api_key));

    headers = curl_slist_append(headers, key);
    headers = curl_slist_append(
        headers, "x-rapidapi-host: apidojo-yahoo-finance-v1.p.rapidapi.com");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);

    cJSON *json = cJSON_Parse(s.ptr);
    free(s.ptr);
    cJSON *results = json->child->child;

    /* quoteResponse -> result -> ARRAY -> regularMarketPrice */
    for (int i = 0; i < cJSON_GetArraySize(results); i++) {
      cJSON *item = cJSON_GetArrayItem(results, i);
      (out + i)->value =
          atof(cJSON_Print(cJSON_GetObjectItem(item, "regularMarketPrice")));
    }
    free(json);

    /* always cleanup */
    curl_easy_cleanup(curl);
  }

  return res;
}
