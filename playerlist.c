
#include "main.h"
//#include "playerlist.h"
void PLAYERListInit(void)
{
	pthread_mutex_init(&PLAYERListMUTEX,NULL);
}


/* inserta un item al comienzo de la lista */
void PLAYERListPush(PLAYERList_t **head, Player_t player, int fd)
{
	pthread_mutex_lock(&PLAYERListMUTEX);

	PLAYERList_t * new;
	new = malloc(sizeof(PLAYERList_t));
	new->node.player = player;
	new->node.fd = fd;
	pthread_mutex_init(&(new->node.pmtx),NULL);
	if(*head == NULL)
	{
		*head = new;
	}
	else
	{
		new->next = *head;
		*head= new;
	}
	
	pthread_mutex_unlock(&PLAYERListMUTEX);
}

/* remueve el primer item de la lista */
PLAYERListNode_t PLAYERListPop(PLAYERList_t **head)
{
	pthread_mutex_lock(&PLAYERListMUTEX);

	PLAYERListNode_t ret;
	ret.fd = -1;
    PLAYERList_t * next_node = NULL;
    if (*head == NULL) {
		pthread_mutex_unlock(&PLAYERListMUTEX);
        return ret;
    }
    next_node = (*head)->next;
    ret = (*head)->node;
	free(*head);
    *head = next_node;

	pthread_mutex_unlock(&PLAYERListMUTEX);
    return ret;
}


void PLAYERListPrint(PLAYERList_t * head, char * path) 
{
	pthread_mutex_lock(&PLAYERListMUTEX);

    PLAYERList_t * current = head;

    char buf[64];
 
	FILE * fd;
	fd = fopen(path,"a+");

    if(fd<0) exit(1);

    while (current != NULL) {
        fprintf(fd,"fd = %d\n\tname = %s\n\tID = %d\n\tx=%d  y=%d\n",
											 current->node.fd,
                                             current->node.player.name,
                                             current->node.player.playerID,
											 current->node.player.x,
											 current->node.player.y);

        current = current->next;
    }

	fclose(fd);

	pthread_mutex_unlock(&PLAYERListMUTEX);

}


int PLAYERListFindByfd(PLAYERListNode_t * head, int fd)
{
	error("PLAYERListFindByfd NOT IMPLEMENTED\n");

	// PLAYERList_t * current = head;
	// int idx = 0;

	// while(current != NULL)
	// {
	// 	if (current->node.player.playerID == ID)
	// 		return idx;
	// 	idx++;
	// 	current = current->next;
	// }
	// return -1;
    return 1;
}


PLAYERListNode_t PLAYERListGetByidx(PLAYERList_t * head, int idx)
{
	pthread_mutex_lock(&PLAYERListMUTEX);
	PLAYERListNode_t nox;
    nox.fd = -1;
	PLAYERList_t * current = head;
	int i = 0;
	for(i=0; i<idx; i++)
	{	
		if(current->next == NULL)
		{
			pthread_mutex_unlock(&PLAYERListMUTEX);
			return nox;
		}
			
		current = current->next;
	}

	pthread_mutex_unlock(&PLAYERListMUTEX);
	return current->node;
}




int PLAYERListSize(PLAYERList_t * head)
{
	pthread_mutex_lock(&PLAYERListMUTEX);
	int i = 0;
	PLAYERList_t * current = head;

	if(head == NULL)
	{
		pthread_mutex_unlock(&PLAYERListMUTEX);
		return 0;
	}
		

	while(1)
	{
		i++;
		if(current->next == NULL)
		{
			pthread_mutex_unlock(&PLAYERListMUTEX);
			return i;
		}
			

		current = current->next;
	}
pthread_mutex_unlock(&PLAYERListMUTEX);
}


/* devuelve el max playerID de todos los players de la lista*/
int PLAYERListMaxID(PLAYERList_t * head);


int PLAYERListRemoveByfd(PLAYERList_t **head, int fd)
{
	pthread_mutex_lock(&PLAYERListMUTEX);
	PLAYERList_t * temp = *head;
	PLAYERList_t * prev = NULL;
	
	if(temp != NULL && temp->node.fd == fd)
	{
		*head = temp->next;
		free(temp);
		pthread_mutex_unlock(&PLAYERListMUTEX);
		return 0;
	}

	while(temp != NULL && temp->node.fd != fd)
	{
		prev = temp;
		temp = temp->next;
	}
	if(temp == NULL)
	{
		pthread_mutex_unlock(&PLAYERListMUTEX);
		return -1;
	}
		
	prev->next = temp->next;
	free(temp);
	pthread_mutex_unlock(&PLAYERListMUTEX);
	return 0;
}


