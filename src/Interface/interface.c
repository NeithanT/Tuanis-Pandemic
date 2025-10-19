#include <gtk/gtk.h>
#include <cairo.h>
#include <gdk/gdk.h>
#include "interface.h"
#include "../Structs/structs.h"
#include "../Structs/Hash/Hash.h"
#include "../Play/Play.h"
#include "../Structs/Player/Player.h"
#include "../Structs/Random/random.h"

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
GtkWidget *btn_country;
GtkWidget *btn_action_poverty;
GtkWidget *btn_action_crime;
GtkWidget *btn_action_unemployment;
GtkWidget *btn_action_political_weakness;
GtkWidget *btn_end_turn;
GtkLabel *lbl_current_country;
GtkLabel *lbl_poverty;
GtkLabel *lbl_crime;
GtkLabel *lbl_corruption;
GtkLabel *lbl_unemployment;
GtkLabel *lbl_solutions;
GtkLabel *lbl_game_status;
GtkLabel *lbl_political_stability;
GtkTextView *textview_messages;
GtkTextBuffer *textbuffer_messages;
cairo_surface_t *surface;

struct DoubleLinkedList* list;
struct Country* current_country;

// Game elements
struct hastTable* hash_table;
struct Player* player;
struct Player* ally;
double scale;
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

// Function to add messages to the message log
void add_message(const char* message) {
    if (!textbuffer_messages) return;
    
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(textbuffer_messages, &end);
    
    // Add timestamp or turn indicator if desired
    gtk_text_buffer_insert(textbuffer_messages, &end, message, -1);
    gtk_text_buffer_insert(textbuffer_messages, &end, "\n", -1);
    
    // Auto-scroll to the end
    GtkTextMark *mark = gtk_text_buffer_get_insert(textbuffer_messages);
    gtk_text_view_scroll_to_mark(textview_messages, mark, 0.0, TRUE, 0.0, 1.0);
}

int is_connected(struct Country* country1, struct Country* country2) {
    if (!country1 || !country2 || !country1->connected_countries) return 0;
    for (int i = 0; i < country1->connected_countries->connected_count; i++) {
        if (country1->connected_countries->connected_list[i] == country2) return 1;
    }
    return 0;
}

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
    struct Country* country = find_country_by_name(name);
    if (country && country->image) {
        *x = country->image->x;
        *y = country->image->y;
    } else {
        *x = 0;
        *y = 0;
    }
}

static void draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    // get the right most and lowest zone of the surface...
    // this is done by adding the position and the length
    int max_x = 0, max_y = 0, cord_x = 0, cord_y = 0;
    struct Country* current = list->start;
    while (current) {
        if (current->image && current->image->original_pixbuf) {
            cord_x = current->image->x + current->image->width;
            cord_y = current->image->y + current->image->height;
            if (cord_x > max_x) max_x = cord_x;
            if (cord_y > max_y) max_y = cord_y;
        }
        current = current->next;
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
    current = list->start;
    while (current) {
        if (current->image && current->image->original_pixbuf) {
            struct Country* country_data = current;
            // Create a copy of the pixbuf to modify
            GdkPixbuf* temp_pixbuf = gdk_pixbuf_copy(current->image->original_pixbuf);
            if (temp_pixbuf) {
                // During action phase, show current and connected countries
                if (current_game_state == TURN_PLAYER_ACTION && 
                    (country_data == current_country || is_connected(current_country, country_data))) {
                    update_img_corruption(temp_pixbuf, country_data->corruption);
                }
                
                // Highlight current country with black border
                if (country_data == current_country) {
                    draw_img_border(temp_pixbuf);
                }
                
                gdk_cairo_set_source_pixbuf(cr, temp_pixbuf, current->image->x, current->image->y);
                cairo_paint(cr);
                g_object_unref(temp_pixbuf);
            }
        }
        current = current->next;
    }

    cairo_restore(cr);
}

