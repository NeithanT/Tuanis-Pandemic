#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>

/** */
void start_window();

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

#endif // INTERFACE_H