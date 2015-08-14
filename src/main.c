#include <pebble.h>

Window *window;
TextLayer *time_layer;
TextLayer *date_layer;
static GFont dead_pool_time_font;
static GFont dead_pool_date_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  static char time_buffer[] = "00:00";
  
  if (clock_is_24h_style()) {
    strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  text_layer_set_text(time_layer, time_buffer);

  static char date_buffer[] = "Thu, 5th Aug";
  strftime(date_buffer, sizeof("Thu, 5th Aug"), "%a, %d %b", tick_time);
  text_layer_set_text(date_layer, date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void window_init(Window *window) {
  dead_pool_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DEADPOOL_FONT_48));
  dead_pool_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DEADPOOL_FONT_14));
  
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_DEADPOOL_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  time_layer = text_layer_create(GRect(0, 104, 144, 48));
  text_layer_set_font(time_layer, dead_pool_time_font);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_background_color(time_layer, GColorClear);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));

  date_layer = text_layer_create(GRect(6, 150, 132, 18));
  text_layer_set_font(date_layer, dead_pool_date_font);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_background_color(date_layer, GColorClear);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
}

static void window_deinit(Window *window) {
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
  text_layer_destroy(time_layer);
}

void handle_init(void) {
  window = window_create();
  window_init(window);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  window_stack_push(window, true);
  update_time();
}

void handle_deinit(void) {
  window_deinit(window);
  window_destroy(window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