void btn_right_shift_clicked(GtkWidget *widget, gpointer data) {
    if (!current_country) return;
    
    if (current_country->next != NULL) {
        current_country = current_country->next;
        label_current_country_update(current_country->name);
        if (drawing_area) gtk_widget_queue_draw(drawing_area);
    }
}

void btn_left_shift_clicked(GtkWidget *widget, gpointer data) {
    if (!current_country) return;
    
    if (current_country->prev != NULL) {
        current_country = current_country->prev;
        label_current_country_update(current_country->name);
        if (drawing_area) gtk_widget_queue_draw(drawing_area);
    }
}

void label_current_country_update(char *text) {
    if (!text || !lbl_current_country) return;
    
    gtk_label_set_text(lbl_current_country, text);
    if (current_country) {
        char buffer[200];
        
        // Game state info
        if (lbl_game_status) {
            const char* state_text = "";
            switch (current_game_state) {
                case TURN_PLAYER_MOVE:
                    state_text = "Tu turno: MOVIMIENTO - Haz clic en un país conectado o quédate aquí";
                    break;
                case TURN_PLAYER_ACTION:
                    sprintf(buffer, "Tu turno: ACCIONES (%d restantes)", player_actions_remaining);
                    state_text = buffer;
                    break;
                case TURN_CORRUPTION:
                    state_text = "Turno: CORRUPCIÓN se expande...";
                    break;
                case TURN_ALLY:
                    state_text = "Turno: ALIADO actuando...";
                    break;
                case GAME_OVER:
                    state_text = "JUEGO TERMINADO";
                    break;
            }
            gtk_label_set_text(lbl_game_status, state_text);
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
            sprintf(buffer, "Estabilidad Política: %d/100", current_country->political_stability);
            gtk_label_set_text(lbl_political_stability, buffer);
        }
        
        // Update solutions
        update_solutions_text();
        
        // Update "Move to Country" button visibility and label
        if (btn_country) {
            if (current_game_state == TURN_PLAYER_MOVE) {
                if (current_country == player->current_country) {
                    // Show "Stay Here" button
                    gtk_button_set_label(GTK_BUTTON(btn_country), "Quedarse Aquí");
                    gtk_widget_set_visible(btn_country, TRUE);
                } else if (is_connected(player->current_country, current_country)) {
                    // Show "Move to Country" button
                    char button_text[100];
                    sprintf(button_text, "Mover a %s", current_country->name);
                    gtk_button_set_label(GTK_BUTTON(btn_country), button_text);
                    gtk_widget_set_visible(btn_country, TRUE);
                } else {
                    // Hide button if not connected
                    gtk_widget_set_visible(btn_country, FALSE);
                }
            } else {
                // Hide button during action phase
                gtk_widget_set_visible(btn_country, FALSE);
            }
        }
        
        // Redraw the current country image
        if (drawing_current_country) {
            gtk_widget_queue_draw(drawing_current_country);
        }
    }
}

void btn_country_clicked(GtkWidget *widget, gpointer data) {
    if (current_game_state != TURN_PLAYER_MOVE) {
        add_message("❌ No es tu turno de movimiento.");
        return;
    }
    
    if (!current_country || !player || !player->current_country) {
        add_message("❌ Error: Estado del juego inválido.");
        return;
    }
    
    // Check if we're staying in current country
    if (current_country == player->current_country) {
        char msg[200];
        sprintf(msg, "✓ Te quedas en %s", player->current_country->name);
        add_message(msg);
        
        // Transition to action phase
        current_game_state = TURN_PLAYER_ACTION;
        player_actions_remaining = 4;
        
        label_current_country_update(current_country->name);
        gtk_widget_queue_draw(drawing_area);
        return;
    }
    
    // Check if the country is connected
    if (!is_connected(player->current_country, current_country)) {
        char msg[200];
        sprintf(msg, "❌ No puedes moverte a %s - no está conectado a %s.", 
               current_country->name, player->current_country->name);
        add_message(msg);
        return;
    }
    
    // Move the player
    char msg[200];
    sprintf(msg, "➜ Moviéndose de %s a %s", player->current_country->name, current_country->name);
    add_message(msg);
    player->current_country = current_country;
    
    // Transition to action phase
    current_game_state = TURN_PLAYER_ACTION;
    player_actions_remaining = 4;
    
    label_current_country_update(current_country->name);
    gtk_widget_queue_draw(drawing_area);
}

