#include "main.h"
#include "msglist.h"

void PLAYERDisplayW(WINDOW * win, Player_t player);



void PLAYERDisconnectedHandler(char ID)
{
    PLAYERListNode_t nodo = PLAYERListGetByID(plist,ID);
    if(nodo.fd > 0)
    {
        close(nodo.fd);
        if(PLAYERListRemoveByID(&plist,ID)==0)
            _clients--;
    }
}

void PLAYERAction(int ch, Player_t * player)
{
    int h_offset = 4;
    switch (ch)
        {
            case KEY_DOWN:
                (*player).y++ ;
                if((*player).y > SCREEN_HEIGHT)
                    (*player).y = SCREEN_HEIGHT;
                if((*player).y < h_offset)
                    (*player).y = h_offset;
                break;
            case KEY_UP:
                    (*player).y-- ;
                    if((*player).y < h_offset)
                    (*player).y = h_offset;
                if((*player).y > SCREEN_HEIGHT)
                    (*player).y = SCREEN_HEIGHT;
                break;
            case KEY_LEFT:
                    (*player).x-- ;
                    if((*player).x > SCREEN_WIDTH)
                    (*player).x = SCREEN_WIDTH;
                if((*player).x < 0)
                    (*player).x = 0;
                break;
            case KEY_RIGHT:
                    (*player).x++ ;
                    if((*player).x < 0)
                    (*player).x = 0;
                if((*player).x > SCREEN_WIDTH)
                    (*player).x = SCREEN_WIDTH;
            default:
                break;
        }
}

void PLAYERDisplay(Player_t player)
{
  
    int x = player.x;
    int y = player.y;

    //attron(COLOR_PAIR(1));

    //mvprintw(y-3,x-3,"       ");
    mvprintw(y-2,x-3,"%s ",player.name);
    //mvprintw(y-1,x-3," |T~T| ");
    mvprintw(y-1,x-3,"|UwU|");
    mvprintw(y,x-3,  "-----");
   // mvprintw(y+1,x-3,"       ");
}

Player_t BULLETCreate(char id, char dir, char from, char x, char y)
{
    Player_t bullet;
    for(int i =0; i<NAME_LEN; i++)
        bullet.name[i] = 0;
    bullet.playerID = id;       //ID de la bala
    bullet.name[0] = dir;       //direccion en que va la bala
    bullet.vida = from;         //ID del que disparo
    bullet.x = x;
    bullet.y = y;
    return bullet;
}

void BULLETCreator(int ch, Player_t player)
{
    Player_t bullet;
    switch(ch)
        {
            case 'a':
                bullet = BULLETCreate(BULLET_ID+_bullets, BULLET_DIR_LEFT, player.playerID, player.x, player.y);
                PLAYERListPush(&blist,bullet,_bullets);
                _bullets++;
                break;

            case 's':
                bullet = BULLETCreate(BULLET_ID+_bullets, BULLET_DIR_DOWN, player.playerID, player.x, player.y);
                PLAYERListPush(&blist,bullet,_bullets);
                _bullets++;
                break;

            case 'd':
                bullet = BULLETCreate(BULLET_ID+_bullets, BULLET_DIR_RIGHT, player.playerID, player.x, player.y);
                PLAYERListPush(&blist,bullet,_bullets);
                _bullets++;
                break;

            case 'w':
                bullet = BULLETCreate(BULLET_ID+_bullets, BULLET_DIR_UP, player.playerID, player.x, player.y);
                PLAYERListPush(&blist,bullet,_bullets);//cargamos la bala en la lista de las balas
                _bullets++;
                break;

            default:
                break;
        }//fin switch

}

int BULLETAction(Player_t * bullet)
{
    int r;
    int h_offset = 2;
    switch((*bullet).name[0])
    {
        case BULLET_DIR_UP:
            (*bullet).y--;
            if((*bullet).y == h_offset)
                return 0;
            else 
            {   
                r =  BULLETCheckImapct(*bullet);  
                return r;
            }
            break;

        case BULLET_DIR_DOWN:
            (*bullet).y++;
            if((*bullet).y == SCREEN_HEIGHT)
                return 0;
            else 
            {   
                r = BULLETCheckImapct(*bullet);  
                return r;
            }
            break;

        case BULLET_DIR_LEFT:
            (*bullet).x--;
            if((*bullet).x == 0)
                return 0;
            else 
            {   
                r =  BULLETCheckImapct(*bullet);  
                return r;
            }
            break;

        case BULLET_DIR_RIGHT:
            (*bullet).x++;
            if((*bullet).x == SCREEN_WIDTH)
                return 0;
            else 
            {   
                r =  BULLETCheckImapct(*bullet);  
                return r;
            }
            break;
        
        default:
            return -1;
    }
    return 0;       //la idea es que retorne el ID del que le pego, BULLET_ID sino le pego a nadie o -1 en caso de error;
}

