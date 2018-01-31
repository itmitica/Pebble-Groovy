#include <pebble.h>
  
Window *MainWindow;

static GBitmap *s_res_image_wallpaper;
static BitmapLayer *Wallpaper;

TextLayer *TimeText;
TextLayer *DateText;
TextLayer *DayText;
TextLayer *LineText;

GFont TimeFont;
GFont DateFont;
GFont DayFont;
GFont LineFont;

typedef enum days { Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday} Day;

Day convert(const char *str)
{
  if(strcmp(str,"Monday") == 0) return Monday;
  else if(strcmp(str, "Tuesday") == 0) return Tuesday;
  else if(strcmp(str, "Wednesday") == 0) return Wednesday;
  else if(strcmp(str, "Thursday") == 0) return Thursday;
  else if(strcmp(str, "Friday") == 0) return Friday;
  else if(strcmp(str, "Saturday") == 0) return Saturday;
    
  return Sunday;
};

void update_date() {
  // Other types declaration
  Day today;
  
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char date[14];
  static char day[25];
  
    // Write the current date into the buffer
    strftime(date, sizeof(date), "%d  %b", tick_time);
    
    // Write the current day into the buffer
    strftime(day, sizeof(day), "%A", tick_time);
    // Get the integer day value
    today = convert(day);
      
    // Display this date on the TextLayer
    text_layer_set_text(DateText, date);
    
    // Display this line on the LineLayer
    // Friday is an exception
    int weekdaytowidth = 0;
    weekdaytowidth = 36 + 18*today;
    
    if ((today == 3) || (today == 5)) {
      text_layer_set_size(LineText, GSize(weekdaytowidth - 2, 18));
    } else {
      text_layer_set_size(LineText, GSize(weekdaytowidth, 18));
    }
  
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Returned week day: %s", day);
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Returned week day: %d", strcmp(day, "Thursday"));
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Returned week day: %d", today);
}
 
void update_time() {  
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char time[10];
    
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 24h hour format
    strftime(time, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(time, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  // Display this time on the TextLayer
  text_layer_set_text(TimeText, time);
  
  if (tick_time->tm_hour == 0) {
    update_date();
  }
}
  
void main_window_load(Window *window) {
  window_set_background_color(MainWindow, GColorBlack);
  s_res_image_wallpaper = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WALLPAPER);
  
  // Wallpaper
  Wallpaper = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(Wallpaper, s_res_image_wallpaper);
  layer_add_child(window_get_root_layer(window), (Layer *)Wallpaper);
  
  // Date Text
  DateText = text_layer_create(GRect(0, 20, 140, 36));
  text_layer_set_background_color(DateText, GColorClear);
  text_layer_set_text_color(DateText, GColorClear);
  DateFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_30));
  text_layer_set_font(DateText, DateFont);
  text_layer_set_text_alignment(DateText, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(DateText));

    // Day Text
  DayText = text_layer_create(GRect(0, 68, 140, 14));
  text_layer_set_background_color(DayText, GColorClear);
  text_layer_set_text_color(DayText, GColorClear);
  text_layer_set_text(DayText, "M T W T F S S");
  DayFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_RAVE_BOLD_14));
  text_layer_set_font(DayText, DayFont);
  text_layer_set_text_alignment(DayText, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(DayText));
  
    // Line Text
  LineText = text_layer_create(GRect(0, 70, 36, 18));
  text_layer_set_background_color(LineText, GColorClear);
  text_layer_set_text_color(LineText, GColorClear);
  text_layer_set_text(LineText, "_");
  LineFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_RAVE_BOLD_14));
  text_layer_set_font(LineText, LineFont);
  text_layer_set_text_alignment(LineText, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(LineText));
  
  // Time Text
  TimeText = text_layer_create(GRect(0, 108, 140, 54));
  text_layer_set_background_color(TimeText, GColorClear);
  text_layer_set_text_color(TimeText, GColorClear);
  TimeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_44));
  text_layer_set_font(TimeText, TimeFont);
  text_layer_set_text_alignment(TimeText, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(TimeText));
  
  // Make sure date and time are displayed from the start
  update_date();
  update_time();
}


void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(TimeText);
  text_layer_destroy(DateText);
  text_layer_destroy(DayText);
  text_layer_destroy(LineText);
  
  // Destroy Font
  fonts_unload_custom_font(TimeFont);
  fonts_unload_custom_font(DateFont);
  fonts_unload_custom_font(DayFont);
  fonts_unload_custom_font(LineFont);
}
  
void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


void init() {
  // Create main Window element and assign to pointer
  MainWindow = window_create();
  window_set_background_color(MainWindow, GColorWhite);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(MainWindow, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(MainWindow, false);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void deinit() {
  // Destroy Window
  window_destroy(MainWindow);
  bitmap_layer_destroy(Wallpaper);
  gbitmap_destroy(s_res_image_wallpaper);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}