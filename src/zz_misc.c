/*

IMAGE FONT BITS...

static GBitmap *clock_numbers[10];
static BitmapLayer *display_layer[4];

static GBitmap *WINDICONS;
static BitmapLayer *WINDLAYER;

static const int TOP = 4, BOTTOM = 85, LEFT = 0, RIGHT = 35, DISPLAY_OFFSET = 74;
static const int WIDTH = 32, HEIGHT = 78;
static const int IMAGE_X_OFFSET = 1, IMAGE_X_REPEAT = 36, IMAGE_Y_OFFSET = 1;

static void init_character(int character) {
  clock_numbers[character] = gbitmap_create_with_resource(RESOURCE_ID_NUMERALS_temp2_BLACK); //108 point image
  GRect rect = gbitmap_get_bounds(clock_numbers[character]);
  rect.size.w=WIDTH;
  rect.size.h=HEIGHT;
  rect.origin.x = IMAGE_X_OFFSET + 1 + (character * IMAGE_X_REPEAT);
  rect.origin.y = IMAGE_Y_OFFSET;
  gbitmap_set_bounds(clock_numbers[character], rect);
}

static BitmapLayer* init_display(int x, int y){
  BitmapLayer *display = bitmap_layer_create(GRect(x + DISPLAY_OFFSET, y, WIDTH, HEIGHT));
  bitmap_layer_set_compositing_mode(display,  GCompOpSet);
  layer_add_child(window_get_root_layer(parent_window), (Layer *)display);
  return display;
}

void draw_character(int character, BitmapLayer *destination){
  bitmap_layer_set_bitmap(destination, clock_numbers[character-48]);
}

void timedisplay_update(char *time_string) {
  for(int i = 0; i < 4; i++)
    draw_character(time_string[i], display_layer[i]);
}


tick handler code
  //static char time_string[5];
  //strftime(time_string, 5, "%I%M", tick_time);
  //timedisplay_update(time_string);

create code block
  for(int i = 0; i < 10; i++)
    init_character(i);
  
  display_layer[0] = init_display(LEFT, TOP);
  display_layer[1] = init_display(RIGHT, TOP);
  display_layer[2] = init_display(LEFT, BOTTOM);
  display_layer[3] = init_display(RIGHT, BOTTOM);


delete code block
  for(int i = 0; i < 10; i++)
    gbitmap_destroy(clock_numbers[i]);
  
  for(int i = 0; i < 4; i++)
    bitmap_layer_destroy(display_layer[i]);






*/