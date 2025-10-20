#include <gtk/gtk.h>
#include <cairo.h>
#include <gdk/gdk.h>
#include <stdarg.h>
#include "interface.h"
#include "../Structs/structs.h"
#include "../Structs/Hash/Hash.h"
#include "../Play/Play.h"
#include "../Structs/Player/Player.h"
#include "../Structs/Random/random.h"

#define NUM_COUNTRIES 21
#define PATH "src/Images/"
#define BORDER_SIZE 5
#define MAX_ACTION_MSG_LEN 200
#define MAX_BUFFER_LEN 1000
#define COUNTRY_DETAIL_WIDTH 300
#define COUNTRY_DETAIL_HEIGHT 300

GtkWidget *win;
GtkBox *box_right_panel;
GtkWidget *drawing_area;
GtkWidget *drawing_current_country;
GtkWidget *btn_right_shift;
GtkWidget *btn_left_shift;
GtkWidget *btn_country;
GtkWidget *btn_action_poverty;
GtkWidget *btn_action_crime;
GtkWidget *btn_action_unemployment;
GtkWidget *btn_action_political_weakness;
GtkLabel *lbl_current_country;
GtkLabel *lbl_poverty;
GtkLabel *lbl_crime;
GtkLabel *lbl_corruption;
GtkLabel *lbl_unemployment;
GtkLabel *lbl_game_status;
GtkLabel *lbl_political_stability;
GtkTextView *textview_messages;
GtkTextBuffer *textbuffer_messages;
cairo_surface_t *surface;
cairo_surface_t *surface_cur_country;

struct Country* current_country;

// Game elements
struct DoubleLinkedList* list;
struct hastTable* hash_table;
struct Player* player;
double scale;
// Game state
typedef enum {
    SELECT_STARTING_COUNTRY,
    TURN_PLAYER_MOVE,
    TURN_PLAYER_ACTION,
    TURN_CORRUPTION,
    GAME_OVER
} GameState;


struct CountryImage countries[] = {
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

GameState current_game_state = SELECT_STARTING_COUNTRY;

int player_actions_remaining = 4; // Player can do 4 actions per turn

// Forward declarations
void update_current_country_surface();
int is_country_valid(struct Country* country);
void restart_game();
struct Country* find_country_by_name(struct DoubleLinkedList* list, const char* name);

// Function to add messages to the message log
void add_message(const char* message) {
    if (!message) return;
    
    if (!textbuffer_messages) {
        return;
    }
    
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(textbuffer_messages, &end);
    
    // Add timestamp or turn indicator if desired
    gtk_text_buffer_insert(textbuffer_messages, &end, message, -1);
    gtk_text_buffer_insert(textbuffer_messages, &end, "\n", -1);
    
    // Auto-scroll to the end
    GtkTextMark *mark = gtk_text_buffer_get_insert(textbuffer_messages);
    gtk_text_view_scroll_to_mark(textview_messages, mark, 0.0, TRUE, 0.0, 1.0);
}

// Function to add debug messages (formatted printf-style)
void add_debug_message(const char* format, ...) {
    char buffer[500];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    add_message(buffer);
}

int is_connected(struct Country* country1, struct Country* country2) {
    if (!country1 || !country2 || !country1->connected_countries) return 0;
    for (int i = 0; i < country1->connected_countries->connected_count; i++) {
        if (country1->connected_countries->connected_list[i] == country2) return 1;
    }
    return 0;
}

// Helper function to check if player can be relocated
void ensure_player_valid() {
    if (!player || !player->current_country || !list || !list->start) {
        return;
    }
    
    // Check if player's current country still exists in the list
    struct Country* check = list->start;
    int found = 0;
    while (check) {
        if (check == player->current_country) {
            found = 1;
            break;
        }
        check = check->next;
    }
    
    if (!found) {
        player->current_country = list->start;
        char msg[MAX_ACTION_MSG_LEN];
        sprintf(msg, "Jugador reubicado a %s (pais anterior eliminado)", player->current_country->name);
        add_message(msg);
    }
    
    // Also check if current_country (the viewed country) is still valid
    if (current_country && !is_country_valid(current_country)) {
        current_country = player->current_country;
        add_message("Vista reubicada al pais del jugador (pais anterior eliminado)");
    }
}

// Helper function to check if country still exists
int is_country_valid(struct Country* country) {
    if (!country || !list || !list->start) return 0;
    
    struct Country* check = list->start;
    while (check) {
        if (check == country) return 1;
        check = check->next;
    }
    return 0;
}

// Generic action handler
void perform_action(const char* action_name, int* action_value, int max_value, int is_increment) {
    if (current_game_state != TURN_PLAYER_ACTION) {
        char msg[MAX_ACTION_MSG_LEN];
        sprintf(msg, "No es tu turno de accion.");
        add_message(msg);
        return;
    }
    
    if (player_actions_remaining <= 0) {
        add_message("No te quedan acciones. Termina tu turno.");
        return;
    }
    
    if (!current_country) return;
    
    // CHANGED: Can only act on the player's current country, not connected ones
    if (current_country != player->current_country) {
        char msg[MAX_ACTION_MSG_LEN];
        sprintf(msg, "No puedes actuar en %s - debes estar en ese pais. Muevete primero.", current_country->name);
        add_message(msg);
        return;
    }
    
    // Check if action is possible
    if (is_increment) {
        // For increasing actions like political stability
        if (*action_value >= max_value) {
            char msg[MAX_ACTION_MSG_LEN];
            sprintf(msg, "%s ya esta al maximo en %s.", action_name, current_country->name);
            add_message(msg);
            return;
        }
        *action_value = (*action_value + 25 > max_value) ? max_value : *action_value + 25;
    } else {
        // For decreasing actions like poverty, crime, etc.
        if (*action_value <= 0) {
            char msg[MAX_ACTION_MSG_LEN];
            sprintf(msg, "%s ya esta en 0 en %s.", action_name, current_country->name);
            add_message(msg);
            return;
        }
        (*action_value)--;
    }
    
    player_actions_remaining--;
    char msg[MAX_ACTION_MSG_LEN];
    sprintf(msg, "Accion aplicada en %s. Acciones restantes: %d", 
           current_country->name, player_actions_remaining);
    add_message(msg);
    
    // Recalculate corruption after change
    calculate_corruption(current_country);
    
    label_current_country_update(current_country->name);
    gtk_widget_queue_draw(drawing_area);
    
    if (player_actions_remaining <= 0) {
        add_message("Fin de acciones. Siguiente Turno");
        end_player_turn();
    }
}

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
    struct Country* country = find_country_by_name(list, name);
    if (country && country->image) {
        *x = country->image->x;
        *y = country->image->y;
    } else {
        *x = 0;
        *y = 0;
    }
}

