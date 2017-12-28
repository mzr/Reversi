#include "reversi.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAKS_DL_TEKSTU 1000

int not_out_of_bounds(int x, int y)
{
	return x >= 1 && x <= 8 && y >= 1 && y <= 8;
}

void przekaz_tekst( GtkWidget *widget,GtkWidget *text){
    char moj_id[2];
    moj_id[0]=id;
    moj_id[1]='\0';
    strcpy(moj_id+1," < ");

    gchar wejscie[MAKS_DL_TEKSTU+5];
    char temp[MAKS_DL_TEKSTU+5];
    sprintf(temp,"*%s",gtk_entry_get_text (GTK_ENTRY (text)));

    sendStringToPipe(potoki, temp);

    strcpy(wejscie,moj_id);
    strcpy(wejscie+strlen(wejscie),gtk_entry_get_text (GTK_ENTRY (text)));
    strcat(wejscie,"\n");

    gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER(buffor),wejscie,-1);

    gtk_entry_set_text(GTK_ENTRY(text), "");

}
void pobierz_tekst(char *data)
{
    char twoj_id[25];
    if(id=='B')
        twoj_id[0]='C';
    else
        twoj_id[0]='B';
    twoj_id[1]='\0';
    strcpy(twoj_id+1," < ");

    gchar wejscie[MAKS_DL_TEKSTU+5];

    strcpy(wejscie,twoj_id);
    strcpy(wejscie+strlen(wejscie),data);
    strcat(wejscie,"\n");
    gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER(buffor),wejscie,-1);

}





void quit(char *data){
    send_signal("@EX");
    closePipes(potoki);
    gtk_main_quit();
}
void open_dialog_exit(){
    GtkWidget *dialog, *label;
    dialog=gtk_dialog_new_with_buttons("Oops!",
                                       GTK_WINDOW(window),
                                       GTK_DIALOG_MODAL,
                                       GTK_STOCK_QUIT,GTK_RESPONSE_OK,NULL);
    label=gtk_label_new("Przeciwnik opóścił rozgrywke.");
    gtk_window_set_deletable(GTK_WINDOW(dialog),FALSE);
    gtk_window_set_has_resize_grip(GTK_WINDOW(dialog),FALSE);
    gtk_window_set_resizable(GTK_WINDOW(dialog),FALSE);
    gtk_box_pack_start(gtk_dialog_get_content_area(dialog),label,TRUE,TRUE,0);
    gtk_widget_show_all(dialog);
    gint response=gtk_dialog_run(GTK_DIALOG(dialog));
    if(response==GTK_RESPONSE_OK)
        gtk_main_quit();
}

void open_dialog_wait(){

    g_timeout_add(100,get_signal,NULL);

    GtkWidget *label, *spin;
    spin=gtk_spinner_new();
    wait_dialog=gtk_dialog_new_with_buttons("Musisz poczekać.",
                                       GTK_WINDOW(window),
                                       GTK_DIALOG_MODAL,NULL);
    label=gtk_label_new("Czekaj na odpowiedź przeciwnika.");
    gtk_window_set_deletable(GTK_WINDOW(wait_dialog),FALSE);
    //gtk_window_set_decorated(GTK_WINDOW(wait_dialog),FALSE);
    gtk_window_set_resizable(GTK_WINDOW(wait_dialog),FALSE);
    gtk_window_set_has_resize_grip(GTK_WINDOW(wait_dialog),FALSE);
    gtk_box_pack_start(gtk_dialog_get_content_area(wait_dialog),label,0,0,0);
    gtk_box_pack_start(gtk_dialog_get_content_area(wait_dialog),spin,0,0,0);
    gtk_spinner_start(spin);
    gtk_widget_show_all(wait_dialog);
}

