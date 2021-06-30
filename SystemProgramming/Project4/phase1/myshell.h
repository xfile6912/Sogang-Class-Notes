#include "csapp.h"
#include <errno.h>

#define MAXARGS 128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);
void Exec(char *filename, char *const argv[], char *const envp[]);
void normalize_cmdline(char * cmdline);
void init_sig();
void sig_int_handler(int sig);
void sig_stop_handler(int sig);
void sig_child_handler(int sig);
sigset_t mask_one, prev_one, mask_all;
int fg_end_flag=0;//foreground job이 끝났는지 안끝났는지 여부를 체크하는 flag; 1인 경우 끝남.
pid_t pid;           /* Process id */
void init_sig()
{
	Sigfillset(&mask_all);
	Sigemptyset(&mask_one);
	Sigaddset(&mask_one, SIGCHLD);
	Signal(SIGCHLD, sig_child_handler);
	Signal(SIGINT, sig_int_handler);
	Signal(SIGTSTP, sig_stop_handler);
	Signal(SIGTTOU, SIG_IGN);
	Signal(SIGTTIN, SIG_IGN);
}
void sig_int_handler(int sig)
{
	if(pid!=0)
	{
		kill(-pid, SIGINT);//foreground 프로세스 그룹에 sigint 보내줌.
	}
	return;
}
void sig_stop_handler(int sig)
{
	if(pid!=0)
	{
		kill(-pid, SIGINT);//foreground 프로세스 그룹에 SIGINT
	}
	return;
}