// Calculate the scale factor for the map
double calculate_map_scale(int widget_width, int widget_height) {
    if (!list || !list->start) return 1.0;
    
    // Find the boundaries of the map
    int max_x = 0, max_y = 0;
    struct Country* current = list->start;
    while (current) {
        if (current->image && current->image->original_pixbuf) {
            int cord_x = current->image->x + current->image->width;
            int cord_y = current->image->y + current->image->height;
            if (cord_x > max_x) max_x = cord_x;
            if (cord_y > max_y) max_y = cord_y;
        }
        current = current->next;
    }
    
    // Calculate scale to fit widget, maintaining aspect ratio
    double scale_width = (double)widget_width / max_x;
    double scale_height = (double)widget_height / max_y;
    return (scale_height < scale_width) ? scale_height : scale_width;
}

// Draw a single country on the map
void draw_country_on_map(cairo_t *cr, struct Country* country, gboolean highlight_border, gboolean show_corruption) {
    if (!country || !country->image || !country->image->original_pixbuf) return;
    
    GdkPixbuf* temp_pixbuf = gdk_pixbuf_copy(country->image->original_pixbuf);
    if (!temp_pixbuf) return;
    
    // Apply corruption coloring if needed
    if (show_corruption) {
        update_img_corruption(temp_pixbuf, country->corruption);
    }
    
    // Add border highlight if needed
    if (highlight_border) {
        draw_img_border(temp_pixbuf);
    }
    
    gdk_cairo_set_source_pixbuf(cr, temp_pixbuf, country->image->x, country->image->y);
    cairo_paint(cr);
    g_object_unref(temp_pixbuf);
}

// Main drawing function for the map
static void draw_map(GtkWidget *widget, cairo_t *cr, gpointer data) {
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);
    
    // Calculate scale
    scale = calculate_map_scale(width, height);
    
    cairo_save(cr);
    cairo_scale(cr, scale, scale);
    
    // Draw background
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
    cairo_paint(cr);
    
    // Draw all countries
    struct Country* current = list->start;
    while (current) {
        gboolean highlight = (current == current_country);
        // Show corruption only on player's current country during action phase
        gboolean show_corruption = (current_game_state == TURN_PLAYER_ACTION && 
                                    current == player->current_country);
        
        draw_country_on_map(cr, current, highlight, show_corruption);
        current = current->next;
    }
    
    cairo_restore(cr);
}

// Drawing function for the current country detail view
static void draw_current_country(GtkWidget *widget, cairo_t *cr, gpointer data) {
    if (!current_country || !current_country->image || !current_country->image->original_pixbuf || !surface_cur_country) {
        // Draw a light gray placeholder
        cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
        cairo_paint(cr);
        return;
    }
    
    // Get actual widget dimensions
    int widget_width = gtk_widget_get_allocated_width(widget);
    int widget_height = gtk_widget_get_allocated_height(widget);
    
    // Update the surface with the current country
    update_current_country_surface();
    
    // Get surface dimensions
    int surf_width = cairo_image_surface_get_width(surface_cur_country);
    int surf_height = cairo_image_surface_get_height(surface_cur_country);
    
    // Prevent division by zero
    if (surf_width <= 0 || surf_height <= 0 || widget_width <= 0 || widget_height <= 0) {
        cairo_set_source_rgb(cr, 1, 0, 0);
        cairo_paint(cr);
        return;
    }
    
    // Calculate scale to fit the available space, maintaining aspect ratio
    // Use the smallest scale to ensure the entire image fits within the widget
    double scale_x = (double)widget_width / surf_width;
    double scale_y = (double)widget_height / surf_height;
    double detail_scale = (scale_x < scale_y) ? scale_x : scale_y;
    
    // Center the image in the widget
    int scaled_width = surf_width * detail_scale;
    int scaled_height = surf_height * detail_scale;
    int offset_x = (widget_width - scaled_width) / 2;
    int offset_y = (widget_height - scaled_height) / 2;
    
    // Apply transformation
    cairo_save(cr);
    cairo_translate(cr, offset_x, offset_y);
    cairo_scale(cr, detail_scale, detail_scale);
    
    // Draw the surface
    cairo_set_source_surface(cr, surface_cur_country, 0, 0);
    cairo_paint(cr);
    
    cairo_restore(cr);
}

