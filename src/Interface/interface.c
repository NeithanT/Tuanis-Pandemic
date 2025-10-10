#include <gtk/gtk.h>
#include <cairo.h>
#include <gdk/gdk.h>

#define NUM_COUNTRIES 21
#define PATH "src/Images/"
#define WIDTH 700
#define HEIGHT 500
#define DIVISION 200
GtkWidget *win;
GtkWidget *drawingarea;
GtkWidget *update_button;
GtkLabel *info_label;
cairo_surface_t *surface;
double scale;

typedef struct {
    const char *name;
    int x, y;
    int width, height;
    GdkPixbuf *pixbuf;
} CountryImage;

CountryImage countries[] = {
    {"Argentina", 410, 532, 0, 0, NULL},
    {"Belice", 194, 112, 0, 0, NULL},
    {"Bolivia", 391, 422, 0, 0, NULL},
    {"Brasil", 348, 280, 0, 0, NULL},
    {"Chile", 384, 496, 0, 0, NULL},
    {"Colombia", 270, 220, 0, 0, NULL},
    {"CostaRica", 225, 181, 0, 0, NULL},
    {"Ecuador", 254, 313, 0, 0, NULL},
    {"ElSalvador", 184, 144, 0, 0, NULL},
    {"Guatemala", 174, 125, 0, 0, NULL},
    {"GuayanaFrancesa", 497, 258, 0, 0, NULL},
    {"Guyana", 442, 247, 0, 0, NULL},
    {"Honduras", 210, 135, 0, 0, NULL},
    {"Mexico", 10, 10, 0, 0, NULL},
    {"Nicaragua", 220, 153, 0, 0, NULL},
    {"Panama", 264, 192, 0, 0, NULL},
    {"Paraguay", 461, 508, 0, 0, NULL},
    {"Peru", 272, 339, 0, 0, NULL},
    {"Surinam", 470, 257, 0, 0, NULL},
    {"Uruguay", 505, 594, 0, 0, NULL},
    {"Venezuela", 351, 227, 0, 0, NULL}
};

static void draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    // get the right most and lowest zone of the surface...
    // this is done by adding the position and the length
    int max_x = 0, max_y = 0, cord_x = 0, cord_y = 0;
    for (int i = 0; i < NUM_COUNTRIES; i++) {

        if (countries[i].pixbuf) {

            cord_x = countries[i].x + countries[i].width;
            cord_y = countries[i].y + countries[i].height;
            if (cord_x > max_x) max_x = cord_x;
            if (cord_y > max_y) max_y = cord_y;
        }
    }

    // get the current available drawing area size
    // so after the window has been resized, this is the new size
    
    // compute scale
    // scale is always computed by getting the lowest proportion between
    // the new width and new height and the actual height and width
    // this is so the windows stays proportional and does not overflow
    // who cares 
    double scale_width = (double)width / max_x;
    double scale_height = (double)height / max_y;
    if (scale_height < scale_width) {
        scale = scale_height;
    } else {
        scale = scale_width;
    }

    // now we have the scale of how much bigger the new image is
    // center the content in the drawing area
    double dw = max_x * scale;
    double dh = max_y * scale;
    // printf every value to check

    printf("width: %d height: %d max_x: %d max_y: %d dw: %f dh: %f\n", width, height, max_x, max_y, dw, dh);

    double tx = (width - dw) / 2.0;
    double ty = (height - dh) / 2.0;

    cairo_save(cr);
    cairo_translate(cr, tx, ty);
    cairo_scale(cr, scale, scale);

    // draw map
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
    cairo_paint(cr);

    // draw flags
    for (int i = 0; i < NUM_COUNTRIES; i++) {
        if (countries[i].pixbuf) {
            gdk_cairo_set_source_pixbuf(cr, countries[i].pixbuf, countries[i].x, countries[i].y);
            cairo_paint(cr);
        }
    }

    cairo_restore(cr);
}

static void on_btn_one_clicked(GtkButton *button, gpointer user_data) {
    
    gtk_widget_queue_draw(drawingarea);
    
    // Update label
    gtk_label_set_text(info_label, "Corruption levels updated!");
}

void start_window() {

    gtk_init(0, NULL);
    scale = 1.0;
    
    // create a surface for the drawing area
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH - DIVISION, HEIGHT);
    
    // load country images
    for (int i = 0; i < NUM_COUNTRIES; i++) {

        char path[200] = PATH;
        strcat(path, countries[i].name);
        strcat(path, ".png");
        countries[i].pixbuf = gdk_pixbuf_new_from_file(path, NULL);
        if (countries[i].pixbuf) {
            countries[i].width = gdk_pixbuf_get_width(countries[i].pixbuf);
            countries[i].height = gdk_pixbuf_get_height(countries[i].pixbuf);
        }
    }

    // Load the Glade file
    GtkBuilder *builder = gtk_builder_new();
    GError *error = NULL;
    if (!gtk_builder_add_from_file(builder, "src/Interface/main.glade", &error)) {
        g_printerr("No se encontro el archivo glade: %s\n", error->message);
        g_clear_error(&error);
        return;
    }

    // Get widgets from the builder
    win = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    drawingarea = GTK_WIDGET(gtk_builder_get_object(builder, "drawing_area"));
    update_button = GTK_WIDGET(gtk_builder_get_object(builder, "btn_one"));
    info_label = GTK_LABEL(gtk_builder_get_object(builder, "lbl_one"));

    gtk_widget_set_size_request(drawingarea, 500, 500);
    // Connect signals
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(drawingarea, "draw", G_CALLBACK(draw), NULL);
    g_signal_connect(update_button, "clicked", G_CALLBACK(on_btn_one_clicked), drawingarea);

    gtk_widget_show_all(win);
    // free memory things...
    
    g_object_unref(builder);
    gtk_main();

    cairo_surface_destroy(surface);
    for (int i = 0; i < NUM_COUNTRIES; i++) {
        if (countries[i].pixbuf) {
            g_object_unref(countries[i].pixbuf);
        }
    }
}