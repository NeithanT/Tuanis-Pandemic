#include <gtk/gtk.h>
#include <cairo.h>
#include <gdk/gdk.h>
#include "interface.h"
#include "../Structs/structs.h"
#include "../Structs/Hash/Hash.h"
#include "../Play/Play.h"
#include "../Structs/Player/Player.h"

#define NUM_COUNTRIES 21
#define PATH "src/Images/"
#define WIDTH 1000
#define HEIGHT 800
#define DIVISION 200
#define BORDER_SIZE 5

GtkWidget *win;
GtkWidget *drawing_area;
GtkWidget *drawing_current_country;
GtkWidget *btn_right_shift;
GtkWidget *btn_left_shift;
GtkLabel *lbl_current_country;
GtkLabel *lbl_poverty;
GtkLabel *lbl_crime;
GtkLabel *lbl_corruption;
GtkLabel *lbl_unemployment;
GtkLabel *lbl_solutions;
GtkLabel *lbl_game_status;
GtkLabel *lbl_political_stability;
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

// Game elements
struct hastTable* hash_table;
struct Player* player;
struct Player* ally;

// Game state
typedef enum {
    TURN_PLAYER_MOVE,
    TURN_PLAYER_ACTION,
    TURN_CORRUPTION,
    TURN_ALLY,
    GAME_OVER
} GameState;

GameState current_game_state = TURN_PLAYER_MOVE;
int player_actions_remaining = 4; // Player can do 4 actions per turn

int is_connected(struct Country* country1, struct Country* country2) {
    if (!country1 || !country2 || !country1->connected_countries) return 0;
    struct Country* current = country1->connected_countries->start;
    while (current) {
        if (current == country2) return 1;
        current = current->next_connected;
    }
    return 0;
}

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

void get_country_position(const char* name, int* x, int* y) {
    for (int i = 0; i < NUM_COUNTRIES; i++) {
        if (strcmp(countries[i].name, name) == 0) {
            *x = countries[i].x;
            *y = countries[i].y;
            return;
        }
    }
    *x = 0;
    *y = 0;
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
            struct Country* country_data = find_country_by_name(countries[i].name);
            if (country_data) {
                // Create a copy of the pixbuf to modify
                GdkPixbuf* temp_pixbuf = gdk_pixbuf_copy(countries[i].original_pixbuf);
                if (temp_pixbuf) {
                    // Show corruption for current country and connected ones
                    if (country_data == current_country || is_connected(current_country, country_data)) {
                        update_img_corruption(temp_pixbuf, country_data->corruption);
                    }
                    
                    // Highlight current country with border
                    if (country_data == current_country) {
                        draw_img_border(temp_pixbuf);
                    }
                    
                    gdk_cairo_set_source_pixbuf(cr, temp_pixbuf, countries[i].x, countries[i].y);
                    cairo_paint(cr);
                    g_object_unref(temp_pixbuf);
                }
            } else {
                gdk_cairo_set_source_pixbuf(cr, countries[i].original_pixbuf, countries[i].x, countries[i].y);
                cairo_paint(cr);
            }
        }
    }

    // Draw player and ally positions
    if (player && player->current_country) {
        int px, py;
        get_country_position(player->current_country->name, &px, &py);
        cairo_set_source_rgb(cr, 0, 0, 1); // Blue for player
        cairo_arc(cr, px + 25, py + 25, 10, 0, 2 * G_PI);
        cairo_fill(cr);
    }
    if (ally && ally->current_country) {
        int ax, ay;
        get_country_position(ally->current_country->name, &ax, &ay);
        cairo_set_source_rgb(cr, 0, 1, 0); // Green for ally
        cairo_arc(cr, ax + 25, ay + 25, 10, 0, 2 * G_PI);
        cairo_fill(cr);
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
    if (current_country) {
        char buffer[200];
        
        // Game state info
        if (lbl_game_status) {
            if (current_game_state == TURN_PLAYER_MOVE) {
                sprintf(buffer, "Tu turno: MOVIMIENTO - Haz clic en un país conectado (azul/verde)");
            } else if (current_game_state == TURN_PLAYER_ACTION) {
                sprintf(buffer, "Tu turno: ACCIONES (%d restantes) - Reduce los problemas", player_actions_remaining);
            } else if (current_game_state == TURN_CORRUPTION || current_game_state == TURN_ALLY) {
                sprintf(buffer, "Turno del oponente...");
            } else {
                sprintf(buffer, "Juego terminado");
            }
            gtk_label_set_text(lbl_game_status, buffer);
        }
        
        sprintf(buffer, "Pobreza: %d/3", current_country->poverty);
        gtk_label_set_text(lbl_poverty, buffer);
        sprintf(buffer, "Crimen: %d/3", current_country->crime);
        gtk_label_set_text(lbl_crime, buffer);
        sprintf(buffer, "Corrupcion: %.0f%%", current_country->corruption * 100);
        gtk_label_set_text(lbl_corruption, buffer);
        sprintf(buffer, "Desempleo: %d/3", current_country->unemployment);
        gtk_label_set_text(lbl_unemployment, buffer);
        
        if (lbl_political_stability) {
            sprintf(buffer, "Estabilidad Política: %d%%", current_country->political_stability);
            gtk_label_set_text(lbl_political_stability, buffer);
        }
        
        // Update solutions
        update_solutions_text();
        
        // Redraw the current country image
        if (drawing_current_country) {
            gtk_widget_queue_draw(drawing_current_country);
        }
    }
}