// Navigate to the next country in the list
void btn_right_shift_clicked(GtkWidget *widget, gpointer data) {
    if (!current_country) return;
    
    if (current_game_state == SELECT_STARTING_COUNTRY) {
        // During country selection, navigate through all countries
        if (current_country->next != NULL) {
            current_country = current_country->next;
        }
    } else {
        // During gameplay, navigate through player's current country and its connected countries
        if (!player || !player->current_country || !player->current_country->connected_countries) {
            return;
        }
        
        struct DoubleLinkedList* nav_list = player->current_country->connected_countries;
        
        // Build navigation array: player's country + connected countries
        int total_countries = nav_list->connected_count + 1;
        struct Country* available_countries[22]; // Max 21 countries + player's location
        
        available_countries[0] = player->current_country;
        for (int i = 0; i < nav_list->connected_count; i++) {
            available_countries[i + 1] = nav_list->connected_list[i];
        }
        
        // Find current_country in the available list
        int current_index = -1;
        for (int i = 0; i < total_countries; i++) {
            if (available_countries[i] == current_country) {
                current_index = i;
                break;
            }
        }
        
        // Move to next country (wrap around)
        if (current_index == -1) {
            current_country = available_countries[0]; // Default to player's country
        } else {
            int next_index = (current_index + 1) % total_countries;
            current_country = available_countries[next_index];
        }
    }
    
    label_current_country_update(current_country->name);
    
    if (drawing_area) gtk_widget_queue_draw(drawing_area);
    if (drawing_current_country) gtk_widget_queue_draw(drawing_current_country);
}

// Navigate to the previous country in the list
void btn_left_shift_clicked(GtkWidget *widget, gpointer data) {
    if (!current_country) return;
    
    if (current_game_state == SELECT_STARTING_COUNTRY) {
        // During country selection, navigate through all countries
        if (current_country->prev != NULL) {
            current_country = current_country->prev;
        }
    } else {
        // During gameplay, navigate through player's current country and its connected countries
        if (!player || !player->current_country || !player->current_country->connected_countries) {
            return;
        }
        
        struct DoubleLinkedList* nav_list = player->current_country->connected_countries;
        
        // Build navigation array: player's country + connected countries
        int total_countries = nav_list->connected_count + 1;
        struct Country* available_countries[22]; // Max 21 countries + player's location
        
        available_countries[0] = player->current_country;
        for (int i = 0; i < nav_list->connected_count; i++) {
            available_countries[i + 1] = nav_list->connected_list[i];
        }
        
        // Find current_country in the available list
        int current_index = -1;
        for (int i = 0; i < total_countries; i++) {
            if (available_countries[i] == current_country) {
                current_index = i;
                break;
            }
        }
        
        // Move to previous country (wrap around)
        if (current_index == -1) {
            current_country = available_countries[0]; // Default to player's country
        } else {
            int prev_index = (current_index - 1 + total_countries) % total_countries;
            current_country = available_countries[prev_index];
        }
    }
    
    label_current_country_update(current_country->name);
    
    if (drawing_area) gtk_widget_queue_draw(drawing_area);
    if (drawing_current_country) gtk_widget_queue_draw(drawing_current_country);
}

// Update the surface for the current country detail view
void update_current_country_surface() {
    if (!current_country || !current_country->image || !current_country->image->original_pixbuf) {
        return;
    }
    
    // Get the dimensions of the current country image
    int img_width = gdk_pixbuf_get_width(current_country->image->original_pixbuf);
    int img_height = gdk_pixbuf_get_height(current_country->image->original_pixbuf);
    
    // Recreate surface with the correct dimensions if needed
    if (surface_cur_country) {
        int surf_width = cairo_image_surface_get_width(surface_cur_country);
        int surf_height = cairo_image_surface_get_height(surface_cur_country);
        
        if (surf_width != img_width || surf_height != img_height) {
            cairo_surface_destroy(surface_cur_country);
            surface_cur_country = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, img_width, img_height);
        }
    }
    
    // Clear and draw on the surface
    if (surface_cur_country) {
        cairo_t *cr = cairo_create(surface_cur_country);
        
        // Clear the surface with transparency
        cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
        cairo_paint(cr);
        cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
        
        // Create a temporary pixbuf with corruption colors
        GdkPixbuf* temp_pixbuf = gdk_pixbuf_copy(current_country->image->original_pixbuf);
        if (temp_pixbuf) {
            update_img_corruption(temp_pixbuf, current_country->corruption);
            
            // Draw the pixbuf onto the surface
            gdk_cairo_set_source_pixbuf(cr, temp_pixbuf, 0, 0);
            cairo_paint(cr);
            
            g_object_unref(temp_pixbuf);
        }
        
        cairo_destroy(cr);
    }
}

