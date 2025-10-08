#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>

/** */
void update_img_corruption(GtkWidget *image, float corruptPercentage);

/** */
void startWindow();

/** */
void btn_one_clicked(GtkWidget *widget, gpointer data);

/** */
void update_label(const char *text);

#endif // INTERFACE_H