void btn_stay_clicked(GtkWidget *widget, gpointer data) {
    if (current_game_state != TURN_PLAYER_MOVE) {
        add_message("❌ No es tu turno de movimiento.");
        return;
    }
    
    char msg[200];
    sprintf(msg, "✓ Te quedas en %s", player->current_country->name);
    add_message(msg);
    current_country = player->current_country;
    
    // Transition to action phase
    current_game_state = TURN_PLAYER_ACTION;
    player_actions_remaining = 4;
    
    label_current_country_update(current_country->name);
    update_button_states();
    gtk_widget_queue_draw(drawing_area);
}

void btn_end_turn_clicked(GtkWidget *widget, gpointer data) {
    if (current_game_state == TURN_PLAYER_MOVE) {
        add_message("❌ Primero debes moverte a un país (o quedarte en el actual).");
        return;
    }
    
    if (current_game_state == TURN_PLAYER_ACTION) {
        char msg[200];
        sprintf(msg, "⏭ Terminando turno del jugador con %d acciones sin usar.", player_actions_remaining);
        add_message(msg);
        end_player_turn();
    } else {
        add_message("❌ No puedes terminar el turno ahora.");
    }
}

void update_solutions_text() {
    if (!lbl_solutions || !hash_table || !current_country || !current_country->name) return;
    
    char buffer[1000] = "";
    
    // Show connected countries
    strcat(buffer, "Países conectados:\n");
    if (current_country->connected_countries && current_country->connected_countries->connected_count > 0) {
        for (int i = 0; i < current_country->connected_countries->connected_count && i < 10; i++) {
            struct Country* conn = current_country->connected_countries->connected_list[i];
            strcat(buffer, "• ");
            strcat(buffer, conn->name);
            strcat(buffer, "\n");
        }
    } else {
        strcat(buffer, "  Ninguno\n");
    }
    
    strcat(buffer, "\n");
    
    // Show solutions if available
    struct listSolutions* solutions = searchByKey(hash_table, current_country->name);
    if (solutions && solutions->start) {
        strcat(buffer, "Soluciones:\n");
        struct nodeList* current = solutions->start;
        while (current) {
            if (current->solutionString) {
                strcat(buffer, current->solutionString);
                strcat(buffer, "\n");
            }
            current = current->nextNode;
        }
    }
    
    gtk_label_set_text(lbl_solutions, buffer);
}

void btn_action_poverty_clicked(GtkWidget *widget, gpointer data) {
    if (current_game_state != TURN_PLAYER_ACTION) {
        add_message("❌ No es tu turno de acción.");
        return;
    }
    
    if (player_actions_remaining <= 0) {
        add_message("❌ No te quedan acciones. Termina tu turno.");
        return;
    }
    
    if (!current_country) return;
    
    // Check if player can act on this country
    if (current_country != player->current_country && 
        !is_connected(player->current_country, current_country)) {
        char msg[200];
        sprintf(msg, "❌ No puedes actuar en %s - no estás conectado.", current_country->name);
        add_message(msg);
        return;
    }
    
    if (current_country->poverty > 0) {
        current_country->poverty--;
        player_actions_remaining--;
        char msg[200];
        sprintf(msg, "✓ Reducida pobreza en %s. Acciones restantes: %d", 
               current_country->name, player_actions_remaining);
        add_message(msg);
        
        // Recalculate corruption after change
        calculateCorruption(current_country);
        
        label_current_country_update(current_country->name);
        gtk_widget_queue_draw(drawing_area);
        
        if (player_actions_remaining <= 0) {
            add_message("⚠ Acciones agotadas. El turno terminará automáticamente.");
            end_player_turn();
        }
    } else {
        char msg[200];
        sprintf(msg, "⚠ La pobreza ya está en 0 en %s.", current_country->name);
        add_message(msg);
    }
}