void open_dialog_finish(){

    g_timeout_add(100,get_signal,NULL);

    int biale=0;
    int czarne=0;
    for(int i=1; i<=8; i++)
        for(int j=1; j<=8; j++){
            if(przycisk[j][i]->stan==bialy)
                biale++;
            if(przycisk[j][i]->stan==czarny)
                czarne++;
        }
    printf("Koniec gry. biale: %d, czarne: %d\n",biale,czarne);

    GtkWidget /* *dialog,*/ *label;
    finish_dialog=gtk_dialog_new_with_buttons("Koniec gry!",
                                       GTK_WINDOW(window),
                                       GTK_DIALOG_MODAL,
                                       GTK_STOCK_OK,GTK_RESPONSE_OK,
                                       GTK_STOCK_QUIT,GTK_RESPONSE_NO,NULL);

   //gtk_widget_set_name(dialog,"dialog_finish");
   gtk_window_set_deletable(GTK_WINDOW(finish_dialog),FALSE);
   gtk_window_set_has_resize_grip(GTK_WINDOW(finish_dialog),FALSE);
   gtk_window_set_resizable(GTK_WINDOW(finish_dialog),FALSE);

    if(biale>czarne)
        label=gtk_label_new("Wygrał gracz biały!");
        else if(czarne>biale)
                label=gtk_label_new("Wygrał gracz czarny!");
             else
                label=gtk_label_new("Remis!");

    gtk_box_pack_start(gtk_dialog_get_content_area(finish_dialog/*dialog*/),label,0,0,0);
    gtk_widget_show_all(finish_dialog/*dialog*/);
    gint response=gtk_dialog_run(GTK_DIALOG(finish_dialog/*dialog*/));
    if(response==GTK_RESPONSE_NO){
        //quit(NULL);
        gtk_widget_destroy(finish_dialog/*dialog*/);
    }
    if(response==GTK_RESPONSE_OK){
        char sig[4]="%SR";
        send_signal(sig);
        gtk_widget_destroy(finish_dialog/*dialog*/);
    }
}
void open_dialog_start(){
    GtkWidget *dialog, *label;

    dialog=gtk_dialog_new_with_buttons("Nowa Gra",
                                       GTK_WINDOW(window),
                                       GTK_DIALOG_MODAL,
                                       GTK_STOCK_YES,GTK_RESPONSE_YES,
                                       GTK_STOCK_NO,GTK_RESPONSE_NO,NULL);

    gtk_window_set_deletable(GTK_WINDOW(dialog),FALSE);
    gtk_window_set_has_resize_grip(GTK_WINDOW(dialog),FALSE);
    gtk_window_set_resizable(GTK_WINDOW(dialog),FALSE);

    label=gtk_label_new("Czy chcesz rozpoczac nowa gre?");
    gtk_box_pack_start(gtk_dialog_get_content_area(dialog),label,0,0,0);
    gtk_widget_show_all(dialog);
    gint response=gtk_dialog_run(GTK_DIALOG(dialog));
    if(response==GTK_RESPONSE_NO){
        char tmp[4]="&SN";
        send_signal(tmp);
        gtk_widget_destroy(dialog);
    }
    if(response==GTK_RESPONSE_YES){
        char sig[4]="#SP";
        send_signal(sig);
        gtk_widget_destroy(dialog);
        new_game();
    }
}

void open_dialog_cofnij(){
    GtkWidget *dialog, *label;
    dialog=gtk_dialog_new_with_buttons("Cofanie",
                                       GTK_WINDOW(window),
                                       GTK_DIALOG_MODAL,
                                       GTK_STOCK_YES,GTK_RESPONSE_YES,
                                       GTK_STOCK_NO,GTK_RESPONSE_NO,NULL);
    label=gtk_label_new("Czy pozwalasz przeciwnikowi cofnąć ruch?");
    gtk_window_set_deletable(GTK_WINDOW(dialog),FALSE);
    gtk_window_set_has_resize_grip(GTK_WINDOW(dialog),FALSE);
    gtk_window_set_resizable(GTK_WINDOW(dialog),FALSE);
    gtk_box_pack_start(gtk_dialog_get_content_area(dialog),label,0,0,0);
    gtk_widget_show_all(dialog);
    gint response=gtk_dialog_run(GTK_DIALOG(dialog));
    if(response==GTK_RESPONSE_NO){
        char tmp[4]=")CN";
        send_signal(tmp);
        gtk_widget_destroy(dialog);
    }
    if(response==GTK_RESPONSE_YES){
        przeciwnik_cofnal=1;
        char sig[4]="(CP";
        send_signal(sig);
        gtk_widget_destroy(dialog);
        cofnij();
    }
}