// Get the game state description text
const char* get_game_state_text(char* buffer) {
    switch (current_game_state) {
        case SELECT_STARTING_COUNTRY:
            return "Selecciona tu pais de inicio - Usa las flechas y presiona 'Escoger Pais' (Presiona R para reiniciar)";
        case TURN_PLAYER_MOVE:
            return "Tu turno: MOVIMIENTO - Haz clic en un pais conectado o quedate aqui (Presiona R para reiniciar)";
        case TURN_PLAYER_ACTION:
            sprintf(buffer, "Tu turno: ACCIONES (%d restantes) - Presiona R para reiniciar", player_actions_remaining);
            return buffer;
        case TURN_CORRUPTION:
            return "Turno: CORRUPCION se expande...";
        case GAME_OVER:
            return "JUEGO TERMINADO";
        default:
            return "Estado desconocido";
    }
}

// Update country statistics labels
void update_country_stats_labels() {
    if (!current_country) return;
    
    char buffer[200];
    
    sprintf(buffer, "Pobreza: %d/3", current_country->poverty);
    gtk_label_set_text(lbl_poverty, buffer);
    
    sprintf(buffer, "Crimen: %d/3", current_country->crime);
    gtk_label_set_text(lbl_crime, buffer);
    
    sprintf(buffer, "Corrupcion: %.0f%%", current_country->corruption * 100);
    gtk_label_set_text(lbl_corruption, buffer);
    
    sprintf(buffer, "Desempleo: %d/3", current_country->unemployment);
    gtk_label_set_text(lbl_unemployment, buffer);
    
    if (lbl_political_stability) {
        sprintf(buffer, "Estabilidad Politica: %d/100", current_country->political_stability);
        gtk_label_set_text(lbl_political_stability, buffer);
    }
}

// Update the "Move to Country" button state and text
void update_move_button() {
    if (!btn_country || !current_country) return;
    
    if (current_game_state == SELECT_STARTING_COUNTRY) {
        gtk_button_set_label(GTK_BUTTON(btn_country), "Escoger Pais");
        gtk_widget_set_visible(btn_country, TRUE);
    } 
    else if (current_game_state == TURN_PLAYER_MOVE) {
        if (current_country == player->current_country) {
            gtk_button_set_label(GTK_BUTTON(btn_country), "Quedarse Aqui");
            gtk_widget_set_visible(btn_country, TRUE);
        } 
        else if (is_connected(player->current_country, current_country)) {
            char button_text[100];
            sprintf(button_text, "Mover a %s", current_country->name);
            gtk_button_set_label(GTK_BUTTON(btn_country), button_text);
            gtk_widget_set_visible(btn_country, TRUE);
        } 
        else {
            gtk_widget_set_visible(btn_country, FALSE);
        }
    }
    else if (current_game_state == TURN_PLAYER_ACTION) {
        // During action phase, show button only if viewing a different connected country
        if (current_country != player->current_country && 
            is_connected(player->current_country, current_country)) {
            char button_text[100];
            sprintf(button_text, "Cambiar a %s (1 accion)", current_country->name);
            gtk_button_set_label(GTK_BUTTON(btn_country), button_text);
            gtk_widget_set_visible(btn_country, TRUE);
        } 
        else {
            gtk_widget_set_visible(btn_country, FALSE);
        }
    }
    else {
        gtk_widget_set_visible(btn_country, FALSE);
    }
}

// Main label update function
void label_current_country_update(char *text) {
    if (!text || !lbl_current_country || !current_country) return;
    
    gtk_label_set_text(lbl_current_country, text);
    
    // Update game state label
    if (lbl_game_status) {
        char buffer[200];
        const char* state_text = get_game_state_text(buffer);
        gtk_label_set_text(lbl_game_status, state_text);
    }
    
    // Update country statistics
    update_country_stats_labels();
    
    // Update move button state
    update_move_button();
    
    // Redraw the current country detail image
    if (drawing_current_country) {
        gtk_widget_queue_draw(drawing_current_country);
    }
}

// Start game with selected country
void start_game_with_country(struct Country* country) {
    if (!country || !player) return;
    
    player->current_country = country;
    char msg[MAX_ACTION_MSG_LEN];
    sprintf(msg, "¡Bienvenido a %s! El juego comienza...", country->name);
    add_message(msg);
    add_message("Tu turno - MOVIMIENTO");
    
    current_game_state = TURN_PLAYER_MOVE;
    player_actions_remaining = 4;
    label_current_country_update(country->name);
    update_button_states();
    gtk_widget_queue_draw(drawing_area);
}

