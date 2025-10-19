#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>

/**
 * Busca un país por su nombre en la lista global
 * name Nombre del país a buscar
 * Puntero al país encontrado, o NULL si no existe
 */
struct Country* find_country_by_name(const char* name);

/**
 * Verifica si hay un ganador en el juego y actualiza la interfaz
 */
void check_winner();

/**
 * Actualiza el texto de soluciones en la interfaz
 */
void update_solutions_text();

/**
 * Inicia la ventana principal de la interfaz GTK
 * argc Número de argumentos de línea de comandos
 * argv Arreglo de argumentos de línea de comandos
 */
void start_window(int argc, char *argv[]);

/**
 * Actualiza la imagen de corrupción de un pixbuf basado en el porcentaje
 * pixbuf Puntero al GdkPixbuf a modificar
 * corruptPercentage Porcentaje de corrupción (0.0 a 1.0)
 */
void update_img_corruption(GdkPixbuf *pixbuf, float corruptPercentage);

/**
 * Calcula la distancia mínima al borde de una imagen
 * x Coordenada x del punto
 * y Coordenada y del punto
 * width Ancho de la imagen
 * height Alto de la imagen
 * Distancia mínima al borde
 */
int min_distance_to_edge(int x, int y, int width, int height);

/**
 * Verifica si un píxel tiene un vecino transparente dentro del radio de búsqueda
 * pixels Arreglo de píxeles de la imagen
 * x Coordenada x del píxel
 * y Coordenada y del píxel
 * width Ancho de la imagen
 * height Alto de la imagen
 * rowstride Paso de fila en bytes
 * search_radius Radio de búsqueda
 * 1 si tiene vecino transparente, 0 en caso contrario
 */
int has_transparent_neighbor(guchar *pixels, int x, int y, int width, int height, 
                                          int rowstride, int search_radius);

/**
 * Dibuja un borde en la imagen del pixbuf
 * pixbuf Puntero al GdkPixbuf a modificar
 */
void draw_img_border(GdkPixbuf *pixbuf);

/**
 * Dibuja el mapa principal con todos los países
 * widget Widget GTK donde dibujar
 * cr Contexto de Cairo para dibujar
 * data Datos adicionales (no usado)
 */
static void draw_map(GtkWidget *widget, cairo_t *cr, gpointer data);

/**
 * Dibuja la vista detallada del país actual
 * widget Widget GTK donde dibujar
 * cr Contexto de Cairo para dibujar
 * data Datos adicionales (no usado)
 */
static void draw_current_country(GtkWidget *widget, cairo_t *cr, gpointer data);

/**
 * Maneja el clic en el botón de desplazamiento a la derecha
 * widget Widget del botón
 * data Datos adicionales
 */
void btn_right_shift_clicked(GtkWidget *widget, gpointer data);

/**
 * Maneja el clic en el botón de desplazamiento a la izquierda
 * widget Widget del botón
 * data Datos adicionales
 */
void btn_left_shift_clicked(GtkWidget *widget, gpointer data);

/**
 * Actualiza la etiqueta del país actual con el texto dado
 * text Texto a mostrar
 */
void label_current_country_update(char *text);

/**
 * Finaliza el turno del jugador y comienza los turnos de la computadora
 */
void end_player_turn();

/**
 * Maneja el clic en el botón para moverse a un país
 * widget Widget del botón
 * data Datos adicionales
 */
void btn_country_clicked(GtkWidget *widget, gpointer data);

/**
 * Maneja el clic en el botón para quedarse en el país actual
 * widget Widget del botón
 * data Datos adicionales
 */
void btn_stay_clicked(GtkWidget *widget, gpointer data);

/**
 * Actualiza los estados de los botones basados en la fase del juego
 */
void update_button_states();

#endif // INTERFACE_H