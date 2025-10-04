#include "interface.h"
#include <gtk/gtk.h>

GtkLabel *label;

void startWindow() {
    GtkBuilder *builder = NULL;
    GtkWidget *window = NULL;
    GtkWidget *button = NULL;
    GError *error = NULL;

    gtk_init(0, NULL);

    builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(builder, "src/Interface/main.glade", &error)) {
        g_warning("Error loading file: %s", error->message);
        g_error_free(error);
        return;
    }

    window = GTK_WIDGET(gtk_builder_get_object(builder, "Window"));
    if (!window) {
        g_warning("Could not get window from builder");
        return;
    }
    //set full screen
    gtk_window_fullscreen(GTK_WINDOW(window));

    button = GTK_WIDGET(gtk_builder_get_object(builder, "BtnOne"));
    label = GTK_LABEL(gtk_builder_get_object(builder, "LblOne"));

    GtkWidget *img_argentina = GTK_WIDGET(gtk_builder_get_object(builder, "ImgArgentina"));
    GtkWidget *img_belice = GTK_WIDGET(builder, "ImgBelice");
    GtkWidget *img_bolivia = GTK_WIDGET(builder, "ImgBolivia");

    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(button, "clicked", G_CALLBACK(btn_one_clicked), NULL);

    gtk_widget_show_all(window);

    gtk_main();

}

void btn_one_clicked(GtkWidget *widget, gpointer data) {
    update_label("Button clicked!");
}

void update_label(const char *text) {
    gtk_label_set_text(label, text);
}