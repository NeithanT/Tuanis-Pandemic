#ifndef INTERFACE_H
#define INTERFACE_H

#include <gtk/gtk.h>

/**
 * @brief Revisa si ya hay un ganador en el juego y actualiza la ventana.
 *
 * Esta función consulta el estado de la partida y, si alguien ganó, se encarga
 * de notificar a la interfaz para que muestre el resultado final.
 */
void check_winner();

/**
 * @brief Refresca el texto que muestra las soluciones disponibles en la interfaz.
 *
 * Se encarga de actualizar el área de texto que lista las posibles soluciones
 * que el jugador puede aplicar a los problemas del país donde se encuentra.
 */
void update_solutions_text();

/**
 * @brief Añade un mensaje de diagnóstico (debug) formateado a la ventana de registro.
 *
 * Utiliza un formato de texto similar a `printf` para registrar lo que está
 * sucediendo "detrás de cámaras" en un log visible, facilitando encontrar errores.
 *
 * @param format Formato de string (similar a `printf`).
 * @param ... Argumentos variables necesarios para el formato.
 */
void add_debug_message(const char* format, ...);

/**
 * @brief Arranca la librería GTK y muestra la ventana principal del juego.
 *
 * Es la función que inicia toda la aplicación gráfica. Se llama al inicio del programa.
 *
 * @param argc Número de argumentos de línea de comandos.
 * @param argv Arreglo de argumentos de línea de comandos.
 */
void start_window(int argc, char *argv[]);

/**
 * @brief Oscurece o modifica la imagen de un país para mostrar su nivel de corrupción.
 *
 * Aplica un filtro o efecto visual al `GdkPixbuf` (la imagen) basándose en
 * el porcentaje de corrupción que tiene el país actualmente.
 *
 * @param pixbuf Puntero al `GdkPixbuf` (imagen) a modificar.
 * @param corruptPercentage Porcentaje de corrupción del país (valor flotante de 0.0 a 1.0).
 */
void update_img_corruption(GdkPixbuf *pixbuf, float corruptPercentage);

/**
 * @brief Calcula qué tan cerca está un punto (x, y) del borde más cercano de la imagen.
 *
 * @param x Coordenada X del punto.
 * @param y Coordenada Y del punto.
 * @param width Ancho de la imagen.
 * @param height Alto de la imagen.
 * @return Distancia mínima al borde de la imagen.
 */
int min_distance_to_edge(int x, int y, int width, int height);

/**
 * @brief Revisa si un píxel tiene un vecino transparente en un radio cercano.
 *
 * Se usa para detectar los bordes de la silueta de un país dentro de su imagen
 * (donde el resto es transparente).
 *
 * @param pixels Arreglo de píxeles de la imagen.
 * @param x Coordenada X del píxel central.
 * @param y Coordenada Y del píxel central.
 * @param width Ancho de la imagen.
 * @param height Alto de la imagen.
 * @param rowstride Paso de fila en bytes.
 * @param search_radius Radio de búsqueda alrededor del píxel central.
 * @return **1** si se encuentra un vecino transparente, **0** si no hay.
 */
int has_transparent_neighbor(guchar *pixels, int x, int y, int width, int height,
                                          int rowstride, int search_radius);

/**
 * @brief Dibuja un contorno de color alrededor del país en el pixbuf.
 *
 * Modifica los píxeles del `GdkPixbuf` directamente para resaltar la figura
 * del país con un borde.
 *
 * @param pixbuf Puntero al `GdkPixbuf` a modificar.
 */
void draw_img_border(GdkPixbuf *pixbuf);

/**
 * @brief La función estática que dibuja el mapa completo del juego.
 *
 * Usa las herramientas de Cairo para renderizar visualmente todos los países
 * y sus fronteras/conexiones.
 *
 * @param widget Widget GTK donde se realizará el dibujo (normalmente el área de dibujo).
 * @param cr Contexto de Cairo (`cairo_t`) para las operaciones de dibujo.
 * @param data Datos adicionales de usuario (generalmente NULL en esta implementación).
 */
static void draw_map(GtkWidget *widget, cairo_t *cr, gpointer data);

/**
 * @brief La función estática que dibuja el detalle o la vista ampliada del país actual.
 *
 * Muestra información gráfica o la imagen enfocada del país donde el jugador
 * se encuentra en este momento.
 *
 * @param widget Widget GTK donde se realizará el dibujo.
 * @param cr Contexto de Cairo (`cairo_t`) para las operaciones de dibujo.
 * @param data Datos adicionales de usuario (generalmente NULL en esta implementación).
 */
static void draw_current_country(GtkWidget *widget, cairo_t *cr, gpointer data);

/**
 * @brief Maneja el evento cuando el usuario hace clic en el botón de desplazamiento a la derecha.
 *
 * Típicamente usado para moverse o seleccionar el siguiente elemento (país, opción) en una lista.
 *
 * @param widget Widget del botón que disparó el evento.
 * @param data Datos adicionales de usuario.
 */
void btn_right_shift_clicked(GtkWidget *widget, gpointer data);

/**
 * @brief Maneja el evento cuando el usuario hace clic en el botón de desplazamiento a la izquierda.
 *
 * Típicamente usado para moverse o seleccionar el elemento anterior.
 *
 * @param widget Widget del botón que disparó el evento.
 * @param data Datos adicionales de usuario.
 */
void btn_left_shift_clicked(GtkWidget *widget, gpointer data);

/**
 * @brief Cambia el texto de la etiqueta para mostrar el nombre del país actual.
 *
 * @param text Cadena de texto que contiene el nuevo nombre del país.
 */
void label_current_country_update(char *text);

/**
 * @brief Marca el final del turno de movimiento/acciones del jugador.
 *
 * Llama a la lógica para avanzar el juego, iniciando la fase de la computadora
 * o pasando al siguiente turno.
 */
void end_player_turn();

/**
 * @brief Maneja el clic en un botón relacionado con la interacción o movimiento a un país.
 *
 * Se activa cuando el jugador decide interactuar o viajar a una nueva ubicación.
 *
 * @param widget Widget del botón que disparó el evento.
 * @param data Datos adicionales de usuario.
 */
void btn_country_clicked(GtkWidget *widget, gpointer data);

/**
 * Actualiza los estados de los botones basados en la fase del juego
 */
void update_button_states();

#endif // INTERFACE_H