void btn_action_crime_clicked(GtkWidget *widget, gpointer data) {
    if (current_game_state != TURN_PLAYER_ACTION) {
        add_message("❌ No es tu turno de acción.");
        return;
    }
    
    if (player_actions_remaining <= 0) {
        add_message("❌ No te quedan acciones. Termina tu turno.");
        return;
    }
    
    if (!current_country) return;
    
    if (current_country != player->current_country && 
        !is_connected(player->current_country, current_country)) {
        char msg[200];
        sprintf(msg, "❌ No puedes actuar en %s - no estás conectado.", current_country->name);
        add_message(msg);
        return;
    }
    
    if (current_country->crime > 0) {
        current_country->crime--;
        player_actions_remaining--;
        char msg[200];
        sprintf(msg, "✓ Reducido crimen en %s. Acciones restantes: %d", 
               current_country->name, player_actions_remaining);
        add_message(msg);
        
        calculateCorruption(current_country);
        
        label_current_country_update(current_country->name);
        gtk_widget_queue_draw(drawing_area);
        
        if (player_actions_remaining <= 0) {
            add_message("⚠ Acciones agotadas. El turno terminará automáticamente.");
            end_player_turn();
        }
    } else {
        char msg[200];
        sprintf(msg, "⚠ El crimen ya está en 0 en %s.", current_country->name);
        add_message(msg);
    }
}

void btn_action_inequality_clicked(GtkWidget *widget, gpointer data) {
    if (current_game_state != TURN_PLAYER_ACTION) {
        add_message("❌ No es tu turno de acción.");
        return;
    }
    
    if (player_actions_remaining <= 0) {
        add_message("❌ No te quedan acciones. Termina tu turno.");
        return;
    }
    
    if (!current_country) return;
    
    if (current_country != player->current_country && 
        !is_connected(player->current_country, current_country)) {
        char msg[200];
        sprintf(msg, "❌ No puedes actuar en %s - no estás conectado.", current_country->name);
        add_message(msg);
        return;
    }
    
    if (current_country->unemployment > 0) {
        current_country->unemployment--;
        player_actions_remaining--;
        char msg[200];
        sprintf(msg, "✓ Reducido desempleo en %s. Acciones restantes: %d", 
               current_country->name, player_actions_remaining);
        add_message(msg);
        
        calculateCorruption(current_country);
        
        label_current_country_update(current_country->name);
        gtk_widget_queue_draw(drawing_area);
        
        if (player_actions_remaining <= 0) {
            add_message("⚠ Acciones agotadas. El turno terminará automáticamente.");
            end_player_turn();
        }
    } else {
        char msg[200];
        sprintf(msg, "⚠ El desempleo ya está en 0 en %s.", current_country->name);
        add_message(msg);
    }
}

void btn_action_political_weakness_clicked(GtkWidget *widget, gpointer data) {
    if (current_game_state != TURN_PLAYER_ACTION) {
        add_message("❌ No es tu turno de acción.");
        return;
    }
    
    if (player_actions_remaining <= 0) {
        add_message("❌ No te quedan acciones. Termina tu turno.");
        return;
    }
    
    if (!current_country) return;
    
    if (current_country != player->current_country && 
        !is_connected(player->current_country, current_country)) {
        char msg[200];
        sprintf(msg, "❌ No puedes actuar en %s - no estás conectado.", current_country->name);
        add_message(msg);
        return;
    }
    
    if (current_country->political_stability < 100) {
        current_country->political_stability += 25;
        if (current_country->political_stability > 100) current_country->political_stability = 100;
        player_actions_remaining--;
        char msg[200];
        sprintf(msg, "✓ Mejorada estabilidad política en %s. Acciones restantes: %d", 
               current_country->name, player_actions_remaining);
        add_message(msg);
        
        calculateCorruption(current_country);
        
        label_current_country_update(current_country->name);
        gtk_widget_queue_draw(drawing_area);
        
        if (player_actions_remaining <= 0) {
            add_message("Acciones agotadas. El turno terminará automáticamente.");
            end_player_turn();
        }
    } else {
        char msg[200];
        sprintf(msg, "La estabilidad política ya está al máximo en %s.", current_country->name);
        add_message(msg);
    }
}