void new_game_process(GtkWidget *widget,gpointer data){
    if(flag_new_game==1){
        open_dialog_wait();
        char tmp[4]="%SR";
        send_signal(tmp);
        //gtk_widget_set_sensitive(newGame,FALSE);
        flag_new_game=0;
    }
}

int get_signal(){
    char bufor[MAKS_DL_TEKSTU];
    if(getStringFromPipe(potoki,bufor,MAKS_DL_TEKSTU)==1){
        printf("odebrano sygnal: %s\n",bufor);
        if(bufor[0]=='!'){
                przeciwnik_cofnal=0;
            dump_cofnij();
            update_history(bufor+1);
            zmien_plansze(bufor+1,ruch_gracza);
            ruch_gracza++;

            if(ruch_gracza%2==1) gtk_label_set_text(GTK_LABEL(tura),"Ruch ma gracz czarny");
            if(ruch_gracza%2==0) gtk_label_set_text(GTK_LABEL(tura),"Ruch ma gracz bia³y");
            odblokuj();

            if(blokuj()==0){
                char sig[4]=".NM";
                ruch_gracza++;
                send_signal(sig);
            }
            return TRUE;
        }
        if(bufor[0]=='.'){
                przeciwnik_cofnal=0;
            update_history(bufor+1);
            ruch_gracza++;
            if(ruch_gracza%2==1) gtk_label_set_text(GTK_LABEL(tura),"Ruch ma gracz czarny");
            if(ruch_gracza%2==0) gtk_label_set_text(GTK_LABEL(tura),"Ruch ma gracz bia³y");
            odblokuj();
            if(blokuj()==0){
                char sig[4]="$GO";
                send_signal(sig);
                open_dialog_finish();
            }
            return TRUE;
        }
        if(bufor[0]=='@'){          //start negative response
            open_dialog_exit();
            return TRUE;
        }
        if(bufor[0]=='$'){          //game over response
            open_dialog_finish();
            return TRUE;
        }
        if(bufor[0]=='#'){          //start positive response
            gtk_widget_destroy(wait_dialog);
            new_game();
            flag_new_game=1;
            return TRUE;
        }
        if(bufor[0]=='&'){          //start negative response
            gtk_widget_destroy(wait_dialog);
            flag_new_game=1;
            return TRUE;
        }
        if(bufor[0]=='%'){
            gtk_widget_destroy(finish_dialog);
            open_dialog_start();
            return TRUE;
        }
        if(bufor[0]=='^'){
            open_dialog_cofnij();
            return TRUE;
        }
        if(bufor[0]=='('){
            gtk_widget_destroy(wait_dialog);
            flag_cofnij=1;
            cofnij();
            return TRUE;
        }
        if(bufor[0]==')'){
            gtk_widget_destroy(wait_dialog);
            flag_cofnij=1;
            return TRUE;
        }
        if(bufor[0]=='*'){
            pobierz_tekst(bufor+1);
            return TRUE;
        }
    }
    return TRUE;
}

void send_signal(char *data){
    printf("wyslano sygnal: %s\n",data);
    sendStringToPipe(potoki,data);
}

void update_history(char *data){
    char tmp[1000];
    if(ruch_gracza%2==0)
        sprintf(tmp,"%s%d.\t%s\t",gtk_label_get_text(historia_label),numer_tury,data );
    if(ruch_gracza%2==1)
        sprintf(tmp,"%s%s\n",gtk_label_get_text(historia_label),data );
    gtk_label_set_text(historia_label,tmp);
}

void on_cofnij_clicked(GtkWidget *widget,gpointer data){
    if(flag_cofnij==1 && ((ruch_gracza%2==1 && id=='B') || (ruch_gracza%2==0 && id=='C'))  ){
        if(ruch_gracza>0 && przeciwnik_cofnal==0){
            open_dialog_wait();
            char tmp[4]="^CR";
            send_signal(tmp);
            flag_cofnij=0;
        }
    }
}


