#include <pebble.h>
#define KEY_VERSE 0
#define KEY_PASSAGE 1

  
static Window *s_main_window;

static TextLayer *s_time_layer;
static TextLayer *s_main_layer;
static TextLayer *s_def_layer;

static GFont s_time_font;
 static GFont s_word_font;
 static GFont s_main_font;
static GFont s_def_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 24h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
  //Create GBitmap, then set to created BitmapLayer
   s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BLACK_RECT);
   s_background_layer = bitmap_layer_create(GRect(0, 0, 100, 168));
   bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
   layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
// Create time TextLayer
    s_time_layer = text_layer_create(GRect(5, 148, 139, 50));
    text_layer_set_background_color(s_time_layer, GColorBlack);
    text_layer_set_text_color(s_time_layer, GColorClear);
    text_layer_set_text(s_time_layer, "00:00");
    
// Create word TextLayer
     s_main_layer = text_layer_create(GRect(6, 0, 139, 19));
     text_layer_set_background_color(s_main_layer, GColorBlack);
     text_layer_set_text_color(s_main_layer, GColorClear);
     //text_layer_set_text(s_main_layer, "Emblem:");
    
 // Create def TextLayer
     s_def_layer = text_layer_create(GRect(5, 19, 139, 132));
     text_layer_set_background_color(s_def_layer, GColorBlack);
     text_layer_set_text_color(s_def_layer, GColorClear);
     text_layer_set_text(s_def_layer, "Getting the verse of the day...");
    
// Create GFont
   // s_time_font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
     s_word_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_15));
   //  s_main_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
     s_def_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_13));
  
// Apply to TextLayer
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);
    
     text_layer_set_font(s_main_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
     text_layer_set_text_alignment(s_main_layer, GTextAlignmentCenter);
   
     text_layer_set_font(s_def_layer, s_def_font);
     text_layer_set_text_alignment(s_def_layer, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
     layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_main_layer));
     layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_def_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  //Unload GFont
    fonts_unload_custom_font(s_time_font);
     fonts_unload_custom_font(s_word_font);
     fonts_unload_custom_font(s_main_font);
     fonts_unload_custom_font(s_def_font);

  //Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
    
    //Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
       text_layer_destroy(s_main_layer);
       text_layer_destroy(s_def_layer);
}


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char verse_buffer[5000];
  static char passage_buffer[5000];
//  static char entire_layer_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_VERSE:
      snprintf(verse_buffer, sizeof(verse_buffer), "%s", t->value->cstring);
      break;
    case KEY_PASSAGE:
      snprintf(passage_buffer, sizeof(passage_buffer), "%s", t->value->cstring);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble full string and display
//   snprintf(entire_layer_buffer, sizeof(entire_layer_buffer), "%s, %s", verse_buffer, passage_buffer);
  snprintf(verse_buffer, sizeof(verse_buffer), "%s", verse_buffer);
  snprintf(passage_buffer, sizeof(passage_buffer), "%s", passage_buffer);
  
  
  text_layer_set_text(s_main_layer, passage_buffer);
  text_layer_set_text(s_def_layer, verse_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
