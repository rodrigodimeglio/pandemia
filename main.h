#define _GAME_H_    1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <ncurses.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>
#include <ctype.h>


#define PORTNO 51430

/*              Formato de la comunicación entre cliente-servidor
 *Comienza la conexión
 *  el cliente envia el player creado al servidor
 *  el servidor inserta este player en la lista de clientes
 * 
*/

#define SCREEN_WIDTH            50                          /*  */
#define SCREEN_HEIGHT           50                          /*  */
#define MAX_PLAYERS             10                          /*  max cantidad de jugadores que soporta el server */
#define TIMEOUT_US_STDIN_VAL    10                          /*  timeout del stdin para mover los player */
#define TIMEOUT_S_STDIN_VAL     0                           /*  timeout del stdin para mover los player */
#define NAME_LEN                8                           /*  largo del nombre en bytes   */            
#define PLAYER_SIZE             12//sizeof(Player_t)
#define PLAYER_BUFF_SIZE        (PLAYER_SIZE * MAX_PLAYERS )  
#define BULLET_ID               70                          /* ID a partir del cual los player son balas */
#define BULLET_DIR_LEFT         1
#define BULLET_DIR_RIGHT        2
#define BULLET_DIR_UP           3
#define BULLET_DIR_DOWN         4
#define BULLET_SPEED_US         80000
#define CMD_TXT                 1                           /*  Text from server        */
#define CMD_PLAYER              2                           /*  Players from server     */
#define CMD_BULLETS             3                           /*  Bullets from server     */



typedef enum
{
    state_txting,
    state_runing
}SERVERStateMachine_t;

typedef struct __attribute__((__packed__))
{
    char cmd;
    char len;
}Command_t;

typedef struct __attribute__((__packed__))
{
    Command_t cmd;
    char buf[PLAYER_BUFF_SIZE];
}ToSend_t;

typedef struct __attribute__((__packed__))
{
    char playerID;
    char name[NAME_LEN];
    char vida;
    char x;
    char y;
}Player_t;


typedef struct s_node{
	Player_t player;
	int fd;
    pthread_mutex_t pmtx;
}PLAYERListNode_t;


typedef struct s_list{
    PLAYERListNode_t node;
    struct s_list * next;
}PLAYERList_t;


/*  Globals   */
extern bool RUNFlag;
extern char ip_server[20],my_ip[20]; 
extern char player_name[30], txtmsg[30];
extern pthread_t server_threads[MAX_PLAYERS];
extern pthread_t client_thread1, client_thread2;
extern pthread_mutex_t PLAYERListMUTEX;
extern PLAYERList_t * plist, *blist;                        /*  listas  */
extern int _clients, _bullets, bullet_flag, CLIENTFlag;     /*  flags   */
extern  Player_t player_buff[MAX_PLAYERS];
extern WINDOW * mainwin;
extern pthread_t sthread;


/*  Movimientos y muestras en la pantalla  */
void PLAYERDisplayW(WINDOW * win, Player_t player);

void PLAYERAction(int ch, Player_t * player);

void PLAYERDisplay(Player_t player);

void BULLETDisplay(Player_t bullet);

Player_t BULLETCreate(char id, char dir, char from, char x, char y);

void BULLETCreator(int ch, Player_t player);

int BULLETCheckImapct(Player_t bullet);

int BULLETAction(Player_t * bullet);

void PLAYERDisconnectedHandler(char ID);


/*  Funciones de cliente y servidor */
    //forkeables
void SERVERServerForEver(void);

void CLIENTClientForEver(void);
    //threadeables
void * SERVEROnlineClientHandler(void * ffd);

void * SERVERNewClientHandler(void * ffd);

void * SERVERUpdateBullets(void);

void * CLIENTRunRead(void * ffd);   /*  Lee el buffer y lo muestra en la pantalla   */

void * CLIENTRunWrite(void * ffd);  /*  Lee STDIN y el socket. Tambien le manda el cmd ingresado al socket   */

void CLIENTClosedHandler(int fd);


/*  Signals and  Handlers  */
void SERVERSignalHandler(int signum);

void SERVERTimerSignalHandler(int signum);

void SERVERInitSignals(void);

void SERVERInitTimer(void);

void CLIENTSignalHandler(int signum);

void CLIENTTimerSignalHandler(int signum);

void CLIENTInitSignals(void);

void CLIENTInitTimer(void);

void CLEARScreen(void);

void OFFLINEServer(void);

void * SERVERCommander(void);

void error(char * msg);
static inline int max(int a, int b);
void get_ip(char * str);
void sendt(int fd, char * buf, int len, pthread_mutex_t * mtx, char * errmsg);
void reciv(int fd, char * buf, int len, char * errmsg);

/*******************************************************************************************************************************************************************/


/* inserta un item al comienzo de la lista */
void PLAYERListPush(PLAYERList_t **head, Player_t player, int fd);

/* inserta un item al fin de la lista */
void PLAYERListAppend(PLAYERList_t **head, Player_t player, int fd);

/* remueve el ultimo item de la lista */
PLAYERListNode_t PLAYERListPop(PLAYERList_t **head);


void PLAYERListPrint(PLAYERList_t * head, char * path);

/* busca el val en la lista y devuelve el idx o -1 si no esta */
int PLAYERListFindByfd(PLAYERListNode_t * head, int fd);

PLAYERListNode_t PLAYERListGetByidx(PLAYERList_t * head, int idx);

PLAYERListNode_t PLAYERListGetByfd(PLAYERList_t * head, int fd);

PLAYERListNode_t PLAYERListGetByID(PLAYERList_t * head, int fd);

PLAYERListNode_t * PLAYERListGetNodeByidx(PLAYERList_t * head, int idx);

int PLAYERListUpdateByID(PLAYERList_t **head, Player_t player, char id);

int PLAYERListSize(PLAYERList_t * head);

int PLAYERListMaxID(PLAYERList_t * head);

int PLAYERListRemoveByfd(PLAYERList_t **head, int fd);

int PLAYERListRemoveByID(PLAYERList_t **head, int id);

int PLAYERListUpdateByfd(PLAYERList_t **head, Player_t player, int fd);

void PLAYERListInit(void);

void PLAYERListDestroy(PLAYERList_t ** head);

