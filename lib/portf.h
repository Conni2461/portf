typedef struct {
  float change;
  float change_percent;
  float price;
  /* TODO(conni2461): time should be a long :thinking: */
  float time;
} value_t;

typedef struct {
  const char *symbol;
  const char *currency;
  const char *display_name;
  /* const char *market; */

  // fifty day stuff
  float fifty_day_average;
  float fifty_day_change;

  // pre
  // Are there no pre market information in the request response?
  /* value_t pre; */

  // regular
  value_t regular;

  float regular_high;
  float regular_low;
  float regular_open;
  float regular_prev_close;

  // post
  value_t post;

  // region
  // date
} stock_t;

const char *get_version();
const char *get_api_key();

void init_shares(stock_t *out, char **symbols, int count);
char *stock_to_string(const stock_t *stock);
int get_shares(stock_t *out, int count);