void btn_end_turn_clicked(GtkWidget *widget, gpointer data) {
    if (current_game_state == TURN_PLAYER_ACTION) {
        end_player_turn();
    }
}

void update_solutions_text() {
    if (!hash_table || !current_country || !current_country->name) return;
    
    struct listSolutions* solutions = searchByKey(hash_table, current_country->name);
    if (solutions && solutions->start) {
        char buffer[1000] = "Soluciones:\n";
        struct nodeList* current = solutions->start;
        while (current) {
            if (current->solutionString) {
                strcat(buffer, current->solutionString);
                strcat(buffer, "\n");
            }
            current = current->nextNode;
        }
    }
}

void btn_action_poverty_clicked(GtkWidget *widget, gpointer data) {
    if (current_country && player_actions_remaining > 0 && current_game_state == TURN_PLAYER_ACTION) {
        // Check if player is in this country or connected to it
        if (current_country == player->current_country || 
            is_connected(player->current_country, current_country)) {
            if (current_country->poverty > 0) {
                current_country->poverty--;
                player_actions_remaining--;
                calculateCorruption(current_country);
                label_current_country_update(current_country->name);
                gtk_widget_queue_draw(drawing_area);
                
                if (player_actions_remaining <= 0) {
                    end_player_turn();
                }
            }
        }
    }
}

void btn_action_crime_clicked(GtkWidget *widget, gpointer data) {
    if (current_country && player_actions_remaining > 0 && current_game_state == TURN_PLAYER_ACTION) {
        if (current_country == player->current_country || 
            is_connected(player->current_country, current_country)) {
            if (current_country->crime > 0) {
                current_country->crime--;
                player_actions_remaining--;
                calculateCorruption(current_country);
                label_current_country_update(current_country->name);
                gtk_widget_queue_draw(drawing_area);
                
                if (player_actions_remaining <= 0) {
                    end_player_turn();
                }
            }
        }
    }
}

void btn_action_inequality_clicked(GtkWidget *widget, gpointer data) {
    if (current_country && player_actions_remaining > 0 && current_game_state == TURN_PLAYER_ACTION) {
        if (current_country == player->current_country || 
            is_connected(player->current_country, current_country)) {
            if (current_country->unemployment > 0) {
                current_country->unemployment--;
                player_actions_remaining--;
                calculateCorruption(current_country);
                label_current_country_update(current_country->name);
                gtk_widget_queue_draw(drawing_area);
                
                if (player_actions_remaining <= 0) {
                    end_player_turn();
                }
            }
        }
    }
}