void BULLETDisplay(Player_t bullet)
{
    int x = bullet.x;
    int y = bullet.y;
    mvprintw(y,x,"O");
}

int BULLETCheckImapct(Player_t bullet)
{
    int px, py, xub,xlb,yub,ylb;
    int x = bullet.x; 
    int y = bullet.y;
    PLAYERListNode_t nodo;

    for(int i = 0; i< _clients; i++)
    {
        nodo = PLAYERListGetByidx(plist,i);
        px = nodo.player.x; py = nodo.player.y;

        xlb = px-3;    xub = px + 3;                    // x upper - lower bounds
        ylb = py-1;    yub = py+1;                      // y upper - lower bounds

        if(nodo.player.playerID == bullet.vida)         //vida tiene el ID del player que disparo
            continue;

        if ((x < xub && x > xlb) && (y < yub && y > ylb))
        {
            if(nodo.player.playerID == 0)
                return MAX_PLAYERS+1;
            else
                return nodo.player.playerID;
        }
    }

    return BULLET_ID;
}


void SERVERServerForEver(void)
{
    SERVERInitSignals();
    PLAYERListInit();
    fd_set fds;

    int ret;
   
    /*****************      BEGIN SOCKETS    **********************/
        int srv_sock, con_sock;
        struct sockaddr_in srv_addr,cli_addr;
        socklen_t cli_len = sizeof(cli_addr);

        srv_sock = socket(AF_INET,SOCK_STREAM,0);
        if(srv_sock < 0)    
            error("socket failed");
        if (setsockopt(srv_sock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
            error("setsockopt(SO_REUSEADDR) failed");
        bzero((char *)&srv_addr, sizeof(srv_addr));
        srv_addr.sin_family = AF_INET;
        srv_addr.sin_addr.s_addr = INADDR_ANY;
        srv_addr.sin_port = htons(PORTNO);
        if(bind(srv_sock, (struct sockaddr *) &srv_addr,sizeof(srv_addr)) < 0)
            error("binding error");

        
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&sthread,&attr, (void *) SERVERUpdateBullets, NULL);
        pthread_create(&sthread,&attr, (void *) MSGSender, NULL);


        listen(srv_sock,5);
    /**************************************************************/

    while(RUNFlag)
    {
        FD_ZERO(&fds);
        FD_SET(srv_sock, &fds);

        ret = select(srv_sock+1, &fds, NULL,NULL,NULL);

        if(ret == -1) 
        {
            if(errno == EBADF)
                error("select error");
        }
            
        if(ret)
        {
            if(FD_ISSET(srv_sock,&fds))
            {
                con_sock = accept(srv_sock,(struct sockaddr *) &cli_addr,&cli_len);
                SERVERNewClientHandler((void*)&con_sock);
            }           
        }
    }
    MSGDestroy();
    exit(EXIT_SUCCESS);
}


void * SERVERUpdateBullets(void)
{
    PLAYERListNode_t node_bullet;
    int baret = 0;
    
    while(RUNFlag)
    {
        usleep(BULLET_SPEED_US);
 
        bullet_flag = 0;

        for(int i =0; i<_bullets; i++)
        {
            node_bullet = PLAYERListGetByidx(blist,i);

            baret = BULLETAction(&(node_bullet.player));
            if(baret == 0)
            {
                PLAYERListRemoveByID(&blist, node_bullet.player.playerID);  //si devuelve 0 es porque la bala se fue del mapa
                _bullets--;
                if(_bullets<0)
                    _bullets = 0;
            }  

            else if(baret < BULLET_ID)
            {
                if(baret == MAX_PLAYERS+1)
                    PLAYERDisconnectedHandler(0);
                else
                    PLAYERDisconnectedHandler(baret);
            }
            if(baret >= BULLET_ID)
            {
                if(PLAYERListUpdateByfd(&blist, node_bullet.player, node_bullet.fd)<0)
                    printf("error updating bullet list\n");
            }
                
        }
        
    }
    return 0;
}


void * SERVERNewClientHandler(void * ffd)
{
    
    int fd = *(int *)ffd;
    Player_t new_player;

    int ret = read(fd,&new_player,PLAYER_SIZE);//read player from client
    if(ret <0) error("SERVERNewClientHandler reading error");
    new_player.playerID = _clients;//asign playerID. asi puedo utilizar el player ID para saber cuantos players quedan en la lista
    new_player.x = 10;
    new_player.y = 10;

    ret = write(fd,&new_player,PLAYER_SIZE);//write news player data to client, so player now have current playerID
    if(ret <0) error("SERVERNewClientHandler writing error");

    PLAYERListPush(&plist,new_player,fd);//add new player to player list
    pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&server_threads[_clients],&attr, (void *) SERVEROnlineClientHandler, (void*)&fd);     //manage client forever
    _clients++;
    return 0;
}


