typedef struct {
  const char *name;
  float value;
} share_t;

const char *get_version();
const char *get_api_key();

int get_share(share_t *out, int count);
