#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>

/** */
void start_window();

/** */
void update_img_corruption(GdkPixbuf *pixbuf, float corruptPercentage);

/** */
static void draw(GtkWidget *widget, cairo_t *cr, gpointer data);
    
/** */
void btn_right_shift_clicked(GtkWidget *widget, gpointer data);

/** */
void btn_left_shift_clicked(GtkWidget *widget, gpointer data);

/** */
void label_current_country_update(char *text);

#endif // INTERFACE_H