void * SERVEROnlineClientHandler(void * ffd)
{
    
    ToSend_t tosend;

    SERVERStateMachine_t state = state_runing;

    int fd = *(int *)ffd;
    int ret,rets,ch,idx,idx0=0,baret;
    char msg[sizeof(player_buff)];

    Player_t bullet;
    PLAYERListNode_t nodo; 
    PLAYERListNode_t node_bullet;
    nodo = PLAYERListGetByfd(plist,fd);
    char thisID = nodo.player.playerID;
    pthread_mutex_t pmtx = nodo.pmtx;

    fd_set fds;

    struct timeval tv;
        tv.tv_sec =  0;
        tv.tv_usec = 10000;


    idx0 = sprintf(msg,"%s",nodo.player.name);
    msg[idx0++] =':';
    msg[idx0++] =' ';
    idx = idx0;

    while(RUNFlag)
    {

        FD_ZERO(&fds);
        FD_SET(fd,&fds);

        rets = select(fd+1, &fds, NULL, NULL, &tv);

        if(rets <0)
        {
            if(errno == EBADF)
            {
                PLAYERDisconnectedHandler(thisID);
                return 0;
            }
            error("SERVEROnlineClient select error");
        }
        if(rets>0)
        {
            if(FD_ISSET(fd,&fds))
            {
                nodo = PLAYERListGetByfd(plist,fd);
                ret = read(fd,&ch,sizeof(ch));

                if((ch == 'y')&&(state==state_runing))
                {
                    state = state_txting;
                    goto next;
                }


                if(ret <0)
                {
                    if(errno == ECONNRESET)
                    {
                        PLAYERDisconnectedHandler(thisID);
                        while(1);
                    }
                    error("SERVEROnlineClient reading error");
                }

               switch(state)
                {
                    case state_runing:
                        PLAYERAction(ch,&(nodo.player));
                        PLAYERListUpdateByfd(&plist, nodo.player, fd);
                        BULLETCreator(ch,nodo.player);
                        break;
                        
                    case state_txting:
                        msg[idx] = (char) ch;

                        if(ch == 10 || ch == KEY_ENTER)
                        {
                            msg[idx] = '\0';
                            MSGPut(msg, idx);
                            state = state_runing;
                            idx = idx0;
                            break;
                        }
                        idx++;
                        break;
                }  
            }//fin if(FD_ISSET(fd))
        }//fin if(ret)
        
        else//select time out
        {
        }
next:
        /*  Carga las balas en la lista */
        for(int i =0; i<_bullets; i++)
        {
            node_bullet = PLAYERListGetByidx(blist,i);
            player_buff[i] = node_bullet.player;
        }

        /*  Carga los players en la lista */
        for(int i=0; i<_clients; i++)
        {
            nodo = PLAYERListGetByidx(plist,i);
            player_buff[i+_bullets] = nodo.player;
        }

        memcpy(tosend.buf, player_buff, sizeof(player_buff));

        tosend.cmd.cmd = CMD_PLAYER;            //comando CMD_PLAYER indica que se envia la posicion de los players
        tosend.cmd.len = sizeof(player_buff);

        sendt(fd,(char *)&tosend,sizeof(tosend),&pmtx,"SERVEROnlineClient writing error");

        
    }
    return 0;
}



