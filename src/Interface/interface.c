#include <gtk/gtk.h>
#include <cairo.h>
#include <gdk/gdk.h>
#include "interface.h"
#include "../Structs/structs.h"

#define NUM_COUNTRIES 21
#define PATH "src/Images/"
#define WIDTH 700
#define HEIGHT 500
#define DIVISION 200
#define BORDER_SIZE 5

GtkWidget *win;
GtkWidget *drawing_area;
GtkWidget *btn_right_shift;
GtkWidget *btn_left_shift;
GtkLabel *lbl_current_country;
cairo_surface_t *surface;
double scale;

struct DoubleLinkedList* list;
struct Country* current_country;
typedef struct {
    const char *name;
    int x, y;
    int width, height;
    GdkPixbuf *original_pixbuf;
    GdkPixbuf *modified_pixbuf;
} CountryImage;

CountryImage countries[] = {
    {"Argentina", 395, 505, 0, 0, NULL},
    {"Belice", 230, 138, 0, 0, NULL},
    {"Bolivia", 376, 401, 0, 0, NULL},
    {"Brasil", 336, 273, 0, 0, NULL},
    {"Chile", 371, 470, 0, 0, NULL},
    {"Colombia", 285, 211, 0, 0, NULL},
    {"CostaRica", 250, 200, 0, 0, NULL},
    {"Ecuador", 266, 308, 0, 0, NULL},
    {"ElSalvador", 218, 170, 0, 0, NULL},
    {"Guatemala", 200, 141, 0, 0, NULL},
    {"GuayanaFrancesa", 509, 265, 0, 0, NULL},
    {"Guyana", 448, 246, 0, 0, NULL},
    {"Honduras", 227, 156, 0, 0, NULL},
    {"Mexico", 15, 15, 0, 0, NULL},
    {"Nicaragua", 240, 170, 0, 0, NULL},
    {"Panama", 276, 209, 0, 0, NULL},
    {"Paraguay", 450, 481, 0, 0, NULL},
    {"Peru", 265, 322, 0, 0, NULL},
    {"Surinam", 477, 264, 0, 0, NULL},
    {"Uruguay", 506, 577, 0, 0, NULL},
    {"Venezuela", 343, 218, 0, 0, NULL}
};

int min_distance_to_edge(int x, int y, int width, int height) {
    int dist = x;
    if (y < dist) dist = y;
    if (width - x < dist) dist = width - x;
    if (height - y < dist) dist = height - y;
    return dist;
}

int has_transparent_neighbor(guchar *pixels, int x, int y, int width, int height, 
                            int rowstride, int search_radius) {
    // search in a box "radius"
    for (int dy = -search_radius; dy <= search_radius; dy++) {
        for (int dx = -search_radius; dx <= search_radius; dx++) {
            if (dx == 0 && dy == 0) continue;
            
            int nx = x + dx;
            int ny = y + dy;
            
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                guchar *neighbor = pixels + ny * rowstride + nx * 4;
                if (neighbor[3] == 0) return 1; // Found transparent pixel
            }
        }
    }
    return 0;
}

void draw_img_border(GdkPixbuf *pixbuf) {
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar *pixel = pixels + y * rowstride + x * 4;

            if (pixel[3] == 0) continue; // Skip transparent pixels

            // Check if pixel is near edge or has transparent neighbors
            if (min_distance_to_edge(x, y, width, height) < BORDER_SIZE ||
                has_transparent_neighbor(pixels, x, y, width, height, rowstride, BORDER_SIZE)) {
                pixel[0] = 0; // R
                pixel[1] = 0; // G
                pixel[2] = 0; // B
            }
        }
    }
}