void btn_action_political_weakness_clicked(GtkWidget *widget, gpointer data) {
    if (current_country && player_actions_remaining > 0 && current_game_state == TURN_PLAYER_ACTION) {
        if (current_country->political_stability < 100) {
            current_country->political_stability += 25;
            if (current_country->political_stability > 100) current_country->political_stability = 100;
            player_actions_remaining--;
            label_current_country_update(current_country->name);
            gtk_widget_queue_draw(drawing_area);
            
            if (player_actions_remaining <= 0) {
                end_player_turn();
            }
        }
    }
}

// Function to get country at mouse position
struct Country* get_country_at_position(int x, int y) {
    // Adjust for scale
    int scaled_x = (int)(x / scale);
    int scaled_y = (int)(y / scale);
    
    // Check each country's bounds
    for (int i = 0; i < NUM_COUNTRIES; i++) {
        if (countries[i].original_pixbuf) {
            int cx = countries[i].x;
            int cy = countries[i].y;
            int cw = countries[i].width;
            int ch = countries[i].height;
            
            if (scaled_x >= cx && scaled_x < cx + cw && 
                scaled_y >= cy && scaled_y < cy + ch) {
                
                // Check if pixel is not transparent
                int local_x = scaled_x - cx;
                int local_y = scaled_y - cy;
                
                if (local_x >= 0 && local_x < cw && local_y >= 0 && local_y < ch) {
                    guchar *pixels = gdk_pixbuf_get_pixels(countries[i].original_pixbuf);
                    int rowstride = gdk_pixbuf_get_rowstride(countries[i].original_pixbuf);
                    guchar *pixel = pixels + local_y * rowstride + local_x * 4;
                    
                    if (pixel[3] > 0) { // Not transparent
                        return find_country_by_name(countries[i].name);
                    }
                }
            }
        }
    }
    return NULL;
}

// Mouse click handler for the map
gboolean on_drawing_area_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if (event->button == 1) { // Left click
        struct Country* clicked_country = get_country_at_position(event->x, event->y);
        
        if (clicked_country) {
            printf("Clicked on: %s\n", clicked_country->name);
            
            if (current_game_state == TURN_PLAYER_MOVE) {
                // Check if player can move to this country
                if (clicked_country == player->current_country) {
                    printf("Already in this country, starting action phase.\n");
                    current_country = clicked_country;
                    label_current_country_update(current_country->name);
                    
                    // Transition to action phase
                    current_game_state = TURN_PLAYER_ACTION;
                    player_actions_remaining = 4;
                    
                    gtk_widget_queue_draw(drawing_area);
                } else if (is_connected(player->current_country, clicked_country)) {
                    printf("Moving from %s to %s\n", player->current_country->name, clicked_country->name);
                    player->current_country = clicked_country;
                    current_country = clicked_country;
                    label_current_country_update(current_country->name);
                    
                    // Transition to action phase
                    current_game_state = TURN_PLAYER_ACTION;
                    player_actions_remaining = 4;
                    
                    gtk_widget_queue_draw(drawing_area);
                } else {
                    printf("Cannot move to %s - not connected to %s\n", clicked_country->name, player->current_country->name);
                }
            } else if (current_game_state == TURN_PLAYER_ACTION) {
                // Just update the view to this country
                current_country = clicked_country;
                label_current_country_update(current_country->name);
                gtk_widget_queue_draw(drawing_area);
            }
        }
    }
    return TRUE;
}