void CLIENTClientForEver(void)
{
    CLIENTInitTimer();
    CLIENTInitSignals();

    Player_t player;
    Player_t myplayer={0,"kM3n3M",1,15,15};
    int i =0;
    while(i<NAME_LEN)
    {
        myplayer.name[i] = player_name[i];
        if (player_name[i] == '\0') break;
        i++;
    }

    myplayer.name[NAME_LEN-1]='\0';
    fd_set fds;

    int ch;
    int id;
    int idx =0;
    int ret;

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("socket error");

    server = gethostbyname((char *)ip_server);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORTNO);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("error connecting");
   

        ret = write(sockfd,&myplayer,PLAYER_SIZE);          //Le mando al server mi nombre de jugador
                                                            //          |
                                                            //          V
        ret = read(sockfd,&myplayer,PLAYER_SIZE);           //el server me devuelve mi player ID;

        int MYID = myplayer.playerID;


        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&client_thread1,&attr, (void *) CLIENTRunRead, (void *) &sockfd);     //manage client forever
        pthread_create(&client_thread2,&attr, (void *) CLIENTRunWrite, (void *) &sockfd);     //manage client forever

        pthread_join(client_thread1, NULL);
        pthread_join(client_thread2, NULL);
        exit(1);
}


void * CLIENTRunRead(void * ffd)
{
    int fd = *(int *)ffd;
    int ret, idx=0;
    move(0,0);
    printw("Tu IP es %s   Conectado a IP %s\n",my_ip,ip_server);
    while(RUNFlag)
    {
        idx = 0;
        CLEARScreen();
        usleep(5000);
        while(1)
        {
            if(player_buff[idx].playerID >= BULLET_ID)
                BULLETDisplay(player_buff[idx]);
            else
                PLAYERDisplay(player_buff[idx]);
            if(player_buff[idx].playerID == 0) break;
            idx++;
        }   
        refresh();
    }
    CLIENTClosedHandler(fd);
    return 0;
}

void * CLIENTRunWrite(void * ffd)
{
    int fd = *(int*) ffd;
    int ret, idx=0 ,ch, rets;
    char buffer[sizeof(player_buff)];
    char msg[sizeof(player_buff)];
    
    Command_t cmd;
    char cmdbuf[2];

    fd_set fdset;

    struct timeval tv;
        tv.tv_sec =  0;
        tv.tv_usec = 10000;

    while(RUNFlag)
    {

        FD_ZERO(&fdset);
        FD_SET(STDIN_FILENO, &fdset);
        FD_SET(fd, &fdset);
        
        rets = select(fd+1, &fdset, NULL, NULL, &tv);
        if(rets<0) 
        {
            if(errno == EBADF)
            {
                CLIENTClosedHandler(fd);
                return 0;
            }
            error("CLIENRunWrite select error");
        }

        if(rets)
        {
            if(FD_ISSET(STDIN_FILENO, &fdset))
            {
                ch = getch();
                ret = write(fd, &ch, sizeof(ch));  
                if(ret<0) 
                {
                    if(errno == EBADF)
                        {
                            CLIENTClosedHandler(fd);
                            return 0;
                        }
                    error("CLIENTRunWrite write error");
                }
                if(ret == 0) 
                {
                    CLIENTClosedHandler(fd);
                    return 0;
                }

            }

            if(FD_ISSET(fd, &fdset))
            {
                memset(cmdbuf,0, sizeof(cmdbuf));
                ret = read(fd,&cmdbuf,sizeof(cmdbuf));

                memcpy(&cmd,cmdbuf,sizeof(cmdbuf));

                if(ret<0) error("CLIENTRunWrite read error");
                if(ret == 0)
                {
                    CLIENTClosedHandler(fd);
                    return 0;
                }

                if(cmd.cmd == CMD_TXT) 
                {
                    move(1,0);
                    printw("                                          ");
                    ret = read(fd, msg, cmd.len);
                    if(ret<0)error("CLIENTRinWrite reading msg error");
                    msg[cmd.len-1] = '\0';
                    move(1,0);
                    printw("%s",msg);
                }

                if(cmd.cmd == CMD_PLAYER)
                {
                    memset(player_buff,0,sizeof(player_buff));
                    ret = read(fd, player_buff, cmd.len);
                }

            }
        }

    }
    CLIENTClosedHandler(fd);
    return 0;
}

void CLIENTClosedHandler(int fd)
{
    CLEARScreen();
    printw("Te bajaron o te banearon del server\n");
    printw("Apreta una tecla para salir\n");
    getch();
    RUNFlag = 0;
    //pthread_join(client_thread1,NULL);
    //pthread_join(client_thread2,NULL);
    endwin();
    //exit(1);
}



/*  Signals and  Handler  */
void SERVERSignalHandler(int signum)
{
    switch ( signum ) 
    {
    case SIGALRM:
        return;

    case SIGTERM:
        RUNFlag = 0;
        MSGDestroy();
        PLAYERListDestroy(&plist);
        PLAYERListDestroy(&blist);
        exit(EXIT_SUCCESS);
        break;

    case SIGINT:
        RUNFlag = 0;
        MSGDestroy();    
        exit(EXIT_SUCCESS);
        break;

    case SIGCHLD:
        RUNFlag = 0;
        MSGDestroy();   
        PLAYERListDestroy(&plist);
        PLAYERListDestroy(&blist);
        exit(EXIT_SUCCESS);
        break;
    }
}

