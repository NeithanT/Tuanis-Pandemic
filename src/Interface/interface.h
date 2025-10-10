#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>

/** */
void start_window();

/**W */
static void draw(GtkWidget *widget, cairo_t *cr, gpointer data);
    
/** */
void btn_one_clicked(GtkWidget *widget, gpointer data);

/** */
void label_one_update(const char *text);

#endif // INTERFACE_H