// Move player to a new country
void move_player_to_country(struct Country* destination) {
    if (!destination || !player || !player->current_country) return;
    
    char msg[MAX_ACTION_MSG_LEN];
    sprintf(msg, "Moviendo de %s a %s", player->current_country->name, destination->name);
    add_message(msg);
    
    player->current_country = destination;
    current_country = destination; // Update view to player's new location
    
    // Check if the new country is isolated (no neighbors)
    if (!destination->connected_countries || destination->connected_countries->connected_count == 0) {
        add_debug_message("No hay vecinos. La corrupcion ha ganado.");
        current_game_state = GAME_OVER;
        
        // Show game over dialog
        char end_msg[MAX_ACTION_MSG_LEN];
        strcpy(end_msg, "Ya no quedan paises vecinos. La corrupcion ha ganado.");
        
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(win), 
                                                     GTK_DIALOG_MODAL, 
                                                     GTK_MESSAGE_INFO, 
                                                     GTK_BUTTONS_NONE, 
                                                     "%s", end_msg);
        
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Reiniciar", 1);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Salir", 2);
        
        int response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        if (response == 1) {
            restart_game();
        } else {
            gtk_main_quit();
        }
        return;
    }
    
    current_game_state = TURN_PLAYER_ACTION;
    player_actions_remaining = 4;
    
    label_current_country_update(destination->name);
    update_button_states(); // Enable action buttons
    gtk_widget_queue_draw(drawing_area);
}

// Stay in current country
void stay_in_current_country() {
    if (!player || !player->current_country) return;
    
    char msg[MAX_ACTION_MSG_LEN];
    sprintf(msg, "Te quedas en %s", player->current_country->name);
    add_message(msg);
    
    current_country = player->current_country;
    
    // Check if current country is isolated (no neighbors)
    if (!current_country->connected_countries || current_country->connected_countries->connected_count == 0) {
        add_debug_message("No hay vecinos. La corrupcion ha ganado.");
        current_game_state = GAME_OVER;
        
        // Show game over dialog
        char end_msg[MAX_ACTION_MSG_LEN];
        strcpy(end_msg, "Ya no quedan paises vecinos. La corrupcion ha ganado.");
        
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(win), 
                                                     GTK_DIALOG_MODAL, 
                                                     GTK_MESSAGE_INFO, 
                                                     GTK_BUTTONS_NONE, 
                                                     "%s", end_msg);
        
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Reiniciar", 1);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Salir", 2);
        
        int response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        if (response == 1) {
            restart_game();
        } else {
            gtk_main_quit();
        }
        return;
    }
    
    current_game_state = TURN_PLAYER_ACTION;
    player_actions_remaining = 4;
    
    label_current_country_update(current_country->name);
    update_button_states(); // Enable action buttons
    gtk_widget_queue_draw(drawing_area);
}

// Change country during action phase (costs 1 action)
void change_country_during_action(struct Country* destination) {
    if (!destination || !player || !player->current_country) return;
    
    if (player_actions_remaining <= 0) {
        add_message("No te quedan acciones para cambiar de pais.");
        return;
    }
    
    char msg[MAX_ACTION_MSG_LEN];
    sprintf(msg, "Cambiando de %s a %s (1 accion usada)", 
           player->current_country->name, destination->name);
    add_message(msg);
    
    player->current_country = destination;
    current_country = destination;
    player_actions_remaining--;
    
    sprintf(msg, "Acciones restantes: %d", player_actions_remaining);
    add_message(msg);
    
    label_current_country_update(destination->name);
    update_button_states();
    gtk_widget_queue_draw(drawing_area);
    
    if (player_actions_remaining <= 0) {
        add_message("Acciones agotadas. El turno terminara automaticamente.");
        end_player_turn();
    }
}

void btn_country_clicked(GtkWidget *widget, gpointer data) {
    // Handle country selection at game start
    if (current_game_state == SELECT_STARTING_COUNTRY) {
        if (!current_country) {
            add_message("Error: No country selected.");
            return;
        }
        start_game_with_country(current_country);
        return;
    }
    
    // Handle country change during action phase
    if (current_game_state == TURN_PLAYER_ACTION) {
        if (!current_country || !player || !player->current_country) {
            add_message("Error: Estado del juego invalido.");
            return;
        }
        
        // Can't change to same country
        if (current_country == player->current_country) {
            add_message("Ya estas en este pais.");
            return;
        }
        
        // Check if destination is connected
        if (!is_connected(player->current_country, current_country)) {
            char msg[MAX_ACTION_MSG_LEN];
            sprintf(msg, "No puedes cambiar a %s - no esta conectado a %s.", 
                   current_country->name, player->current_country->name);
            add_message(msg);
            return;
        }
        
        // Change to new country (costs 1 action)
        change_country_during_action(current_country);
        return;
    }
    
    // Handle movement during TURN_PLAYER_MOVE phase
    if (current_game_state != TURN_PLAYER_MOVE) {
        add_message("No es tu turno de movimiento.");
        return;
    }
    
    if (!current_country || !player || !player->current_country) {
        add_message("Error: Estado del juego invalido.");
        return;
    }
    
    // Stay in current country
    if (current_country == player->current_country) {
        stay_in_current_country();
        return;
    }
    
    // Check if destination is connected
    if (!is_connected(player->current_country, current_country)) {
        char msg[MAX_ACTION_MSG_LEN];
        sprintf(msg, "No puedes moverte a %s - no esta conectado a %s.", 
               current_country->name, player->current_country->name);
        add_message(msg);
        return;
    }
    
    // Move to new country
    move_player_to_country(current_country);
}