void update_img_corruption(GdkPixbuf *pixbuf, float corruptPercentage) {

    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    // Create new pixbuf for colored version
    // colored_pixels is a pointer to the first pixel...
    guchar *colored_pixels = gdk_pixbuf_get_pixels(pixbuf);
    int pixels_per_row = gdk_pixbuf_get_rowstride(pixbuf);

    // get the limit at which it starts painting red
    int split_y = (int)(height * corruptPercentage);

    // Transform each pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            // 4 bytes per pixel that's the reason por x * 4
            // and the pixel's per row, because, that's why pixels_per_row * y
            guchar *current_pixel = colored_pixels + (y * pixels_per_row) + x * 4;

            if (current_pixel[3] == 0) {
                // transparent pixel
                continue;
            }
            if (y > split_y) {
                // green
                current_pixel[0] = 76;
                current_pixel[1] = 185;
                current_pixel[2] = 68;
                
            } else {
                // red
                current_pixel[0] = 239;
                current_pixel[1] = 41;
                current_pixel[2] = 23;
            }
        }
    }
}

static void draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    // get the right most and lowest zone of the surface...
    // this is done by adding the position and the length
    int max_x = 0, max_y = 0, cord_x = 0, cord_y = 0;
    for (int i = 0; i < NUM_COUNTRIES; i++) {

        if (countries[i].original_pixbuf) {

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

    cairo_save(cr);
    cairo_scale(cr, scale, scale);

    // draw map
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
    cairo_paint(cr);

    // draw flags
    for (int i = 0; i < NUM_COUNTRIES; i++) {
        if (countries[i].original_pixbuf) {
            gdk_cairo_set_source_pixbuf(cr, countries[i].original_pixbuf, countries[i].x, countries[i].y);
            cairo_paint(cr);
        }
    }

    cairo_restore(cr);
}

void btn_right_shift_clicked(GtkWidget *widget, gpointer data) {
    if (current_country->next != NULL) {
        current_country = current_country->next;
        label_current_country_update(current_country->name);
    }
}

void btn_left_shift_clicked(GtkWidget *widget, gpointer data) {
    if (current_country->prev != NULL) {
        current_country = current_country->prev;
        label_current_country_update(current_country->name);
    }
}

void label_current_country_update(char *text) {
    gtk_label_set_text(lbl_current_country, text);
}


void start_window() {

    gtk_init(0, NULL);
    scale = 1.0;
    
    // create a surface for the drawing area
    //surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH - DIVISION, HEIGHT);
    surface = cairo_image_surface_create_from_png(PATH "blue_image.png");
    // load country images
    for (int i = 0; i < NUM_COUNTRIES; i++) {

        char path[200] = PATH;
        strcat(path, countries[i].name);
        strcat(path, ".png");
        countries[i].original_pixbuf = gdk_pixbuf_new_from_file(path, NULL);
        update_img_corruption(countries[i].original_pixbuf, 0.5);
        draw_img_border(countries[i].original_pixbuf);
        if (countries[i].original_pixbuf) {
            countries[i].width = gdk_pixbuf_get_width(countries[i].original_pixbuf);
            countries[i].height = gdk_pixbuf_get_height(countries[i].original_pixbuf);
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
    drawing_area = GTK_WIDGET(gtk_builder_get_object(builder, "drawing_area"));
    btn_right_shift = GTK_WIDGET(gtk_builder_get_object(builder, "btn_right_shift"));
    btn_left_shift = GTK_WIDGET(gtk_builder_get_object(builder, "btn_left_shift"));
    lbl_current_country = GTK_LABEL(gtk_builder_get_object(builder, "lbl_current_country"));

    //check if any widget is null

    list = initializeDoubleLinkedList();
    current_country = list->start;

    // Connect signals
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw), NULL);

    // connect buttons 
    g_signal_connect(btn_right_shift, "clicked", G_CALLBACK(btn_right_shift_clicked), NULL);
    g_signal_connect(btn_left_shift, "clicked", G_CALLBACK(btn_left_shift_clicked), NULL);
    gtk_widget_show_all(win);

    
    // free memory things...
    
    g_object_unref(builder);
    gtk_main();

    cairo_surface_destroy(surface);
    for (int i = 0; i < NUM_COUNTRIES; i++) {
        if (countries[i].original_pixbuf) {
            g_object_unref(countries[i].original_pixbuf);
        }
    }
}