void new_game(GtkWidget *widget,gpointer data){
    ruch_gracza=0;
    numer_tury=1;

    flag_cofnij=1;
    przeciwnik_cofnal=0;

    gtk_label_set_text(GTK_LABEL(tura),"Ruch ma gracz biały");

    for(int i=1; i<=8; i++)
        for(int j=1; j<=8; j++){
            przycisk[i][j]->stan=nic;
        }
    przycisk[4][4]->stan=czarny;
    przycisk[5][5]->stan=czarny;
    przycisk[4][5]->stan=bialy;
    przycisk[5][4]->stan=bialy;

    gtk_image_set_from_file(GTK_IMAGE(przycisk[4][4]->obrazek),"czarny.jpg");
    gtk_image_set_from_file(przycisk[4][5]->obrazek,"bialy.jpg");
    gtk_image_set_from_file(przycisk[5][4]->obrazek,"bialy.jpg");
    gtk_image_set_from_file(przycisk[5][5]->obrazek,"czarny.jpg");

    gtk_label_set_text(historia_label,"");

    dump_cofnij();
    dump_state();
    odblokuj();
    blokuj();
}
void dump_state(){
    for(int i=8; i>=1; i--){
        for(int j=1; j<=8; j++){
                printf("%d",przycisk[j][i]->stan);
        }

        printf("\n");
    }
}

void dump_cofnij(){
    for(int i=8; i>=1; i--)
        for(int j=1; j<=8; j++)
            last_state[j][i]=przycisk[j][i]->stan;
}

void on_field_clicked(GtkWidget *btn_clicked,GdkEventButton *event,char *data){
    char tmp[4];
    sprintf(tmp,"!%s",data);
    send_signal(tmp);

    update_history(data);

    dump_cofnij();

    zmien_plansze(data,ruch_gracza);
    ruch_gracza++;
    odblokuj();
    blokuj();

    if(ruch_gracza%2==1)
        gtk_label_set_text(GTK_LABEL(tura),"Ruch ma gracz czarny");
    else
        gtk_label_set_text(GTK_LABEL(tura),"Ruch ma gracz bialy");

    //dump_state();
}

void cofnij(){

    for(int i=8; i>=1; i--)
        for(int j=1; j<=8; j++){
            przycisk[j][i]->stan=last_state[j][i];
            if(przycisk[j][i]->stan==bialy) gtk_image_set_from_file(przycisk[j][i]->obrazek,"bialy.jpg");
            else if(przycisk[j][i]->stan==czarny) gtk_image_set_from_file(przycisk[j][i]->obrazek,"czarny.jpg");
            else if(przycisk[j][i]->stan==nic) gtk_image_set_from_file(przycisk[j][i]->obrazek,"nic.jpg");
        }


    if(ruch_gracza%2==1){ //bialy chce cofnac
        char *tmp;
        tmp = gtk_label_get_text(historia_label);
        tmp[strlen(tmp)-6]='\0';

        //tmp[strlen(tmp)-9]='\0';
        gtk_label_set_text(historia_label,tmp);
        //numer_tury--;
        gtk_label_set_text(GTK_LABEL(tura),"Ruch ma gracz bialy");
    }
    else{   //czarny chcial cofnac
        char *tmp;
        tmp = gtk_label_get_text(historia_label);
        tmp[strlen(tmp)-3]='\0';
        gtk_label_set_text(historia_label,tmp);
        gtk_label_set_text(GTK_LABEL(tura),"Ruch ma gracz czarny");
    }
    ruch_gracza--;

    odblokuj();
    blokuj();

}