void sig_child_handler(int sig)
{
int olderrno=errno;
	sigset_t mask_all, prev_all;
	pid_t pid;
	int status;
	Sigfillset(&mask_all);
	//wnohang: 자식이 종료되지 않았떠라도 함수는 바로 리턴하게 끔 해줌
	//wuntraced: 종료된 프로세스 뿐만 아니라 멈춘 프로세스로부터도 상태정보 얻어옴.
	while((pid=waitpid(-1, &status, WNOHANG | WUNTRACED))>0)//종료된 프로세스와 중단된 프로세스의 상태를 받음
	{
		Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
		//child가 중단된 경우나 //child가 정상적으로 종료된 경우나 //child가 signal에 의해 종료된 경우
		if(WIFSTOPPED(status) || WIFEXITED(status) || WIFSIGNALED(status))
		{
			fg_end_flag=1;//parent 프로세스가 기다리는 것을 끝내도록 함
		}
		Sigprocmask(SIG_SETMASK, &prev_all, NULL);
	}
	errno=olderrno;
}
void Exec(char *filename, char *const argv[], char *const envp[])
{
	if (execve(filename, argv, environ) < 0)
	{
		///bin/에 없는 경우 user/bin에서 찾아봄
		strcpy(filename, "/usr/bin/");
		strcat(filename, argv[0]);
		if(execve(filename, argv, environ) < 0)
		{
			printf("%s: Command not found.\n", argv[0]);
			exit(0);
		}
	}

}
void normalize_cmdline(char * cmdline)
{
	int i;
	int length = strlen(cmdline);
	char buf[MAXLINE];
	int buf_idx=0;
	cmdline[strlen(cmdline) - 1] = ' '; /* Replace trailing '\n' with space */
	for (i = 0; i < length; i++)
	{
		if (cmdline[i] == '\t')
			cmdline[i] = ' '; //tab이 있으면 space로 바꾸어줌
	}

	for(i=0; i<length; i++)
	{
		if(cmdline[i]=='|' || cmdline[i]=='&')//pipe'|'나 bg'&'가 있는 경우 앞뒤에 space 넣어줌
		{
			buf[buf_idx++]=' ';
			buf[buf_idx++]=cmdline[i];
			buf[buf_idx++]=' ';
		}
		else
		{
			buf[buf_idx++]=cmdline[i];
		}
		buf[buf_idx]='\0';
	}
	strcpy(cmdline, buf);
}
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline)
{
	char *argv[MAXARGS]; /* Argument list execve() */
	char buf[MAXLINE];   /* Holds modified command line */
	int bg;              /* Should the job run in bg or fg? */

	normalize_cmdline(cmdline);//cmdline을 적절한 형식으로 변화시켜줌 ' '단위로 분리시켜줌
	//space단위로 나중에 parsing하기 때문에
	strcpy(buf, cmdline);
	bg = parseline(buf, argv);
	if (argv[0] == NULL)
		return; /* Ignore empty lines */
	if (!builtin_command(argv))
	{ //quit -> exit(0), & -> ignore, other -> run

		char argv0[MAXARGS] = "/bin/";
		strcat(argv0, argv[0]); //"/bin/ls" 식으로 만들어줌.
		Sigprocmask(SIG_BLOCK, &mask_one, &prev_one);
		if ((pid = Fork()) == 0)
		{
			setpgid(0, 0);
			Exec(argv0, argv, environ);
		}
		/* Parent waits for foreground job to terminate */
		if (!bg)
		{
			setpgid(pid, 0);
			tcsetpgrp(STDERR_FILENO, pid);
			int status;
			Sigprocmask(SIG_BLOCK, &mask_all, NULL);
			while(!fg_end_flag)//foreground job을 명시적으로 기다리기 위해서
				Sigsuspend(&prev_one);
			fg_end_flag=0;
			Sigprocmask(SIG_SETMASK, &prev_one, NULL);//unblock sigchild
			tcsetpgrp(STDERR_FILENO, getpgrp());
		}
		else //when there is backgrount process!
			printf("%d %s", pid, cmdline);
	}
	return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv)
{
	if (!strcmp(argv[0], "exit"))
		exit(0);
	if (!strcmp(argv[0], "quit")) /* quit command */
		exit(0);
	if (!strcmp(argv[0], "&")) /* Ignore singleton & */
		return 1;
	if (!strcmp(argv[0], "cd"))
	{
		if (argv[1] == NULL)
			chdir(getenv("HOME"));
		else if (argv[2] != NULL)
		{
			fprintf(stderr, "usage: cd [directory name] ...\n");
		}
		else if ((chdir(argv[1]) < 0))
			fprintf(stderr, "%s: %s: No such file or directory\n", argv[0], argv[1]);

		return 1;
	}

	return 0; /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
int parseline(char *buf, char **argv)
{
	char *delim; /* Points to first space delimiter */
	int argc;    /* Number of args */
	int bg;      /* Background job? */

	while (*buf && (*buf == ' ')) /* Ignore leading spaces */
		buf++;

	/* Build the argv list */
	argc = 0;
	while ((delim = strchr(buf, ' ')))
	{
		argv[argc++] = buf;

		char *temp=buf;
		char *start=buf;//따옴표의 시작지점
		char *end=buf;//따옴표의 끝지점
		int flag=0;//따옴표에 대한 처리가 필요한지 체크하는 flag;
		//따옴표에 대한 처리.
		while(*temp && (*temp!=' '))//buf 부터 space가 아닌 영역를 탐색
		{
			if(*temp=='\'')//여는 작은 따옴표가 안에 있는 경우
			{
				start=temp;
				temp++;
				end=strchr(temp, '\'');//end은 닫는 작은 따옴표를 가리킴
				temp=end;//temp는 닫는 작은 따옴표를 가리킴
				delim=strchr(temp, ' ');//delim은 닫는 작은 따옴표 이후의 ' '를 가리킴
				flag=1;
				break;
			}
			if(*temp=='\"')//여는 큰 따옴표가 안에 있는 경우
			{
				start=temp;
				temp++;
				end=strchr(temp, '\"');//end은 닫는 큰 따옴표를 가리킴
				temp=end;//temp는 닫는 큰 따옴표를 가리킴
				delim=strchr(temp, ' ');//delim은 닫는 큰 따옴표 이후의 ' '를 가리킴
				flag=1;
				break;
			}
			temp++;
		}
		*delim = '\0';

		if(flag==1)
		{
			temp=buf;
			while(temp<=delim)
			{
				if(buf>delim)
				{
					*temp=' ';
					temp++;
				}
				else if(buf!=start && buf !=end)//buf가 따옴표(열고, 닫는)가 아니라면
				{
					*temp=*buf;
					temp++;
				}
				buf++;
			}
		}


		buf = delim + 1;
		while (*buf && (*buf == ' ')) /* Ignore spaces */
			buf++;
	}
	argv[argc] = NULL;

	if (argc == 0) /* Ignore blank line */
		return 1;

	/* Should the job run in the background? */
	if ((bg = (*argv[argc - 1] == '&')) != 0)
		argv[--argc] = NULL;

	return bg;
}
/* $end parseline */

