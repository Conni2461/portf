#include "portf.h"

#include <cjson/cJSON.h>
#include <curl/curl.h>

#include <stdlib.h>
#include <string.h>

#define json_to_str(obj, name)                                                 \
  cJSON_GetStringValue(cJSON_GetObjectItem(obj, name))
#define json_to_float(obj, name)                                               \
  cJSON_GetNumberValue(cJSON_GetObjectItem(obj, name))

#define DYNAMIC_STR_SIZE 1024

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
  size_t cap;
  size_t len;
} string_t;

void init_string_with(string_t *s, size_t cap) {
  s->len = 0;
  s->cap = cap;
  s->ptr = malloc(s->cap + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

void init_string(string_t *s) {
  init_string_with(s, 0);
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, string_t *s) {
  size_t new_len = s->len + size * nmemb;
  s->ptr = realloc(s->ptr, new_len + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr + s->len, ptr, size * nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;
  s->cap = new_len;

  return size * nmemb;
}

void append_str(string_t *string, const char *src) {
  unsigned long len = strlen(src);
  while (string->len + len > string->cap) {
    /* TODO(conni2461): Maybe not double it maybe +1024, fixed DEFINE */
    string->cap *= 2;
    string->ptr = realloc(string->ptr, sizeof(char) * string->cap);
    /* TODO(conni2461): LOGGING */
    /* printf("RESIZING...\n"); */
    if (string->ptr == NULL) {
      /* TODO(conni2461): LOGGING */
      fprintf(stderr, "RESIZING FAILED...\n");
      exit(EXIT_FAILURE);
    }
  }
  strncat(string->ptr, src, len);
  string->len += len;
}

void append_float(string_t *string, float src, const char *format) {
  char number[32];
  snprintf(number, 32, format, src);
  return append_str(string, number);
}

void init_shares(stock_t *out, char **symbols, int count) {
  for (int i = 0; i < count; i++) {
    (out + i)->symbol = symbols[i];
    /* TODO(conni2461): maybe i want to do here more */
  }
}

char *stock_to_string(const stock_t *stock) {
  string_t s;
  init_string_with(&s, DYNAMIC_STR_SIZE);
  /* output: display_name: $price (change, percentage)\n */
  append_str(&s, stock->display_name);
  append_str(&s, ": $");
  append_float(&s, stock->regular.price, "%.2f");
  append_str(&s, " ($");
  append_float(&s, stock->regular.change, "%+.2f");
  append_str(&s, ", ");
  append_float(&s, stock->regular.change_percent, "%+.2f");
  append_str(&s, "%)\n");

  return s.ptr;
}

int get_shares(stock_t *out, int count) {
  CURL *curl;
  CURLcode res = 1;

  curl = curl_easy_init();
  if (curl) {
    string_t s;
    init_string(&s);

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

    string_t url;
    init_string_with(&url, DYNAMIC_STR_SIZE);
    append_str(&url,
               "https://yahoo-finance-low-latency.p.rapidapi.com/v6/finance/"
               "quote?symbols=");
    for (int i = 0; i < count; i++) {
      append_str(&url, (out + i)->symbol);
      if (i != count - 1) {
        append_str(&url, SEPARATOR);
      }
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.ptr);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    struct curl_slist *headers = NULL;
    char key[64];
    strcpy(key, "x-rapidapi-key:");
    const char *api_key = get_api_key();
    strncat(key, api_key, strlen(api_key));

    headers = curl_slist_append(headers, key);
    headers = curl_slist_append(
        headers, "x-rapidapi-host: yahoo-finance-low-latency.p.rapidapi.com");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);

    cJSON *json = cJSON_Parse(s.ptr);
    /* quoteResponse -> result -> ARRAY -> regularMarketPrice */
    cJSON *results = json->child->child;
    for (int i = 0; i < cJSON_GetArraySize(results); i++) {
      cJSON *item = cJSON_GetArrayItem(results, i);
      (out + i)->display_name = json_to_str(item, "displayName");
      (out + i)->currency = json_to_str(item, "currency");
      (out + i)->regular.price = json_to_float(item, "regularMarketPrice");
      (out + i)->regular.change = json_to_float(item, "regularMarketChange");
      (out + i)->regular.change_percent =
          json_to_float(item, "regularMarketChangePercent");
    }

    /* cleanup memory */
    free(url.ptr);
    free(s.ptr);
    free(json);

    /* always cleanup */
    curl_easy_cleanup(curl);
  }

  return res;
}
