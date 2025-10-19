#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>


/** */
struct Country* find_country_by_name(const char* name);

/** */
void check_winner();

/** */
void update_solutions_text();

/** */
void start_window(int argc, char *argv[]);

/** */
void update_img_corruption(GdkPixbuf *pixbuf, float corruptPercentage);

/** */
int min_distance_to_edge(int x, int y, int width, int height);

/** */
int has_transparent_neighbor(guchar *pixels, int x, int y, int width, int height, 
                                          int rowstride, int search_radius);

/** */
void draw_img_border(GdkPixbuf *pixbuf);

/** */
static void draw(GtkWidget *widget, cairo_t *cr, gpointer data);
    
/** */
void btn_right_shift_clicked(GtkWidget *widget, gpointer data);

/** */
void btn_left_shift_clicked(GtkWidget *widget, gpointer data);

/** */
void label_current_country_update(char *text);

/** End player turn and start computer turns */
void end_player_turn();

/** Handle end turn button click */
void btn_end_turn_clicked(GtkWidget *widget, gpointer data);

/** Get country at mouse position */
struct Country* get_country_at_position(int x, int y);

/** Handle mouse clicks on the drawing area */
gboolean on_drawing_area_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data);

#endif // INTERFACE_H