int blokuj(){
    int ile=0;

    for(int i=1; i<=8; i++)
        for(int j=1; j<=8; j++)
        if((ruch_gracza%2==0 && id=='C') || (ruch_gracza%2==1 && id=='B')){             //DO POPRAWY
            gtk_widget_set_sensitive(przycisk[i][j]->btn,FALSE);
        }
    for(int i=1; i<=8; i++)
        for(int j=1; j<=8; j++){
            if(przycisk[i][j]->stan!=nic){
                gtk_widget_set_sensitive(przycisk[i][j]->btn,FALSE);
            }
            else if(przycisk[i][j]->stan==nic && czy_zmiana(przycisk[i][j]->id)==0){
                    gtk_widget_set_sensitive(przycisk[i][j]->btn,FALSE);
                    gtk_image_set_from_file(przycisk[i][j]->obrazek,"nic.jpg");
            }
            else {
                if((ruch_gracza%2==0 && id=='C') || (ruch_gracza%2==1 && id=='B') )
                    gtk_image_set_from_file(przycisk[i][j]->obrazek,"nic.jpg");
                else{
                    gtk_image_set_from_file(przycisk[i][j]->obrazek,"ruch.jpg");
                    ile++;
                }
            }
        }
    return ile;
}
void odblokuj(){
    for(int i=1; i<=8; i++)
        for(int j=1; j<=8; j++){
            gtk_widget_set_sensitive(przycisk[i][j]->btn,TRUE);
        }
}