void end_player_turn() {
    printf("Player turn ended. Starting corruption turn.\n");
    current_game_state = TURN_CORRUPTION;
    
    // Corruption spreads
    turnCorruption(list);
    calculateCorruptionCountryList(list);
    
    // Clean dead countries
    while (!eraseDeadCountries(list));
    
    // Check for winner
    int winner = verifyWinner(list);
    if (winner != 2) {
        current_game_state = GAME_OVER;
        check_winner();
        return;
    }
    
    // Ally turn
    printf("Starting ally turn.\n");
    current_game_state = TURN_ALLY;
    turnAlly(ally);
    
    // Corruption spreads again
    turnCorruption(list);
    calculateCorruptionCountryList(list);
    
    // Clean dead countries
    while (!eraseDeadCountries(list));
    
    // Check for winner
    winner = verifyWinner(list);
    if (winner != 2) {
        current_game_state = GAME_OVER;
        check_winner();
        return;
    }
    
    // Back to player turn
    printf("Starting player turn.\n");
    current_game_state = TURN_PLAYER_MOVE;
    player_actions_remaining = 4;
    
    // Update display
    if (player && player->current_country) {
        current_country = player->current_country;
    }
    label_current_country_update(current_country->name);
    gtk_widget_queue_draw(drawing_area);
}

void check_winner() {
    int winner = verifyWinner(list);
    if (winner != 2) {
        char msg[200];
        if (winner == 0) {
            strcpy(msg, "¡Felicidades! Has ganado. Todos los países tienen al menos un problema resuelto.");
        } else {
            sprintf(msg, "La corrupción ha ganado. Solo quedan %d países.", lengthDoubleLinkedList(list));
        }
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(win), 
                                                     GTK_DIALOG_MODAL, 
                                                     GTK_MESSAGE_INFO, 
                                                     GTK_BUTTONS_OK, 
                                                     "%s", msg);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        gtk_main_quit();
    }
}

struct Country* find_country_by_name(const char* name) {
    struct Country* current = list->start;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}