void update_button_states() {
    if (!btn_action_poverty || !btn_action_crime || 
        !btn_action_unemployment || !btn_action_political_weakness) return;
    
    // During action phase, enable action buttons
    gboolean enable_actions = (current_game_state == TURN_PLAYER_ACTION && player_actions_remaining > 0);
    
    gtk_widget_set_sensitive(btn_action_poverty, enable_actions);
    gtk_widget_set_sensitive(btn_action_crime, enable_actions);
    gtk_widget_set_sensitive(btn_action_unemployment, enable_actions);
    gtk_widget_set_sensitive(btn_action_political_weakness, enable_actions);
    
    // End turn button - enabled during action phase
    if (btn_end_turn) {
        gtk_widget_set_sensitive(btn_end_turn, 
            current_game_state == TURN_PLAYER_ACTION || current_game_state == TURN_PLAYER_MOVE);
    }
}

void end_player_turn() {
    if (!list || !player || !ally) {
        add_message("❌ Error crítico: Estado del juego inválido.");
        return;
    }
    
    add_message("=== Turno del jugador terminado ===");
    add_message("⚠ La corrupción se expande...");
    current_game_state = TURN_CORRUPTION;
    
    // Corruption spreads
    turnCorruption(list);
    calculateCorruptionCountryList(list);
    
    // Clean dead countries
    while (!eraseDeadCountries(list));
    
    // Check if player/ally are in dead countries and relocate them
    if (player && player->current_country && list->start) {
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
            char msg[200];
            sprintf(msg, "Jugador reubicado a %s (país anterior eliminado)", player->current_country->name);
            add_message(msg);
        }
    }
    
    if (ally && ally->current_country && list->start) {
        struct Country* check = list->start;
        int found = 0;
        while (check) {
            if (check == ally->current_country) {
                found = 1;
                break;
            }
            check = check->next;
        }
        if (!found) {
            ally->current_country = list->start;
            char msg[200];
            sprintf(msg, "Aliado reubicado a %s (país anterior eliminado)", ally->current_country->name);
            add_message(msg);
        }
    }
    
    // Check for winner
    int winner = verifyWinner(list);
    if (winner != 2) {
        current_game_state = GAME_OVER;
        check_winner();
        return;
    }
    
    // Ally turn
    add_message("--- Turno del aliado ---");
    current_game_state = TURN_ALLY;
    turnAlly(ally);
    
    // Corruption spreads again
    add_message("La corrupción se expande de nuevo...");
    turnCorruption(list);
    calculateCorruptionCountryList(list);
    
    // Clean dead countries
    while (!eraseDeadCountries(list));
    
    // Check if player/ally are in dead countries again
    if (player && player->current_country && list->start) {
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
            char msg[200];
            sprintf(msg, "⚠ Jugador reubicado a %s (país anterior eliminado)", player->current_country->name);
            add_message(msg);
        }
    }
    
    if (ally && ally->current_country && list->start) {
        struct Country* check = list->start;
        int found = 0;
        while (check) {
            if (check == ally->current_country) {
                found = 1;
                break;
            }
            check = check->next;
        }
        if (!found) {
            ally->current_country = list->start;
            char msg[200];
            sprintf(msg, "Aliado reubicado a %s (país anterior eliminado)", ally->current_country->name);
            add_message(msg);
        }
    }
    
    // Check for winner
    winner = verifyWinner(list);
    if (winner != 2) {
        current_game_state = GAME_OVER;
        check_winner();
        return;
    }
    
    // Back to player turn
    add_message("=== Tu turno - MOVIMIENTO ===");
    current_game_state = TURN_PLAYER_MOVE;
    player_actions_remaining = 4;
    
    // Update display - make sure current_country is still valid
    if (player && player->current_country) {
        struct Country* check = list->start;
        int found = 0;
        while (check) {
            if (check == current_country) {
                found = 1;
                break;
            }
            check = check->next;
        }
        if (!found) {
            current_country = player->current_country;
        }
    }
    
    label_current_country_update(current_country->name);
    update_button_states();
    gtk_widget_queue_draw(drawing_area);
}

