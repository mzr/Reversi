#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "reversi.h"
#include "pipes.h"

#define MAKS_DL_TEKSTU 1000




int main(int argc,char *argv[])
{
    if(argc < 2){
        printf("\nThis program should be called with the first argument: [C]zarny or [B]bialy\n\n");
        exit(1);
    }

    switch( (int)argv[1][0] ){
    case 'C': id='C'; potoki=initPipes(argc,argv); break;
    case 'B': id='B'; potoki=initPipes(argc,argv); break;
    default:
        printf("\nThis program should be called with the first argument: [C]zarny or [B]bialy\n\n"); exit(1);
    }

    gtk_init (&argc, &argv);
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"Reversi");
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window),30);                   //style
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(quit),NULL);
    gtk_window_set_has_resize_grip(GTK_WINDOW(window),FALSE);
    gtk_window_set_resizable(GTK_WINDOW(window),FALSE);

    GtkWidget *mainBox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    GtkWidget *leftBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    GtkWidget *rightBox=gtk_box_new(GTK_ORIENTATION_VERTICAL,10);

    GtkWidget *plansza=gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(plansza), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(plansza), TRUE);

    for(int i=8; i>=1; i--)
        for(int j=1; j<=8; j++){
            przycisk[j][i]=malloc(sizeof(Button));
            przycisk[j][i]->stan=nic;
            sprintf(przycisk[j][i]->id,"%c%d",'a'+j-1 ,i );
            przycisk[j][i]->obrazek=gtk_image_new_from_file("nic.jpg");
            przycisk[j][i]->btn=gtk_event_box_new();
            gtk_container_add(GTK_CONTAINER(przycisk[j][i]->btn),GTK_WIDGET(przycisk[j][i]->obrazek));
            gtk_grid_attach(GTK_GRID(plansza),przycisk[j][i]->btn, j-1,8-i ,1,1);
        }
    for(int i=0; i<10; i++)
        przycisk[i][0]=przycisk[0][i]=przycisk[9][i]=przycisk[i][9]=NULL;

    //indeksy planszy
    for(int i=0; i<8; i++){
        char a[2];
        sprintf(a,"%d",8-i);
        char c[2];
        sprintf(c,"%c",'a'+i);
        gtk_grid_attach(GTK_GRID(plansza),gtk_label_new(a),8,i,1,1);
        gtk_grid_attach(GTK_GRID(plansza),gtk_label_new(c),i,8,1,1);
    }

    //pole ruchu gracza
    tura=gtk_label_new("Ruch ma gracz: ");
    gtk_box_pack_end(GTK_BOX(leftBox),tura,TRUE,TRUE,10);

    GtkWidget *rightGrid=gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(rightGrid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(rightGrid), TRUE);

    GtkWidget *gracz;
    if(id=='C')
        gracz=gtk_label_new("Gracz Czarny");
    else
        gracz=gtk_label_new("Gracz Bialy");
    newGame=gtk_button_new_with_label("Nowa gra");
    GtkWidget *cofnij=gtk_button_new_with_label("Cofnij");

    gtk_grid_attach(GTK_GRID(rightGrid),gracz,0,0,2,1);
    gtk_grid_attach(GTK_GRID(rightGrid),newGame,0,1,1,1);
    gtk_grid_attach(GTK_GRID(rightGrid),cofnij,1,1,1,1);

    //historia
    GtkWidget *historia_theme=gtk_label_new("HSTORIA:");
    GtkWidget *historia=gtk_scrolled_window_new(NULL,NULL);
    historia_label=gtk_label_new("1.\ta2\tb2\n2.\ta2\tb2\n3.\ta2\tb2\n4.\ta2\tb2\n5.\ta2\tb2\n6.\ta2\tb2\n7.\ta2\tb2\n8.\ta2\tb2\n");
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (historia), historia_label);
    gtk_grid_attach(GTK_GRID(rightGrid),historia,0,3,2,6);
    gtk_grid_attach(GTK_GRID(rightGrid),historia_theme,0,2,2,1);



    //GTK TALK

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 1);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    buffor = (GtkWidget *)gtk_text_buffer_new (NULL);
    GtkWidget *text_view = gtk_text_view_new_with_buffer (GTK_TEXT_BUFFER(buffor));
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);

    GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
    gtk_container_add (GTK_CONTAINER (scrolled_window), text_view);
    gtk_container_set_border_width (GTK_CONTAINER(scrolled_window), 1);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 0, 10, 6);

    GtkWidget *text = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(text), MAKS_DL_TEKSTU);
    gtk_entry_set_text(GTK_ENTRY(text), "");
    g_signal_connect(G_OBJECT(text), "activate",G_CALLBACK(przekaz_tekst),(gpointer) text);
    gtk_grid_attach(GTK_GRID(grid), text, 0, 6, 10, 1);


    //gtk talk ends




    gtk_container_add(GTK_CONTAINER(leftBox), plansza);
    gtk_container_add(GTK_CONTAINER(rightBox), rightGrid );
    gtk_container_add(GTK_CONTAINER(mainBox), leftBox);
    gtk_container_add(GTK_CONTAINER(mainBox),rightBox);
    gtk_container_add(GTK_CONTAINER(window), mainBox);

    //to do gtk talk
    //gtk_grid_attach(GTK_GRID(rightGrid),grid,0,4,2,4);
    GtkWidget *czat=gtk_label_new("Czat");
    gtk_box_pack_end(GTK_BOX(rightBox),grid,0,0,0);
    gtk_box_pack_end(GTK_BOX(rightBox),czat,0,0,0);



    flag_new_game=1;
    flag_cofnij=0;

    //zdarzenia
    g_signal_connect(newGame,"clicked",G_CALLBACK(new_game_process),NULL);
    g_signal_connect(cofnij,"clicked",G_CALLBACK(on_cofnij_clicked),NULL);
    for(int i=1; i<=8; i++)
        for(int j=1; j<=8; j++){
            g_signal_connect(G_OBJECT(przycisk[i][j]->btn),"button_press_event",G_CALLBACK(on_field_clicked),przycisk[i][j]->id);
            gtk_widget_set_sensitive(przycisk[i][j]->btn,FALSE);
        }

    g_timeout_add(100,get_signal,NULL);

    gtk_widget_show_all(window);
    gtk_main ();
    return 0;
}



