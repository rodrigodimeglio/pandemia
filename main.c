#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <ncurses.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>

#include "main.h"
#include "msglist.h"

/*  Globals   */
char ip_server[20],my_ip[20];
bool RUNFlag;
char player_name[30];
int _clients = 0, _bullets = 0, bullet_flag, CLIENTFlag = 0;
pthread_mutex_t PLAYERListMUTEX;
PLAYERList_t * plist = NULL, *blist = NULL;
Player_t player_buff[MAX_PLAYERS];
pthread_t server_threads[MAX_PLAYERS];
pthread_t client_thread1, client_thread2;
pthread_t sthread;
MSGManager_t MSGMngr;


WINDOW * mainwin;



int main (void)
{
    //lf = fopen("log.txt","w+");
    //fprintf(lf,"- Main --> iniciado\n");
    RUNFlag = true;

    pid_t pid;
    mainwin = initscr();
    int oldcursor = curs_set(0);
    keypad(mainwin,TRUE);
    //start_color();
    //init_pair(1,COLOR_BLUE, COLOR_GREEN);
    //init_pair(2,COLOR_RED, COLOR_BLACK);
    

    int ch;
    attron(COLOR_PAIR(2));
    get_ip(ip_server);

    memcpy(my_ip,ip_server,20);

    while(1)
    {
        printw("                                                                \n");
        printw("                            P A N D E M I A                     \n");
        printw("                                                                \n\n\n");
    
        printw("                Tu IP es %s                                     \n\n\n",ip_server);
        
        printw("       (1)      Ayuda                                           \n\n");
        printw("       (2)      Crear una partida                               \n\n");
        printw("       (3)      Unirse a una partida                            \n\n");
        printw("       (q)      Salir del juego                                 \n\n");


        ch = getch();
        switch(ch)
        {
            case '1':
                clear();
                printw("                Ayuda                           \n\n");
                printw("    Te moves con: las flechitas  \n ");
                printw("    Disparas con: w a s d \n");  
                printw("    Chateas con: y  apreta Enter para enviar el mensaje\n");  
                printw("    Disparale al resto  \n\n\n");
                printw("                Historia                          \n\n");
                printw("    Se solto una pandemia y todos estan muy enfermos. Usted se piensa que los otros enfermos\n ");
                printw("lo quieren matar pero usted tambien esta enfermo, por eso se matan entre todos.\n");
                getch();
                clear();
                break;
            
            case '2':
                clear();
                printw("                Crear  una partida                            \n\n");
                printw("        Ingresa un nombre de jugador: ");
                scanw("%s\n",player_name);
                pid = fork();
                noecho();
                erase();

                switch(pid)
                {
                    case 0://hijo
                        clear();
                        //SERVERServerForEver();
                        CLIENTClientForEver();

                        break;
                    case -1: //error
                        error("FORK ERROR");
                        break;
                    default:
                        //initscr();
                        //attron(COLOR_PAIR(1));
                        SERVERServerForEver();
                        //CLIENTClientForEver();
                        break;
                }

            case '3':
                clear();
                printw("                Unirse a una partida                            \n\n");
                printw("        Ingresa la IP del server: ");
                scanw("%s\n",ip_server);
                printw("        Ingresa un nombre de jugador: ");
                scanw("%s\n",player_name);
                noecho();
                clear();
                //attron(COLOR_PAIR(1));
                CLIENTClientForEver();
                break;

            case 'q':
                curs_set(oldcursor);
                endwin();
                return 0;
                break;
            
            default:
                clear();
                
        }


    }//end while 1
    endwin();
    //SERVERServerForEver();
    //CLIENTClientForEver();
    //endwin();
    return 0;
}