void btn_action_poverty_clicked(GtkWidget *widget, gpointer data) {
    perform_action("Pobreza", &current_country->poverty, 3, 0);
}

void btn_action_crime_clicked(GtkWidget *widget, gpointer data) {
    perform_action("Crimen", &current_country->crime, 3, 0);
}

void btn_action_inequality_clicked(GtkWidget *widget, gpointer data) {
    perform_action("Desempleo", &current_country->unemployment, 3, 0);
}

void btn_action_political_weakness_clicked(GtkWidget *widget, gpointer data) {
    perform_action("Estabilidad Politica", &current_country->political_stability, 100, 1);
}

// Handler for restart button/key
void btn_restart_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(win), 
                                                 GTK_DIALOG_MODAL, 
                                                 GTK_MESSAGE_QUESTION, 
                                                 GTK_BUTTONS_YES_NO, 
                                                 "¿Estas seguro de que quieres reiniciar el juego?");
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    if (response == GTK_RESPONSE_YES) {
        restart_game();
    }
}

// Key press event handler
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    // Press 'R' to restart
    if (event->keyval == GDK_KEY_r || event->keyval == GDK_KEY_R) {
        btn_restart_clicked(widget, data);
        return TRUE;
    }
    return FALSE;
}

void update_action_button_labels() {
    if (!btn_action_poverty || !btn_action_crime || 
        !btn_action_unemployment || !btn_action_political_weakness || 
        !current_country || !hash_table) return;
    
    // Get solutions for current country
    struct listSolutions* solutions = search_by_key(hash_table, current_country->name);
    
    // Helper arrays to map solutions to buttons
    const char* default_labels[] = {
        "Reducir Pobreza",
        "Reducir Crimen", 
        "Reducir Desempleo",
        "Aumentar Estabilidad"
    };
    
    GtkWidget* buttons[] = {
        btn_action_poverty,
        btn_action_crime,
        btn_action_unemployment,
        btn_action_political_weakness
    };
    
    // Reset to default labels first
    for (int i = 0; i < 4; i++) {
        gtk_button_set_label(GTK_BUTTON(buttons[i]), default_labels[i]);
    }
    
    // If we have solutions and we're at the player's current country, show them
    if (solutions && solutions->start && current_country == player->current_country) {
        struct nodeList* current_solution = solutions->start;
        int button_index = 0;
        
        while (current_solution && button_index < 4) {
            if (current_solution->solutionString) {
                // Truncate if too long
                char label[100];
                strncpy(label, current_solution->solutionString, 99);
                label[99] = '\0';
                
                gtk_button_set_label(GTK_BUTTON(buttons[button_index]), label);
                button_index++;
            }
            current_solution = current_solution->nextNode;
        }
    }
}

void update_button_states() {
    if (!btn_action_poverty || !btn_action_crime || 
        !btn_action_unemployment || !btn_action_political_weakness) return;
    
    // Action buttons only enabled during action phase, with actions remaining, AND viewing player's current country
    // DISABLED during country selection phase
    gboolean enable_actions = (current_game_state == TURN_PLAYER_ACTION && 
                               player_actions_remaining > 0 &&
                               current_country == player->current_country);
    
    gtk_widget_set_sensitive(btn_action_poverty, enable_actions);
    gtk_widget_set_sensitive(btn_action_crime, enable_actions);
    gtk_widget_set_sensitive(btn_action_unemployment, enable_actions);
    gtk_widget_set_sensitive(btn_action_political_weakness, enable_actions);
    
    // Update button labels with solutions
    update_action_button_labels();
}