void start_window(int argc, char *argv[]) {

    gtk_init(&argc, &argv);
    printf("GTK initialized\n");
    printf("Starting window\n");
    scale = 1.0;
    
    // create a surface for the drawing area
    //surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH - DIVISION, HEIGHT);
    surface = cairo_image_surface_create_from_png(PATH "blue_image.png");
    if (!surface) {
        printf("Failed to create surface from png\n");
        return;
    }
    printf("Surface created\n");
    // load country images
    for (int i = 0; i < NUM_COUNTRIES; i++) {

        char path[200] = PATH;
        strcat(path, countries[i].name);
        strcat(path, ".png");
        countries[i].original_pixbuf = gdk_pixbuf_new_from_file(path, NULL);
        // Don't update corruption here, do it in draw
        if (countries[i].original_pixbuf) {
            countries[i].width = gdk_pixbuf_get_width(countries[i].original_pixbuf);
            countries[i].height = gdk_pixbuf_get_height(countries[i].original_pixbuf);
        }
    }
    printf("Pixbufs loaded\n");

    // Load the Glade file
    GtkBuilder *builder = gtk_builder_new();
    GError *error = NULL;
    if (!gtk_builder_add_from_file(builder, "/home/nei/Desktop/Estructuras/Tuanis-Pandemic/src/Interface/main.glade", &error)) {
        g_printerr("No se encontro el archivo glade: %s\n", error->message);
        g_clear_error(&error);
        return;
    }
    printf("Glade loaded\n");

    // Initialize game elements
    hash_table = createNewHashTable();
    //populateHashTable(hash_table);
    list = initializeDoubleLinkedList();
    
    // Calculate initial corruption for all countries
    calculateCorruptionCountryList(list);
    
    player = allocateInitialPlayerOnMap(list);
    ally = allocateInitialPlayerOnMap(list);
    
    // Make sure ally is in a different country
    while (ally->current_country == player->current_country) {
        ally = allocateInitialPlayerOnMap(list);
    }
    
    // Set game state
    current_game_state = TURN_PLAYER_MOVE;
    player_actions_remaining = 4;

    // Get widgets from the builder
    win = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    drawing_area = GTK_WIDGET(gtk_builder_get_object(builder, "drawing_area"));
    drawing_current_country = GTK_WIDGET(gtk_builder_get_object(builder, "drawing_current_country"));
    btn_right_shift = GTK_WIDGET(gtk_builder_get_object(builder, "btn_right_shift"));
    btn_left_shift = GTK_WIDGET(gtk_builder_get_object(builder, "btn_left_shift"));
    lbl_current_country = GTK_LABEL(gtk_builder_get_object(builder, "lbl_current_country"));
    lbl_poverty = GTK_LABEL(gtk_builder_get_object(builder, "lbl_poverty"));
    lbl_crime = GTK_LABEL(gtk_builder_get_object(builder, "lbl_crime"));
    lbl_corruption = GTK_LABEL(gtk_builder_get_object(builder, "lbl_corruption"));
    lbl_unemployment = GTK_LABEL(gtk_builder_get_object(builder, "lbl_unemployment"));
    lbl_game_status = GTK_LABEL(gtk_builder_get_object(builder, "lbl_game_status"));
    lbl_political_stability = GTK_LABEL(gtk_builder_get_object(builder, "lbl_political_stability"));
    //textview_solutions = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "textview_solutions"));

    // Check if widgets are null (optional ones can be NULL)
    if (!win || !drawing_area || !drawing_current_country || !btn_right_shift || !btn_left_shift || !lbl_current_country ||
        !lbl_poverty || !lbl_crime || !lbl_corruption || !lbl_unemployment /*|| !textview_solutions*/) {
        g_printerr("Error: Some widgets not found in glade file\n");
        return;
    }
    printf("Widgets retrieved\n");

    // Get action buttons
    GtkWidget *btn_action_poverty = GTK_WIDGET(gtk_builder_get_object(builder, "btn_action_poverty"));
    GtkWidget *btn_action_crime = GTK_WIDGET(gtk_builder_get_object(builder, "btn_action_crime"));
    GtkWidget *btn_action_unemployment = GTK_WIDGET(gtk_builder_get_object(builder, "btn_action_unemployment"));
    GtkWidget *btn_action_political_weakness = GTK_WIDGET(gtk_builder_get_object(builder, "btn_action_political_weakness"));

    // Check if widgets are null
    if (!win || !drawing_area || !drawing_current_country || !btn_right_shift || !btn_left_shift || !lbl_current_country ||
        !lbl_poverty || !lbl_crime || !lbl_corruption || !lbl_unemployment ||
        !btn_action_poverty || !btn_action_crime || !btn_action_unemployment || !btn_action_political_weakness) {
        g_printerr("Error: Some widgets not found in glade file\n");
        return;
    }

    // Get layout
    GtkLayout *layout_game = GTK_LAYOUT(gtk_builder_get_object(builder, "layout_game"));
    if (!layout_game) {
        g_printerr("Error: layout_game not found\n");
        return;
    }

    current_country = player->current_country;
    label_current_country_update(current_country->name);

    // Connect signals
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw), NULL);
    g_signal_connect(drawing_current_country, "draw", G_CALLBACK(draw), NULL);
    
    // Enable button press events for the drawing area
    gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_drawing_area_button_press), NULL);

    // connect buttons 
    g_signal_connect(btn_right_shift, "clicked", G_CALLBACK(btn_right_shift_clicked), NULL);
    g_signal_connect(btn_left_shift, "clicked", G_CALLBACK(btn_left_shift_clicked), NULL);
    g_signal_connect(btn_action_poverty, "clicked", G_CALLBACK(btn_action_poverty_clicked), NULL);
    g_signal_connect(btn_action_crime, "clicked", G_CALLBACK(btn_action_crime_clicked), NULL);
    g_signal_connect(btn_action_unemployment, "clicked", G_CALLBACK(btn_action_inequality_clicked), NULL);
    g_signal_connect(btn_action_political_weakness, "clicked", G_CALLBACK(btn_action_political_weakness_clicked), NULL);
    
    // Try to get end turn button if it exists
    GtkWidget *btn_end_turn = GTK_WIDGET(gtk_builder_get_object(builder, "btn_end_turn"));
    if (btn_end_turn) {
        g_signal_connect(btn_end_turn, "clicked", G_CALLBACK(btn_end_turn_clicked), NULL);
    }
    
    gtk_widget_show_all(win);
    gtk_window_present(GTK_WINDOW(win));
    printf("Window shown\n");

    
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