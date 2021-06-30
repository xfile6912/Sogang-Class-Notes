/*
 * echoclient.c - An echo client
 */
/* $begin echoclientmain */
#include "csapp.h"

int main(int argc, char **argv) 
{
	int clientfd;
	char *host, *port, buf[MAXLINE];
    char buf2[MAXLINE];
	rio_t rio;
	int flag=0;//1인 경우 show, 2인 경우 exit
	if (argc != 3) {
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(0);
	}
	host = argv[1];
	port = argv[2];

	clientfd = Open_clientfd(host, port);
	Rio_readinitb(&rio, clientfd);

	while (Fgets(buf, MAXLINE, stdin) != NULL) {
        if(!strcmp("show\n", buf))
            flag=1;
        if(!strcmp("exit\n", buf))
            flag=2;
		Rio_writen(clientfd, buf, strlen(buf));
        strcpy(buf2, buf);
		Rio_readlineb(&rio, buf, MAXLINE);
        if(!strcmp(buf2, buf) && flag!=2)
        {
            printf("server closed\n");
            Close(clientfd);
            exit(0);
        }

        if(flag==1)//show인 경우
        {
            int i;
            for(i=0; i<strlen(buf); i++)
            {
                if(buf[i]=='\n')
                    buf[i]='\0';
                if(buf[i]=='\t')
                    buf[i]='\n';
            }
            flag=0;
        }
        if(flag==2)//exit인 경우
        {
            Close(clientfd);
            exit(0);
        }
		Fputs(buf, stdout);
	}
	Close(clientfd); //line:netp:echoclient:close
	exit(0);
}
/* $end echoclientmain */
