#include "interface.h"
#include <gtk/gtk.h>

#define PATH "src/Images/"

GtkBuilder *builder = NULL;
GtkWidget *window = NULL;
GtkWidget *button = NULL;
GError *error = NULL;
GtkLabel *label = NULL;

void update_img_corruption(GtkWidget *image, float corruptPercentage) {

    // Convert it to an image and get the pixels
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));

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

void startWindow() {

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

    button = GTK_WIDGET(gtk_builder_get_object(builder, "BtnOne"));
    label = GTK_LABEL(gtk_builder_get_object(builder, "LblOne"));

    GTK_WIDGET(gtk_builder_get_object(builder, "ImgBrasil"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgChile"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgColombia"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgCostaRica"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgEcuador"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgElSalvador"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgGuatemala"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgGuayanaFrancesa"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgGuyana"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgHonduras"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgMexico"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgNicaragua"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgPanama"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgParaguay"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgPeru"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgSurinam"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgUruguay"));
    GTK_WIDGET(gtk_builder_get_object(builder, "ImgVenezuela"));

    // load all images
    GtkWidget **images = calloc(21, sizeof(GtkWidget*));

    images[0] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgArgentina"));
    images[1] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgBelice"));
    images[2] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgBolivia"));
    images[3] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgBrasil"));
    images[4] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgChile"));
    images[5] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgColombia"));
    images[6] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgCostaRica"));
    images[7] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgEcuador"));
    images[8] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgElSalvador"));
    images[9] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgGuatemala"));
    images[10] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgGuayanaFrancesa"));
    images[11] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgGuyana"));
    images[12] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgHonduras"));
    images[13] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgMexico"));
    images[14] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgNicaragua"));
    images[15] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgPanama"));
    images[16] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgParaguay"));
    images[17] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgPeru"));
    images[18] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgSurinam"));
    images[19] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgUruguay"));
    images[20] = GTK_WIDGET(gtk_builder_get_object(builder, "ImgVenezuela"));

    for (int i = 0; i < 21; i++) {
        update_img_corruption(images[i], 0.5);
    }


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