void SERVERTimerSignalHandler(int signum)
{

    PLAYERListNode_t node_bullet;
    int baret;
        if(signum == SIGALRM)
            bullet_flag = 1;
        
}

void SERVERInitSignals()
{
    struct sigaction sa;

    sa.sa_handler = SERVERTimerSignalHandler;
    sa.sa_flags   = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);

    sa.sa_handler = SERVERSignalHandler;
    sa.sa_flags   = 0;

    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGINT,  &sa, NULL);
}


void SERVERInitTimer()
{
    struct itimerval it;

    timerclear(&it.it_interval);
    timerclear(&it.it_value);

    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 100000;    //bullet speed

    it.it_value.tv_sec    = 0;
    it.it_value.tv_usec   = 50000;

    setitimer(ITIMER_REAL, &it, NULL);
}


void CLIENTSignalHandler(int signum)
{
    //extern WINDOW * mainwin;
    switch ( signum ) 
    {
    case SIGTERM:
        RUNFlag=0;
        delwin(mainwin);
        endwin();
        exit(EXIT_SUCCESS);
        break;

    case SIGINT:
        RUNFlag=0;
        delwin(mainwin);
        endwin();
        exit(EXIT_SUCCESS);
        break;
    }

}


void CLIENTInitSignals(void)
{
    struct sigaction sa;

    sa.sa_handler = CLIENTSignalHandler;
    sa.sa_flags   = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGINT,  &sa, NULL);

    sa.sa_handler = CLIENTTimerSignalHandler;
    sa.sa_flags   = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);
}


void CLIENTInitTimer(void)
{
    struct itimerval it;

    timerclear(&it.it_interval);
    timerclear(&it.it_value);

    /* Interval for periodic timer */
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 16000;   
    /* Time until next expiration */
    it.it_value.tv_sec    = 0;
    it.it_value.tv_usec   = 16000;

    setitimer(ITIMER_REAL, &it, NULL);

}

void CLIENTTimerSignalHandler(int signum)
{
        if(signum == SIGALRM)
            CLIENTFlag = 1;
}

void CLEARScreen(void)
{
    move(2,0);
    for (int i=3; i<SCREEN_HEIGHT; i++)
        printw("                                                       \n");
}

void error(char * msg)
{
    RUNFlag = 0;
    sleep(1);
    FILE * f = fopen("log.txt","a+");
    fprintf(f,"errno:%d\nmsg:%s\n",errno,msg);
    fclose(f);
    exit(EXIT_FAILURE);
}

static inline int max(int a, int b)
{
    if(a>b) return a;
    else return b;
}


void get_ip(char * str)
{
    struct ifaddrs *iflist, *iface;
	if(getifaddrs(&iflist) < 0) 
        error("getifaddrs");
	       	
	for(iface = iflist; iface; iface = iface->ifa_next)
	{
        int af = iface->ifa_addr->sa_family;
        const void *addr;
        char addrp[INET6_ADDRSTRLEN];
		switch (af) 
		{
        	case AF_INET:
        		addr = &((struct sockaddr_in *)iface->ifa_addr)->sin_addr;
           		break;
        	default:
        		addr = NULL;
        }

		if(addr)
		{
        	if (inet_ntop(af, addr, addrp, sizeof addrp) == NULL)
			{
				error("inet_ntop");
           		continue;
        	}

			if(strcmp(addrp,"0.0.0.0") &&  strcmp(addrp,"127.0.0.1"))
            	strcpy(str,addrp);
				
    	}
	}

	freeifaddrs(iflist);
}

void sendt(int fd, char * buf, int len, pthread_mutex_t * mtx, char * errmsg)
{
    int sent = 0;
    pthread_mutex_lock(mtx);
    while(len > 0)
    {
        sent = write(fd, buf, len);
        if(sent<0)
        {
            pthread_mutex_unlock(mtx);
            error(errmsg);
        }
        buf+=sent;
        len-=sent;
    }
    pthread_mutex_unlock(mtx);
}

void reciv(int fd, char * buf, int len,  char * errmsg)
{
    int red = 0;
    while(len > 0)
    {
        red = read(fd,buf,len);
        if(red<0)
        {
            error(errmsg);
        }
        buf+=red;
        len-=red;
    }
}