void end_player_turn() {
    if (!list || !player) {
        add_message("Error critico: Estado del juego invalido.");
        return;
    }
    
    add_message("Turno del jugador terminado");
    add_message("La corrupcion se expande...");
    current_game_state = TURN_CORRUPTION;
    
    // Corruption spreads - first round
    turn_corruption(list);
    calculate_corruption_country_list(list);
    while (!erase_dead_countries(list));
    ensure_player_valid();
    
    // Check for winner
    int winner = verify_winner(list);
    if (winner != 2) {
        current_game_state = GAME_OVER;
        check_winner();
        return;
    }
    
    // Corruption spreads again - second round
    add_message("La corrupcion se expande de nuevo...");
    turn_corruption(list);
    calculate_corruption_country_list(list);
    while (!erase_dead_countries(list));
    ensure_player_valid();
    
    // Check for winner again
    winner = verify_winner(list);
    if (winner != 2) {
        current_game_state = GAME_OVER;
        check_winner();
        return;
    }
    
    // Back to player turn
    add_message("Tu turno - MOVIMIENTO");
    
    // Check if player's current country is isolated (no neighbors left)
    if (!player->current_country->connected_countries || 
        player->current_country->connected_countries->connected_count == 0) {
        add_debug_message("Tu pais esta aislado. La corrupcion ha ganado.");
        current_game_state = GAME_OVER;
        
        // Show game over dialog
        char end_msg[MAX_ACTION_MSG_LEN];
        strcpy(end_msg, "Ya no quedan paises vecinos. La corrupcion ha ganado.");
        
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(win), 
                                                     GTK_DIALOG_MODAL, 
                                                     GTK_MESSAGE_INFO, 
                                                     GTK_BUTTONS_NONE, 
                                                     "%s", end_msg);
        
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Reiniciar", 1);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Salir", 2);
        
        int response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        if (response == 1) {
            restart_game();
        } else {
            gtk_main_quit();
        }
        return;
    }
    
    current_game_state = TURN_PLAYER_MOVE;
    player_actions_remaining = 4;
    
    // Update display - make sure current_country is still valid
    if (!is_country_valid(current_country)) {
        current_country = player->current_country;
    }
    
    label_current_country_update(current_country->name);
    update_button_states();
    gtk_widget_queue_draw(drawing_area);
}

// Function to clean up old game data
void cleanup_game_data() {
    // Free the old list
    if (list) {
        struct Country* current = list->start;
        while (current != NULL) {
            struct Country* next = current->next;
            if (current->connected_countries) {
                free(current->connected_countries);
            }
            free(current);
            current = next;
        }
        free(list);
        list = NULL;
    }
    
    // Free player
    if (player) {
        free(player);
        player = NULL;
    }
}

// Function to restart the game
void restart_game() {
    // Clean up old data
    cleanup_game_data();
    
    // Clear message log
    if (textbuffer_messages) {
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(textbuffer_messages, &start, &end);
        gtk_text_buffer_delete(textbuffer_messages, &start, &end);
    }
    
    // Reinitialize game elements
    free(list);
    list = initialize_double_linked_list();
    
    // Reassign images to countries
    struct Country* current = list->start;
    while (current) {
        for (int i = 0; i < NUM_COUNTRIES; i++) {
            if (strcmp(current->name, countries[i].name) == 0) {
                current->image = &countries[i];
                break;
            }
        }
        current = current->next;
    }
    
    // Calculate initial corruption for all countries
    calculate_corruption_country_list(list);
    
    player = allocate_initial_player_on_map(list);
    
    // Reset game state
    current_game_state = SELECT_STARTING_COUNTRY;
    player_actions_remaining = 4;
    current_country = list->start;
    
    // Add welcome messages
    add_message("=== NUEVO JUEGO ===");
    add_message("Bienvenido a Tuanis Pandemic");
    add_message("Selecciona tu pais de inicio...");
    
    // Update UI
    label_current_country_update(current_country->name);
    update_button_states();
    gtk_widget_queue_draw(drawing_area);
    if (drawing_current_country) {
        gtk_widget_queue_draw(drawing_current_country);
    }
}

void check_winner() {
    if (!list) return;
    
    int winner = verify_winner(list);
    if (winner != 2) {
        char msg[MAX_ACTION_MSG_LEN];
        GtkButtonsType buttons = GTK_BUTTONS_NONE;
        
        if (winner == 0) {
            strcpy(msg, "¡Felicidades! Has ganado. Todos los paises tienen al menos un problema resuelto.");
        } else if (winner == 3) {
            strcpy(msg, "Ya no quedan paises vecinos. La corrupcion ha ganado.");
        } else {
            sprintf(msg, "La corrupcion ha ganado. Solo quedan %d paises.", length_double_linked_list(list));
        }
        
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(win), 
                                                     GTK_DIALOG_MODAL, 
                                                     GTK_MESSAGE_INFO, 
                                                     buttons, 
                                                     "%s", msg);
        
        // Add custom buttons
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Reiniciar", 1);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Salir", 2);
        
        int response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        if (response == 1) {
            // Restart game
            restart_game();
        } else {
            // Exit
            gtk_main_quit();
        }
    }
}

