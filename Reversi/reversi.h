#ifndef REVERSI_H_INCLUDED
#define REVERSI_H_INCLUDED
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pipes.h"

int ruch_gracza; // 0 - bialy | 1 - czarny
int numer_tury;
char id;
int flag_new_game;
int flag_cofnij;

int przeciwnik_cofnal;


typedef enum state{
    bialy=0,czarny=1,nic
} State;

State last_state[9][9];

typedef struct button {
    GtkWidget *btn;
    State stan;
    char id[3];
    GtkImage *obrazek;
} Button;

GtkWidget   *window;
Button      *przycisk[10][10];
GtkWidget   *tura;
GtkWidget   *historia_label;
GtkWidget   *newGame;

GtkWidget   *finish_dialog;
GtkWidget   *wait_dialog;

GtkWidget *buffor;

int zmien_plansze(char *id, int ruch_gracza);
void new_game();
void on_field_clicked(GtkWidget *btn_clicked,GdkEventButton *event,char *data);
void pobierz_sygnal(GtkWidget *widget,gpointer data);
int blokuj();
int czy_zmiana(char *id);
void odblokuj();
void dump_state();
void update_history();
void cofnij();
void dump_cofnij();

void on_cofnij_clicked(GtkWidget *widget,gpointer data);
void open_dialog_cofnij();

int get_signal();
void send_signal(char *data);
PipesPtr potoki;

void quit(char *data);
void open_dialog_exit();
void open_dialog_finish();
void open_dialog_start();

void new_game_process(GtkWidget *widget,gpointer data);
void open_dialog_wait();


void przekaz_tekst( GtkWidget *widget,GtkWidget *text);
void pobierz_tekst(char *data);



#endif // REVERSI_H_INCLUDED