int PLAYERListRemoveByID(PLAYERList_t **head, int id)
{
	pthread_mutex_lock(&PLAYERListMUTEX);
	PLAYERList_t * temp = *head;
	PLAYERList_t * prev = NULL;
	
	if(temp != NULL && temp->node.player.playerID == id)
	{
		*head = temp->next;
		free(temp);
		pthread_mutex_unlock(&PLAYERListMUTEX);
		return 0;
	}

	while(temp != NULL && temp->node.player.playerID != id)
	{
		prev = temp;
		temp = temp->next;
	}
	if(temp == NULL)
	{
		pthread_mutex_unlock(&PLAYERListMUTEX);
		return -1;
	}
		
	prev->next = temp->next;
	free(temp);
	pthread_mutex_unlock(&PLAYERListMUTEX);
	return 0;
}



PLAYERListNode_t PLAYERListGetByfd(PLAYERList_t * head, int fd)
{
	pthread_mutex_lock(&PLAYERListMUTEX);
	PLAYERList_t * current = head;
	int idx = 0;
	PLAYERListNode_t ret_node;
	ret_node.fd = -1;
	while(current != NULL)
	{
		if (current->node.fd == fd)
		{
			pthread_mutex_unlock(&PLAYERListMUTEX);
			return current->node;
		}
		current = current->next;
	}
	pthread_mutex_unlock(&PLAYERListMUTEX);
	return ret_node;
}


PLAYERListNode_t PLAYERListGetByID(PLAYERList_t * head, int ID)
{
	pthread_mutex_lock(&PLAYERListMUTEX);
	PLAYERList_t * current = head;
	int idx = 0;
	PLAYERListNode_t ret_node;
	ret_node.fd = -1;
	while(current != NULL)
	{
		if (current->node.player.playerID == ID)
			{pthread_mutex_unlock(&PLAYERListMUTEX);
			return current->node;}
		current = current->next;
	}
	pthread_mutex_unlock(&PLAYERListMUTEX);
	return ret_node;
}

int PLAYERListUpdateByfd(PLAYERList_t **head, Player_t player, int fd)
{
	pthread_mutex_lock(&PLAYERListMUTEX);
	PLAYERList_t * temp = *head;
	int idx = 0;
	int ret = -1;

	while((*head) != NULL)
	{
		if ((*head)->node.fd == fd)
		{
			(*head)->node.player = player;
			ret = 0;
			break;
		}
		*head = (*head)->next;
	}
	*head = temp;
	
	pthread_mutex_unlock(&PLAYERListMUTEX);
	return ret;
}


void PLAYERListAppend(PLAYERList_t **head, Player_t player, int fd)
{
	pthread_mutex_lock(&PLAYERListMUTEX);
	PLAYERListNode_t new_node = {player, fd};
	PLAYERList_t * current = *head;
	PLAYERList_t * new = malloc(sizeof(PLAYERList_t));
	new->node = new_node;
	new->next = NULL;

    while ((*head)->next != NULL) 
        (*head) = (*head)->next;
    
    (*head)->next = new;

	*head = current;
	pthread_mutex_unlock(&PLAYERListMUTEX);
}


int PLAYERListUpdateByID(PLAYERList_t **head, Player_t player, char id)
{
	pthread_mutex_lock(&PLAYERListMUTEX);
	PLAYERList_t * temp = *head;
	int idx = 0;
	int ret = -1;

	while((*head) != NULL)
	{
		if ((*head)->node.player.playerID == id)
		{
			(*head)->node.player = player;
			ret = 0;
			break;
		}
		*head = (*head)->next;
	}
	*head = temp;
	pthread_mutex_unlock(&PLAYERListMUTEX);
	return ret;
}


void PLAYERListDestroy(PLAYERList_t ** head)
{
	if(*head != NULL)
	{
		pthread_mutex_destroy(&PLAYERListMUTEX);

		PLAYERList_t * next;
		PLAYERList_t * current = *head;

		while(current != NULL)
		{
			next = current->next;
			pthread_mutex_destroy(&(current->node.pmtx));
			free(current);
			current = next;
		}
		*head = NULL;
	}
}