int zmien_plansze(char *id, int ruch_gracza){
    int czy_zmiana=0;
    int x,y;
    x=id[0]-'a'+1;
    y=id[1]-'1'+1;
    printf("Nacisnieto: %s, pozycje: %d, %d | ruch gracza: %d\n",id,x,y,ruch_gracza);
    if(ruch_gracza%2==0){     //jesli ruch wykonal bialy
        przycisk[x][y]->stan=bialy;
        gtk_image_set_from_file(przycisk[x][y]->obrazek,"bialy.jpg");

        int xp,yp;



/*
        for(int xi=-1; xi <= 1; xi++){
	        for(int yi=-1; yi <= 1; yi++){
		        if(xi == 0 && yi == 0)
			        continue;
		        // [xi, yi] vector
    		    xp = x + xi;
	    	    yp = y + yi;
		        while(not_out_of_bounds(xp,yp) && przycisk[xp][yp]->stan==czarny){
			        xp += xi;
    			    yp += yi;
	    	    }
		        if(not_out_of_bounds(xp,yp) && przycisk[xp][yp]->stan==bialy){
			        xp -= xi;
			        yp -= yi;
			        while(yp != y && xp != x){
				        przycisk[xp][yp]->stan=bialy;
				        gtk_image_set_from_file(przycisk[xp][yp]->obrazek,"bialy.jpg");
				        xp -= xi;
				        yp -= yi;
			        }
    		    }
	        }
        }
        */





        //na lewo
        xp=x-1;
        while(xp>=1 && przycisk[xp][y]->stan==czarny)
            xp--;
        if(xp>=1 && przycisk[xp][y]->stan==bialy){
            xp++;
            while(xp<x){
                przycisk[xp][y]->stan=bialy;
                gtk_image_set_from_file(przycisk[xp][y]->obrazek,"bialy.jpg");
                xp++;
            }
        }
        //na prawo
        xp=x+1;
        while(xp<=8 && przycisk[xp][y]->stan==czarny)
            xp++;
        if(xp<=8 && przycisk[xp][y]->stan==bialy){
            xp--;
            while(xp>x){
                przycisk[xp][y]->stan=bialy;
                gtk_image_set_from_file(przycisk[xp][y]->obrazek,"bialy.jpg");
                xp--;
            }
        }
        //w dol
        yp=y-1;
        while(yp>=1 && przycisk[x][yp]->stan==czarny)
            yp--;
        if(yp>=1 && przycisk[x][yp]->stan==bialy){
            yp++;
            while(yp<y){
                przycisk[x][yp]->stan=bialy;
                gtk_image_set_from_file(przycisk[x][yp]->obrazek,"bialy.jpg");
                yp++;
            }
        }
        //w gore
        yp=y+1;
        while(yp<=8 && przycisk[x][yp]->stan==czarny)
            yp++;
        if(yp<=8 && przycisk[x][yp]->stan==bialy){
            yp--;
            while(yp>y){
                przycisk[x][yp]->stan=bialy;
                gtk_image_set_from_file(przycisk[x][yp]->obrazek,"bialy.jpg");
                yp--;
            }
        }
        //lewo gora
        xp=x-1,yp=y+1;
        while(yp<=8 && xp>=1 && przycisk[xp][yp]->stan==czarny){
            yp++;
            xp--;
        }
        if(yp<=8 && xp>=1 && przycisk[xp][yp]->stan==bialy){
            xp++;
            yp--;
            while(yp>y){
                przycisk[xp][yp]->stan=bialy;
                gtk_image_set_from_file(przycisk[xp][yp]->obrazek,"bialy.jpg");
                xp++;
                yp--;
            }
        }
        //prawo gora
        xp=x+1,yp=y+1;
        while(yp<=8 && xp<=8 && przycisk[xp][yp]->stan==czarny){
            yp++;
            xp++;
        }
        if(yp<=8 && xp<=8 && przycisk[xp][yp]->stan==bialy){
            xp--;
            yp--;
            while(yp>y){
                przycisk[xp][yp]->stan=bialy;
                gtk_image_set_from_file(przycisk[xp][yp]->obrazek,"bialy.jpg");
                xp--;
                yp--;
            }
        }
        //prawo dol
        xp=x+1,yp=y-1;
        while(yp>=1 && xp<=8 && przycisk[xp][yp]->stan==czarny){
            yp--;
            xp++;
        }
        if(yp>=1 && xp<=8 && przycisk[xp][yp]->stan==bialy){
            xp--;
            yp++;
            while(yp<y){
                przycisk[xp][yp]->stan=bialy;
                gtk_image_set_from_file(przycisk[xp][yp]->obrazek,"bialy.jpg");
                xp--;
                yp++;
            }
        }
        //lewo dol
        xp=x-1,yp=y-1;
        while(yp>=1 && xp>=1 && przycisk[xp][yp]->stan==czarny){
            yp--;
            xp--;
        }
        if(yp>=1 && xp>=1 && przycisk[xp][yp]->stan==bialy){
            xp++;
            yp++;
            while(yp<y){
                przycisk[xp][yp]->stan=bialy;
                gtk_image_set_from_file(przycisk[xp][yp]->obrazek,"bialy.jpg");
                xp++;
                yp++;
            }
        }






    }
    if(ruch_gracza%2==1){
        numer_tury++;
        przycisk[x][y]->stan=czarny;
        gtk_image_set_from_file(przycisk[x][y]->obrazek,"czarny.jpg");

        int xp,yp;






/*
        for(int xi=-1; xi <= 1; xi++){
	        for(int yi=-1; yi <= 1; yi++){
		        if(xi == 0 && yi == 0)
			        continue;
		        // [xi, yi] vector
    		    xp = x + xi;
	    	    yp = y + yi;
		        while(not_out_of_bounds(xp,yp) && przycisk[xp][yp]->stan==bialy){
			        xp += xi;
    			    yp += yi;
	    	    }
		        if(not_out_of_bounds(xp,yp) && przycisk[xp][yp]->stan==czarny){
			        xp -= xi;
			        yp -= yi;
			        while(yp != y && xp != x){
				        przycisk[xp][yp]->stan=czarny;
				        gtk_image_set_from_file(przycisk[xp][yp]->obrazek,"czarny.jpg");
				        xp -= xi;
				        yp -= yi;
			        }
    		    }
	        }
        }
        */












        //na lewo
        xp=x-1;
        while(xp>=1 && przycisk[xp][y]->stan==bialy)
            xp--;
        if(xp>=1 && przycisk[xp][y]->stan==czarny){
            xp++;
            while(xp<x){
                przycisk[xp][y]->stan=czarny;
                gtk_image_set_from_file(przycisk[xp][y]->obrazek,"czarny.jpg");
                xp++;
            }
        }
        //na prawo
        xp=x+1;
        while(xp<=8 && przycisk[xp][y]->stan==bialy)
            xp++;
        if(xp<=8 && przycisk[xp][y]->stan==czarny){
            xp--;
            while(xp>x){
                przycisk[xp][y]->stan=czarny;
                gtk_image_set_from_file(przycisk[xp][y]->obrazek,"czarny.jpg");;
                xp--;
            }
        }
        //w dol
        yp=y-1;
        while(yp>=1 && przycisk[x][yp]->stan==bialy)
            yp--;
        if(yp>=1 && przycisk[x][yp]->stan==czarny){
            yp++;
            while(yp<y){
                przycisk[x][yp]->stan=czarny;
                gtk_image_set_from_file(przycisk[x][yp]->obrazek,"czarny.jpg");
                yp++;
            }
        }
        //w gore
        yp=y+1;
        while(yp<=8 && przycisk[x][yp]->stan==bialy)
            yp++;
        if(yp<=8 && przycisk[x][yp]->stan==czarny){
            yp--;
            while(yp>y){
                przycisk[x][yp]->stan=czarny;
                gtk_image_set_from_file(przycisk[x][yp]->obrazek,"czarny.jpg");
                yp--;
            }
        }
        //lewo gora
        xp=x-1,yp=y+1;
        while(yp<=8 && xp>=1 && przycisk[xp][yp]->stan==bialy){
            yp++;
            xp--;
        }
        if(yp<=8 && xp>=1 && przycisk[xp][yp]->stan==czarny){
            xp++;
            yp--;
            while(yp>y){
                przycisk[xp][yp]->stan=czarny;
                gtk_image_set_from_file(przycisk[xp][yp]->obrazek,"czarny.jpg");
                xp++;
                yp--;
            }
        }
        //prawo gora
        xp=x+1,yp=y+1;
        while(yp<=8 && xp<=8 && przycisk[xp][yp]->stan==bialy){
            yp++;
            xp++;
        }
        if(yp<=8 && xp<=8 && przycisk[xp][yp]->stan==czarny){
            xp--;
            yp--;
            while(yp>y){
                przycisk[xp][yp]->stan=czarny;
                gtk_image_set_from_file(przycisk[xp][yp]->obrazek,"czarny.jpg");;
                xp--;
                yp--;
            }
        }
        //prawo dol
        xp=x+1,yp=y-1;
        while(yp>=1 && xp<=8 && przycisk[xp][yp]->stan==bialy){
            yp--;
            xp++;
        }
        if(yp>=1 && xp<=8 && przycisk[xp][yp]->stan==czarny){
            xp--;
            yp++;
            while(yp<y){
                przycisk[xp][yp]->stan=czarny;
                gtk_image_set_from_file(przycisk[xp][yp]->obrazek,"czarny.jpg");
                xp--;
                yp++;
            }
        }
        //lewo dol
        xp=x-1,yp=y-1;
        while(yp>=1 && xp>=1 && przycisk[xp][yp]->stan==bialy){
            yp--;
            xp--;
        }
        if(yp>=1 && xp>=1 && przycisk[xp][yp]->stan==czarny){
            xp++;
            yp++;
            while(yp<y){
                przycisk[xp][yp]->stan=czarny;
                gtk_image_set_from_file(przycisk[xp][yp]->obrazek,"czarny.jpg");
                xp++;
                yp++;
            }
        }





    }
    dump_state();
}