void check_winner() {
    if (!list) return;
    
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
    } else {
        printf("No se encontro el CSS.\n");
        return;
    }
    g_object_unref(css_provider);
    // free css provider, it already loaded

    // create a surface for the drawing area, a image background in this case
    surface = cairo_image_surface_create_from_png(PATH "blue_image.png");
    if (!surface) {
        printf("No se encontro el background\n");
        return;
    }

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
    if (!gtk_builder_add_from_file(builder, "src/Interface/main.glade", &error)) {
        g_printerr("No se encontro el archivo glade.\n");
        g_clear_error(&error);
        return;
    }

    // Initialize game elements
    hash_table = createNewHashTable();
    populateHashTable(hash_table);
    list = initializeDoubleLinkedList();
    
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
    textview_messages = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "textview_messages"));
    
    // Initialize the text buffer for messages
    if (textview_messages) {
        textbuffer_messages = gtk_text_view_get_buffer(textview_messages);
        add_message("=== Bienvenido a Tuanis Pandemic ===");
        add_message("Jugador y aliado han sido posicionados.");
        add_message("=== Tu turno - MOVIMIENTO ===");
    }

    // Check if widgets are null (optional ones can be NULL)
    if (!win || !drawing_area || !drawing_current_country || !btn_right_shift || !btn_left_shift || !lbl_current_country ||
        !lbl_poverty || !lbl_crime || !lbl_corruption || !lbl_unemployment /*|| !textview_solutions*/) {
        g_printerr("Error: Some widgets not found in glade file\n");
        return;
    }
    printf("Widgets retrieved\n");

    // Get action buttons
    btn_action_poverty = GTK_WIDGET(gtk_builder_get_object(builder, "btn_action_poverty"));
    btn_action_crime = GTK_WIDGET(gtk_builder_get_object(builder, "btn_action_crime"));
    btn_action_unemployment = GTK_WIDGET(gtk_builder_get_object(builder, "btn_action_unemployment"));
    btn_action_political_weakness = GTK_WIDGET(gtk_builder_get_object(builder, "btn_action_political_weakness"));
    btn_country = GTK_WIDGET(gtk_builder_get_object(builder, "btn_country"));
    btn_end_turn = GTK_WIDGET(gtk_builder_get_object(builder, "btn_end_turn"));

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
    // gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    // g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_drawing_area_button_press), NULL);

    // connect buttons 
    g_signal_connect(btn_right_shift, "clicked", G_CALLBACK(btn_right_shift_clicked), NULL);
    g_signal_connect(btn_left_shift, "clicked", G_CALLBACK(btn_left_shift_clicked), NULL);
    g_signal_connect(btn_action_poverty, "clicked", G_CALLBACK(btn_action_poverty_clicked), NULL);
    g_signal_connect(btn_action_crime, "clicked", G_CALLBACK(btn_action_crime_clicked), NULL);
    g_signal_connect(btn_action_unemployment, "clicked", G_CALLBACK(btn_action_inequality_clicked), NULL);
    g_signal_connect(btn_action_political_weakness, "clicked", G_CALLBACK(btn_action_political_weakness_clicked), NULL);
    
    // Connect move button
    if (btn_country) {
        g_signal_connect(btn_country, "clicked", G_CALLBACK(btn_country_clicked), NULL);
    }
    
    // Connect end turn button (already retrieved earlier)
    if (btn_end_turn) {
        g_signal_connect(btn_end_turn, "clicked", G_CALLBACK(btn_end_turn_clicked), NULL);
    }
    
    // Set initial button states
    update_button_states();
    
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