void start_window(int argc, char *argv[]) {

    gtk_init(&argc, &argv);
    
    // Load CSS
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GError *css_error = NULL;
    if (gtk_css_provider_load_from_path(css_provider, 
        "src/Interface/style.css", &css_error)) {
        gtk_style_context_add_provider_for_screen(
            gdk_screen_get_default(),
            GTK_STYLE_PROVIDER(css_provider),
            GTK_STYLE_PROVIDER_PRIORITY_USER
        );
    }
    g_object_unref(css_provider);

    // create a surface for the drawing area, a image background in this case
    surface = cairo_image_surface_create_from_png(PATH "blue_image.png");
    // Create a proper surface for current country (will be updated dynamically)
    surface_cur_country = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 800, 600);

    if (!surface) {
        g_printerr("No se encontro el background\n");
        return;
    }

    // load country images
    for (int i = 0; i < NUM_COUNTRIES; i++) {
        char path[200] = PATH;
        strcat(path, countries[i].name);
        strcat(path, ".png");
        countries[i].original_pixbuf = gdk_pixbuf_new_from_file(path, NULL);
        
        if (countries[i].original_pixbuf) {
            countries[i].width = gdk_pixbuf_get_width(countries[i].original_pixbuf);
            countries[i].height = gdk_pixbuf_get_height(countries[i].original_pixbuf);
        }
    }

    // Load the Glade file
    GtkBuilder *builder = gtk_builder_new();
    GError *error = NULL;
    if (!gtk_builder_add_from_file(builder, "src/Interface/main.glade", &error)) {
        g_printerr("No se encontro el archivo glade.\n");
        if (error) {
            g_printerr("GTK Error: %s\n", error->message);
        }
        g_clear_error(&error);
        return;
    }

    // Initialize game elements
    hash_table = create_new_hash_table();
    populate_hash_table(hash_table);
    list = initialize_double_linked_list();
    
    // Assign images to countries
    struct Country* current = list->start;
    while (current) {
        for (int i = 0; i < NUM_COUNTRIES; i++) {
            if (strcmp(current->name, countries[i].name) == 0) {
                current->image = &countries[i];
                break;
            }
        }
        current = current->next;
    }
    
    // Calculate initial corruption for all countries
    calculate_corruption_country_list(list);
    
    player = allocate_initial_player_on_map(list);
    
    // Set game state
    current_game_state = TURN_PLAYER_MOVE;
    player_actions_remaining = 4;


    // Get widgets from the builder first (needed for dialog)
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
    textview_messages = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "textview_messages"));
    // Get action buttons
    btn_action_poverty = GTK_WIDGET(gtk_builder_get_object(builder, "btn_action_poverty"));
    btn_action_crime = GTK_WIDGET(gtk_builder_get_object(builder, "btn_action_crime"));
    btn_action_unemployment = GTK_WIDGET(gtk_builder_get_object(builder, "btn_action_unemployment"));
    btn_action_political_weakness = GTK_WIDGET(gtk_builder_get_object(builder, "btn_action_political_weakness"));
    btn_country = GTK_WIDGET(gtk_builder_get_object(builder, "btn_country"));
    box_right_panel = GTK_BOX(gtk_builder_get_object(builder, "box_right_panel"));
    
    // Check if widgets are null (optional ones can be NULL)
    if (!win || !drawing_area || !drawing_current_country || !btn_right_shift || !btn_left_shift || !lbl_current_country ||
        !lbl_poverty || !lbl_crime || !lbl_corruption || !lbl_unemployment || !lbl_game_status || !lbl_political_stability || !textview_messages ||
        !box_right_panel || !btn_action_poverty || !btn_action_crime || !btn_action_unemployment || !btn_action_political_weakness) {
        g_printerr("Hay widgets faltantes en el glade\n");
        return;
    }
    
    // Initialize the text buffer for messages
    if (textview_messages) {
        textbuffer_messages = gtk_text_view_get_buffer(textview_messages);
        add_message("Bienvenido a Tuanis Pandemic");
        add_message("Selecciona tu pais de inicio...");
    }

    // Show main window first
    gtk_widget_show_all(win);
    gtk_window_present(GTK_WINDOW(win));
    
    // Set initial country to first in list for selection
    current_country = list->start;
    current_game_state = SELECT_STARTING_COUNTRY;
    label_current_country_update(current_country->name);
    update_button_states();

    // Connect signals
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(win, "key-press-event", G_CALLBACK(on_key_press), NULL);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_map), NULL);
    g_signal_connect(drawing_current_country, "draw", G_CALLBACK(draw_current_country), NULL);
    
    // connect buttons 
    g_signal_connect(btn_right_shift, "clicked", G_CALLBACK(btn_right_shift_clicked), NULL);
    g_signal_connect(btn_left_shift, "clicked", G_CALLBACK(btn_left_shift_clicked), NULL);
    g_signal_connect(btn_action_poverty, "clicked", G_CALLBACK(btn_action_poverty_clicked), NULL);
    g_signal_connect(btn_action_crime, "clicked", G_CALLBACK(btn_action_crime_clicked), NULL);
    g_signal_connect(btn_action_unemployment, "clicked", G_CALLBACK(btn_action_inequality_clicked), NULL);
    g_signal_connect(btn_action_political_weakness, "clicked", G_CALLBACK(btn_action_political_weakness_clicked), NULL); 
    g_signal_connect(btn_country, "clicked", G_CALLBACK(btn_country_clicked), NULL);
    
    // release memory
    g_object_unref(builder);
    gtk_main();

    // end of the program
    cairo_surface_destroy(surface);
    if (surface_cur_country) {
        cairo_surface_destroy(surface_cur_country);
    }
    for (int i = 0; i < NUM_COUNTRIES; i++) {
        if (countries[i].original_pixbuf) {
            g_object_unref(countries[i].original_pixbuf);
        }
    }
}