int czy_zmiana(char *id){
    int x,y;
    x=id[0]-'a'+1;
    y=id[1]-'1'+1;

    if(ruch_gracza%2==0){     //jesli ruch wykonal bialy
        int xp,yp;




        /*
        for(int xi=-1; xi<=1; xi++){
            for(int yi=-1; yi<=1; yi++){
                if(xi == 0 && yi == 0)
                    continue;
                xp += xi;
                yp += yi;
                while(not_out_of_bounds(xp,yp) && przycisk[x][yp]->stan==czarny){
                    xp += xi;
                    yp += yi;
                }
                if(not_out_of_bounds(xp,yp) && przycisk[xp][yp]->stan==bialy)
                    return 1;
            }
        }

        */





        //na lewo
        xp=x-1;
        while(xp>=1 && przycisk[xp][y]->stan==czarny) xp--;
        if(xp>=1 && przycisk[xp][y]->stan==bialy && xp<x-1) return 1;

        //na prawo
        xp=x+1;
        while(xp<=8 && przycisk[xp][y]->stan==czarny) xp++;
        if(xp<=8 && przycisk[xp][y]->stan==bialy && xp>x+1) return 1;

        //w dol
        yp=y-1;
        while(yp>=1 && przycisk[x][yp]->stan==czarny) yp--;
        if(yp>=1 && przycisk[x][yp]->stan==bialy && yp<y-1) return 1;

        //w gore
        yp=y+1;
        while(yp<=8 && przycisk[x][yp]->stan==czarny) yp++;
        if(yp<=8 && przycisk[x][yp]->stan==bialy && yp>y+1) return 1;

        //lewo gora
        xp=x-1,yp=y+1;
        while(yp<=8 && xp>=1 && przycisk[xp][yp]->stan==czarny){
            yp++;
            xp--;
        }
        if(yp<=8 && xp>=1 && przycisk[xp][yp]->stan==bialy && xp<x-1) return 1;

        //prawo gora
        xp=x+1,yp=y+1;
        while(yp<=8 && xp<=8 && przycisk[xp][yp]->stan==czarny){
            yp++;
            xp++;
        }
        if(yp<=8 && xp<=8 && przycisk[xp][yp]->stan==bialy && xp>x+1) return 1;

        //prawo dol
        xp=x+1,yp=y-1;
        while(yp>=1 && xp<=8 && przycisk[xp][yp]->stan==czarny){
            yp--;
            xp++;
        }
        if(yp>=1 && xp<=8 && przycisk[xp][yp]->stan==bialy && xp>x+1) return 1;

        //lewo dol
        xp=x-1,yp=y-1;
        while(yp>=1 && xp>=1 && przycisk[xp][yp]->stan==czarny){
            yp--;
            xp--;
        }
        if(yp>=1 && xp>=1 && przycisk[xp][yp]->stan==bialy && xp<x-1) return 1;

    }
    if(ruch_gracza%2==1){
        int xp,yp;






/*
        for(int xi=-1; xi<=1; xi++){
            for(int yi=-1; yi<=1; yi++){
                if(xi == 0 && yi == 0)
                    continue;
                xp += xi;
                yp += yi;
                while(not_out_of_bounds(xp,yp) && przycisk[x][yp]->stan==bialy){
                    xp += xi;
                    yp += yi;
                }
                if(not_out_of_bounds(xp,yp) && przycisk[xp][yp]->stan==czarny)
                    return 1;
            }
        }

*/






        //na lewo
        xp=x-1;
        while(xp>=1 && przycisk[xp][y]->stan==bialy) xp--;
        if(xp>=1 && przycisk[xp][y]->stan==czarny && xp<x-1) return 1;

        //na prawo
        xp=x+1;
        while(xp<=8 && przycisk[xp][y]->stan==bialy) xp++;
        if(xp<=8 && przycisk[xp][y]->stan==czarny && xp>x+1) return 1;

        //w dol
        yp=y-1;
        while(yp>=1 && przycisk[x][yp]->stan==bialy) yp--;
        if(yp>=1 && przycisk[x][yp]->stan==czarny && yp<y-1) return 1;

        //w gore
        yp=y+1;
        while(yp<=8 && przycisk[x][yp]->stan==bialy) yp++;
        if(yp<=8 && przycisk[x][yp]->stan==czarny && yp>y+1) return 1;

        //lewo gora
        xp=x-1,yp=y+1;
        while(yp<=8 && xp>=1 && przycisk[xp][yp]->stan==bialy){
            yp++;
            xp--;
        }
        if(yp<=8 && xp>=1 && przycisk[xp][yp]->stan==czarny && xp<x-1) return 1;

        //prawo gora
        xp=x+1,yp=y+1;
        while(yp<=8 && xp<=8 && przycisk[xp][yp]->stan==bialy){
            yp++;
            xp++;
        }
        if(yp<=8 && xp<=8 && przycisk[xp][yp]->stan==czarny && xp>x+1) return 1;

        //prawo dol
        xp=x+1,yp=y-1;
        while(yp>=1 && xp<=8 && przycisk[xp][yp]->stan==bialy){
            yp--;
            xp++;
        }
        if(yp>=1 && xp<=8 && przycisk[xp][yp]->stan==czarny && xp>x+1) return 1;

        //lewo dol
        xp=x-1,yp=y-1;
        while(yp>=1 && xp>=1 && przycisk[xp][yp]->stan==bialy){
            yp--;
            xp--;
        }
        if(yp>=1 && xp>=1 && przycisk[xp][yp]->stan==czarny && xp<x-1) return 1;

    }
    return 0;
}
