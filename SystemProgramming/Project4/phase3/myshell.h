#include "csapp.h"
#include <errno.h>

#define MAXARGS 128
sigset_t mask_all, mask_one, prev_one;

typedef struct job_entry *job_link;
typedef struct job_entry{
	pid_t pid;//job pid
	int job_id;//job id
	char cmdline[MAXLINE];
	int state;//job의 상태를 저장, -1:아무것도 할당 되지 않은 job, 0:BACKGROUND, 1:FOREGROUND, 2:stopped, 3:done, 4:terminated
	int job_count;
	job_link link;
}Job_Entry;

job_link jobs_head=NULL;
job_link jobs_tail=NULL;
int job_count=0;


int fg_end_flag=0;//foreground job이 끝났는지 안끝났는지 여부를 체크하는 flag; 1인 경우 끝남.
int pipe_end_flag=0;//pipe에서 현재 수행하는 명령이 끝났는지 안끝났는지 여부를 체크하는 flag;
/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);
int pipe_execution(char **p_command, int p_command_cnt);//execution command line which has pipe
void Exec(char *filename, char *const argv[], char *const envp[]);
void normalize_cmdline(char * cmdline);
void init_sig();
void sig_int_handler(int sig);
void sig_stop_handler(int sig);
void sig_child_handler(int sig);
void init_jobs();
int get_next_job_id();
Job_Entry* add_job(pid_t pid, char** argv, int state);
int get_job_id(pid_t pid);
Job_Entry* get_job(pid_t pid);
void delete_job(int job_id);
void print_jobs();
void kill_command(char **argv);
void bg_command(char **argv);
void fg_command(char **argv);
int check_plus_minus(int job_count);
void print_reaped_child();
int check_plus_minus(int job_count)//+인지 -인지 체크 0인경우 +, 1인경우 -, 2인경우 ' '
{
	Job_Entry* ptr;
	int count=0;//job_count보다 더 높은 것의 개수 체크
	for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
	{
		if(job_count<ptr->job_count)
			count++;
	}

	if(count>=2)
		count=2;
	return count;
}
void print_reaped_child()//background프로세스중 done이나 terminated 된것들을 순서대로 출력
{
	Job_Entry* ptr;
	for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
	{
		if(ptr->state==3)//DONE
		{
			Sio_puts("[");
			Sio_putl((long)(ptr->job_id));
			Sio_puts("] ");
			Sio_puts(" DONE	");
			Sio_putl((long)(ptr->pid));
			Sio_puts(" ");
			Sio_puts(ptr->cmdline);
			Sio_puts("\n");
		}
		else if(ptr->state==4)//TERMINATED
		{
			Sio_puts("[");
			Sio_putl((long)(ptr->job_id));
			Sio_puts("]");
			Sio_puts(" TERMINATED	");
			Sio_putl((long)(ptr->pid));
			Sio_puts(" ");
			Sio_puts(ptr->cmdline);
			Sio_puts("\n");
		}
			//printf("[%d] RUNNING %d %s\n", ptr->job_id, ptr->pid, ptr->cmdline);
	}
	for(ptr=jobs_head; ptr!=NULL; )
	{
		Job_Entry *next_ptr=ptr->link;
		if(ptr->state==3 || ptr->state==4)//done이나 terminated면 job list에서 삭제
		{
			delete_job(ptr->job_id);
		}
		ptr=next_ptr;
	}
}


int get_next_job_id()//job_id로 부여될 job number구해줌
{
	Job_Entry *ptr;
	int job_number=1;
	int i;

	for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)//add 되는 job의 job_id로 부여될 job number구해줌
	{
		if(ptr->job_id>=job_number)
		{
			job_number=ptr->job_id+1;
		}
	}
	return job_number;
}
//job list에 job 추가하고 job id 반환
Job_Entry* add_job(pid_t pid, char** argv, int state)
{
	Job_Entry * new_job=(Job_Entry*)malloc(sizeof(Job_Entry));
	
	char cmdline[MAXLINE]="";
	int i;
	for(i=0; argv[i]!=NULL; i++)//parsing된 argv를 이용해 cmdline 만들어냄.
	{
		strcat(cmdline, argv[i]);
		strcat(cmdline, " ");
	}

	int job_number=get_next_job_id();

	new_job->pid=pid;
	if(state==0)//background process인경우
	{
		new_job->job_id=job_number;
		new_job->job_count=job_count++;
	}
	else if(state==1)//foreground process인 경우
	{
		new_job->job_id=-1;//일단은 job id를 부여하지 않음
	}
	new_job->state=state;
	strcpy(new_job->cmdline, cmdline);
	new_job->link=NULL;
	if(jobs_head==NULL)//job list에 아무것도 없으면 head와 tail이 newjob을 가리킴
	{
		jobs_head=new_job;
		jobs_tail=new_job;
	}
	else//job list에 job이 하나라도 있는 경우
	{
		jobs_tail->link=new_job;
		jobs_tail=new_job;
	}
	return new_job;
}
//pid를 받아서 jobid 리턴 없으면 0 리턴
int get_job_id(pid_t pid)
{
	Job_Entry* ptr;
	for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
	{
		if(ptr->pid==pid)//jobs list에서 pid를 찾은 경우
		{
			return ptr->job_id;
		}
	}
	return 0;
}
//pid를 받아서 job 리턴 없으면 NULL 리턴
Job_Entry* get_job(pid_t pid)
{
	Job_Entry* ptr;
	for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
	{
		if(ptr->pid==pid)//jobs list에서 pid를 찾은 경우
		{
			return ptr;
		}
	}
	return NULL;
}
//job list에서 job_id에 해당하는 job를 삭제
void delete_job(int job_id)
{
	Job_Entry* ptr;
	Job_Entry* prev_ptr=jobs_head;
	for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
	{
		if(ptr->job_id==job_id)//jobs list에서 job id를 찾은 경우
		{
			break;
		}
		prev_ptr=ptr;
	}
	if(ptr==NULL)//joblist에서 job을 찾지 못한 경우
	{
		Sio_puts("cannot find job id in jobs\n");
	}
	else//job list에서 job을 찾은 경우
	{
		if(ptr==jobs_head && ptr==jobs_tail)//하나밖에 없었던 경우에 이제 job list는 비어있게 된다
		{
			jobs_head=NULL;
			jobs_tail=NULL;
			free(ptr);
		}
		else if(ptr==jobs_head)//ptr이 job의 head인 경우
		{
			jobs_head=ptr->link;//job list의 head가 ptr의 다음 node를 가리키도록 함
			free(ptr);
		}
		else if(ptr==jobs_tail)//ptr이 job의 tail인 경우
		{
			jobs_tail=prev_ptr;//job list의 tail이 ptr의 이전 node를 가리키도록함;
			jobs_tail->link=NULL;
			free(ptr);
		}
		else//ptr이 job의 중간인 경우
		{
			prev_ptr->link=ptr->link;//ptr의 이전 node가 ptr의 다음 node를 가리키도록함
			free(ptr);
		}	
	}
}
//jobs 명령시 job list에 있는 job들을 출력해줌.
void print_jobs()
{
	Job_Entry* ptr;
	for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
	{
		if(ptr->state==2)
		{
			Sio_puts("[");
			Sio_putl((long)(ptr->job_id));
			Sio_puts("]");
			Sio_puts(" STOPPED	");
			Sio_putl((long)(ptr->pid));
			Sio_puts(" ");
			Sio_puts(ptr->cmdline);
			Sio_puts("\n");
		}
			//printf("[%d] STOPPED %d %s\n", ptr->job_id, ptr->pid, ptr->cmdline);
		else if(ptr->state==0)//background인 경우 뒤에 &붙여줌
		{
			Sio_puts("[");
			Sio_putl((long)(ptr->job_id));
			Sio_puts("]");
			Sio_puts(" RUNNING	");
			Sio_putl((long)(ptr->pid));
			Sio_puts(" ");
			Sio_puts(ptr->cmdline);
			Sio_puts("&\n");
		}
			//printf("[%d] RUNNING %d %s&\n", ptr->job_id, ptr->pid, ptr->cmdline);
		else if(ptr->state==1)
		{
			Sio_puts("[");
			Sio_putl((long)(ptr->job_id));
			Sio_puts("]");
			Sio_puts(" RUNNING	");
			Sio_putl((long)(ptr->pid));
			Sio_puts(" ");
			Sio_puts(ptr->cmdline);
			Sio_puts("\n");
		}
		else if(ptr->state==3)//DONE
		{
			Sio_puts("[");
			Sio_putl((long)(ptr->job_id));
			Sio_puts("]");
			Sio_puts(" DONE	");
			Sio_putl((long)(ptr->pid));
			Sio_puts(" ");
			Sio_puts(ptr->cmdline);
			Sio_puts("\n");
		}
		else if(ptr->state==4)//TERMINATED
		{
			Sio_puts("[");
			Sio_putl((long)(ptr->job_id));
			Sio_puts("]");
			Sio_puts(" TERMINATED	");
			Sio_putl((long)(ptr->pid));
			Sio_puts(" ");
			Sio_puts(ptr->cmdline);
			Sio_puts("\n");
		}
			//printf("[%d] RUNNING %d %s\n", ptr->job_id, ptr->pid, ptr->cmdline);
	}
	for(ptr=jobs_head; ptr!=NULL; )
	{
		Job_Entry *next_ptr=ptr->link;
		if(ptr->state==3 || ptr->state==4)//done이나 terminated면 job list에서 삭제
		{
			delete_job(ptr->job_id);
		}
		ptr=next_ptr;
	}
}
//kill command시 kill해줌
void kill_command(char **argv)
{
	pid_t pid=0;
	Job_Entry *ptr;
	
	char *id=argv[1];
	
	if(id[0]=='%')//job 번호인 경우
	{
		int job_id=atoi(id+1);
		for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
		{
			if(ptr->job_id==job_id)//job_id가 일치하는 job을 job list에서 찾아냄
			{
				pid=ptr->pid;
				break;
			}
		}
	}
	else//pid 인 경우
	{
		pid_t temp_pid=atoi(id);
		for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
		{
			if(ptr->pid==temp_pid)//pid가 일치하는 job의 pid를 job list에서 찾아냄
			{
				pid=ptr->pid;
				break;
			}
		}
	}

	if(pid!=0)
	{
		if(ptr->state==2)//stopped이면 stopped상태인 거 출력해주고 이후에 termintate 시킴
		{
				Sio_puts("[");
				Sio_putl((long)(ptr->job_id));
				Sio_puts("]");
				Sio_puts(" STOPPED	");
				Sio_putl((long)(ptr->pid));
				Sio_puts(" ");
				Sio_puts(ptr->cmdline);
				Sio_puts("\n");
		}
		kill(-pid, SIGCONT);//진행시킨 후
		kill(-pid, SIGINT);//foreground 프로세스 그룹에 sigint 보내줌.
	}
	else
	{
		Sio_puts("Cannot find the proccess id in job lists\n");
	}
	return;
}
//bg command가 들어오면 background에서 돌려줌
void bg_command(char **argv)
{
	pid_t pid=0;
	Job_Entry* ptr;
	Job_Entry *last_stopped;//stop된것중 list의 가장 뒤쪽에 있는 것
	last_stopped=NULL;
	if(argv[1]==NULL)//인자가 들어오지 않은 경우(jobid가 없는 경우)
	{
		for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
		{
			if(ptr->state==2)//stop인 job을 job list에서 찾아냄
			{
				last_stopped=ptr;
			}
		}
		if(last_stopped!=NULL)
		{
			Sio_puts("[");
			Sio_putl((long)(last_stopped->job_id));
			Sio_puts("] ");
			Sio_putl((long)(last_stopped->pid));
			Sio_puts(" ");
			Sio_puts(last_stopped->cmdline);
			Sio_puts("&\n");
			last_stopped->state=0;//bg running으로 바꾸어줌 상태를.
			kill(-(last_stopped->pid), SIGCONT);//BACKGROUND의 해당 process 프로세스 그룹에 SIGCONT 보내줌.
		}
		else
		{
			Sio_puts("Cannot run in background(no such jobs or already running in background)\n");
		}
	}
	else//인자가 들어온 경우(jobid가 있는 경우)
	{
		int job_id;
		if(argv[1][0]=='%')
			job_id=atoi(&argv[1][1]);
		else
			job_id=atoi(argv[1]);

		for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
		{
			if(ptr->job_id==job_id && ptr->state==2)//job_id가 일치하고 stop인 job을 job list에서 찾아냄
			{
				pid=ptr->pid;
				break;
			}
		}
		if(pid!=0)
		{
			Sio_puts("[");
			Sio_putl((long)(ptr->job_id));
			Sio_puts("] ");
			Sio_putl((long)(ptr->pid));
			Sio_puts(" ");
			Sio_puts(ptr->cmdline);
			Sio_puts("&\n");
			ptr->state=0;//bg running으로 바꾸어줌 상태를.
			kill(-pid, SIGCONT);//BACKGROUND의 해당 process 프로세스 그룹에 SIGCONT 보내줌.
		}
		else
		{
			Sio_puts("Cannot run in background(no such jobs or already running in background)\n");
		}
	}

}
//fg_command가 들어온경우
void fg_command(char **argv)
{
	pid_t pid=0;
	sigset_t prev_all;
	Sigemptyset(&prev_all);
	Job_Entry* ptr;
	Job_Entry *last_stopped;//stop된것중 list의 가장 뒤쪽에 있는 것
	Job_Entry *last_background;//bacground중 list의 가장 뒤쪽에 있는 것
	last_stopped=NULL;
	last_background=NULL;
	if(argv[1]==NULL)//인자가 들어오지 않은 경우(jobid가 들어오지 않은경우)
	{
		for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
		{
			if(ptr->state==2)//stop인 job을 job list에서 찾아냄
			{
				last_stopped=ptr;
			}
			if(ptr->state==0)
			{
				last_background=ptr;
			}
		}
		if(last_stopped!=NULL)
		{

			last_stopped->state=1;//fg running으로 바꾸어줌 상태를.
			Sio_puts(last_stopped->cmdline);
			Sio_puts("\n");
			kill(-(last_stopped->pid), SIGCONT);//BACKGROUND의 해당 process 프로세스 그룹에 SIGCONT 보내줌.
			tcsetpgrp(0, last_stopped->pid);
			while(!fg_end_flag)
				Sigsuspend(&prev_all);
			fg_end_flag=0;

			Signal(SIGTTOU, SIG_IGN);
			tcsetpgrp(0, getpid());
			Signal(SIGTTOU, SIG_DFL);
		}
		else if(last_background!=NULL)
		{

			last_background->state=1;//fg running으로 바꾸어줌 상태를.
			Sio_puts(last_background->cmdline);
			Sio_puts("\n");
			kill(-(last_background->pid), SIGCONT);//BACKGROUND의 해당 process 프로세스 그룹에 SIGCONT 보내줌
			tcsetpgrp(0, last_background->pid);
			while(!fg_end_flag)
				Sigsuspend(&prev_all);
			fg_end_flag=0;
			Signal(SIGTTOU, SIG_IGN);
			tcsetpgrp(0, getpid());
			Signal(SIGTTOU, SIG_DFL);
		}
		else
		{
			Sio_puts("No Such jobs for fg.\n");
		}
	}
	else//job id가 들어온 경우
	{
		int job_id;
		if(argv[1][0]=='%')
			job_id=atoi(&argv[1][1]);
		else
			job_id=atoi(argv[1]);
		for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
		{
			if(ptr->job_id==job_id)//job_id가 일치하는 job을 job list에서 찾아냄
			{
				pid=ptr->pid;
				break;
			}
		}
		if(pid!=0)//job list에서 job_id를 찾은 경우
		{
			ptr->state=1;//fg running으로 바꾸어줌 상태를.
			Sio_puts(ptr->cmdline);
			Sio_puts("\n");
			kill(-pid, SIGCONT);//BACKGROUND의 해당 process 프로세스 그룹에 SIGCONT 보내줌.

			tcsetpgrp(0, ptr->pid);

			while(!fg_end_flag)
				Sigsuspend(&prev_all);
			fg_end_flag=0;
			Signal(SIGTTOU, SIG_IGN);
			tcsetpgrp(0, getpid());
			Signal(SIGTTOU, SIG_DFL);
		}
		else//찾지 못한경우
		{
			Sio_puts("Cannot find the job in job lists\n");
		}
	}
}
void init_sig()
{
	Sigfillset(&mask_all);
	Sigemptyset(&mask_one);
	Sigaddset(&mask_one, SIGCHLD);
	Signal(SIGCHLD, sig_child_handler);
	Signal(SIGINT, sig_int_handler);
	Signal(SIGTSTP, sig_stop_handler);
	Signal(SIGTTIN, SIG_IGN);
	pid_t pid = getpid();
	setpgid(pid, pid);
	tcsetpgrp(0, pid);
}
void sig_int_handler(int sig)
{
	pid_t pid=0;
	Job_Entry *ptr;
	for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
	{
		if(ptr->state==1)//foreground job찾음
		{
			pid=ptr->pid;
			break;
		}
	}
	//printf("%d", pid);
	if(pid!=0)
	{
		kill(-pid, SIGINT);//foreground 프로세스 그룹에 sigint 보내줌.
		tcsetpgrp(STDERR_FILENO, getpgrp());
	}
	return;
}
void sig_stop_handler(int sig)
{
	pid_t pid=0;
	Job_Entry *ptr;
	for(ptr=jobs_head; ptr!=NULL; ptr=ptr->link)
	{
		//		printf("state: %d\n", ptr->state);
		if(ptr->state==1)//foreground job찾음
		{
			pid=ptr->pid;
			break;
		}
	}
	//printf("%d\n", pid);
	if(pid!=0)
	{
		kill(-pid, SIGTSTP);//foreground 프로세스 그룹에 SIGTSTP 보내줌.
		tcsetpgrp(STDERR_FILENO, getpgrp());
	}
	return;
}
void sig_child_handler(int sig)
{
	int olderrno=errno;
	pid_t pid;
	int status;
	while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)//종료된 프로세스와 중단된 프로세스의 상태를 받음
	{
		//printf("%d\n", pid);
		if (WIFSTOPPED(status))//child가 중단된 경우
		{
			//printf("%d\n", 1);
			Job_Entry* job = get_job(pid);
			
			if (job->state == 1)//foreground인 경우
			{
				if (job->job_id == -1)//job_id가 처음에 add되지 않고foreground로 지정된 잡이 stop된경우
				{
					//job의 job_id를 새로 할당해줌.
					job->job_id = get_next_job_id();
				}
				job->job_count=job_count++;
				Sio_puts("[");
				Sio_putl((long)(job->job_id));
				Sio_puts("] STOPPED	");
				Sio_putl((long)(job->pid));
				Sio_puts(" ");
				Sio_puts(job->cmdline);
				Sio_puts("\n");
				job->state = 2;//stopped로 상태 바꾸어줌
				fg_end_flag = 1;//parent 프로세스가 기다리는 것을 끝내도록 함
			}
			else//background인 경우
			{
				job->state = 2;//stopped로 상태 바꾸어줌
			}

		}
		else if (WIFEXITED(status))//child가 정상적으로 종료된 경우
		{
			//printf("%d\n", 2);
			Job_Entry* job = get_job(pid);
			if (job->state == 1)//foreground
			{
				fg_end_flag = 1;//parent 프로세스가 기다리는 것을 끝내도록 함.
				delete_job(job->job_id);//foreground프로세스의 경우 나중에 출력이 필요없기 때문에 바로 delete해줌
			}
			else//background
			{
				job->state=3;//backgorund의 경우 나중에 done이라는 출력이 필요하므로 jobstate를 3으로 바꾸저줌
			}
		}
		else if (WIFSIGNALED(status))//child가 signal에 의해 종료된 경우
		{
			Job_Entry* job = get_job(pid);


			if (job->state == 1)//foreground job인 경우에는
			{
				fg_end_flag = 1;
				delete_job(job->job_id);//foreground 프로세스의 경우 나중에 출력이 필요없기 때문에 바로 delete해줌
			}
			else
			{
				job->state=4;//background의 경우 나중에 terminate라는 출력이 필요하므로 job state를 4로 바꾸어줌
			}
		}
	}
	errno=olderrno;
}


void sig_child_in_pipe_handler(int sig)
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
		//child가 정상적으로 종료된 경우나 //child가 signal에 의해 종료된 경우
		if(WIFEXITED(status) || WIFSIGNALED(status))
		{
			pipe_end_flag=1;//parent 프로세스가 기다리는 것을 끝내도록 함
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
			Sio_puts(argv[0]);
			Sio_puts(": Command not found.\n");
			exit(0);
		}
	}

}

int pipe_execution(char **p_command, int p_command_cnt)
{
	int i;
	char *argv[MAXARGS];
	char buf[MAXLINE];
	int pipefd[MAXARGS][2];
	pid_t pid;
	int status;
	sigset_t prev_all;
	Sigemptyset(&prev_all);
	Signal(SIGCHLD, sig_child_in_pipe_handler);
	Signal(SIGTSTP, SIG_DFL);//CHILD PROCESS에서 sigtstp과 sigint는 defalut로 진행되게함.
	Signal(SIGINT, SIG_DFL);
	for(i=0; i<p_command_cnt; i++)//pipe 단위로 쪼개지는 각각의 명령 수행
	{
		char argv0[MAXLINE]="/bin/";
		int temp;
		strcpy(buf, p_command[i]);
		temp=parseline(buf, argv);//각 명령을 argv로 parsing
		strcat(argv0, argv[0]);//각 명령의 argv[0]을 "/bin/ls" 형태로 바꾸어줌

		pipe(pipefd[i]);//pipe 생성

		/*int j;
		  for(j=0; argv[j]!=NULL; j++)
		  printf("%d %s\n", j, argv[j]);*/

		if((pid=Fork())==0)
		{
			setpgid(0, getppid());
			if(i==0)//가장 앞에 있는 명령의 경우
			{
				close(pipefd[i][0]);///////나중에 들어감

				//STDOUT_FILENO를 닫고 pipefd[i][1]을 stdout으로 지정
				dup2(pipefd[i][1], STDOUT_FILENO);//stdout의 출력 pipe 지정
				close(pipefd[i][1]);
				Exec(argv0, argv, environ);
			}
			if(i==p_command_cnt-1)//가장 뒤의 명령어의 경우
			{
				close(pipefd[i-1][1]);//////나중에 들어감
				dup2(pipefd[i-1][0], STDIN_FILENO);
				close(pipefd[i-1][0]);

				Exec(argv0, argv, environ);
			}
			else//나머지 의 경우
			{
				dup2(pipefd[i-1][0], STDIN_FILENO);//입력 pipe 지정
				dup2(pipefd[i][1], STDOUT_FILENO);//출력 pipe 지정
				close(pipefd[i-1][0]);
				close(pipefd[i][1]);
				Exec(argv0, argv, environ);
			}
		}
		close(pipefd[i][1]);//나중에 들어감pipefd[i][0]은 닫아주면 안됨(부모가 다음 자식에게 전달해줘야하므로)
		while(!pipe_end_flag)//pipe 하나의 명령이 끝날때까지 명시적으로 기다리기 위해서
			Sigsuspend(&prev_all);
		pipe_end_flag=0;
	}
	exit(0);
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
void eval(char *cmdline)
{

	char *argv[MAXARGS]; /* Argument list execve() */
	char buf[MAXLINE];   /* Holds modified command line(for argv) */
	char buf2[MAXLINE];  /* Holds modified command line (for piped command)*/
	int bg;              /* Should the job run in bg or fg? */
	pid_t pid;           /* Process id */
	int pipe_flag=0;//pipe가 있는지 없는지 저장
	int i;
	Job_Entry* job;
	sigset_t prev_all;
	Sigemptyset(&prev_all);
	normalize_cmdline(cmdline);//cmdline을 적절한 형식으로 변화시켜줌 ' '단위로 분리시켜줌
	//space단위로 나중에 parsing하기 때문에

	strcpy(buf, cmdline);//buf에 commandline 복사
	bg = parseline(buf, argv);//parsing 수행
	   
	if (argv[0] == NULL)
		return; /* Ignore empty lines */
	if (!builtin_command(argv))
	{ //quit -> exit(0), & -> ignore, other -> run

		char argv0[MAXLINE] = "/bin/";
		strcat(argv0, argv[0]); //"/bin/ls" 식으로 만들어줌.
		char *p_command[MAXARGS];//pipe단위로 쪼개지는 각각의 command 저장
		int p_command_cnt=0;//pipe단위로 쪼개지는 command의 개수 저장
		int p_start=0;//pipe 단위로 쪼개지는 각각의 command의 시작점을 저장하는 변수
		strcpy(buf2, cmdline);//임시 공간 buf2에 cmdline 저장
		int i;
		//addjob을 하기전에 SIGCHLD가 호출되면 delete가 먼저 수행될 수 있으므로 sigchild를 막아놈

		if (!bg)
		{
			job = add_job(-1, argv, 1);//ADD the foreground Child to the job list
		}
		else
		{
			job = add_job(-1, argv, 0);//ADD the background Child to the job list
		}

		if((pid=Fork())==0)
		{
			Signal(SIGTTOU, SIG_DFL);
			Signal(SIGTTIN, SIG_DFL);
			if (job->pid > 0)
				setpgid(0, job->pid);
			else
			{
				job->pid = getpid();
				setpgid(0, job->pid);
			}
			//pipe 있는지 체크;
			for(i=0; i<strlen(cmdline); i++)//strlen(buf2)를 쓰게되면 중간에 strlen이 변하게 됨('\0'를 넣어주게 되므로)
			{
				if(buf2[i]=='|')//pipe가 있으면
				{
					pipe_flag=1;
					p_command[p_command_cnt++]= &buf2[p_start];
					buf2[i]='\0';
					p_start=i+1;
				}
			}
			if(pipe_flag==1)//pipe가 있는 경우
			{
				//마지막 pipe 뒤쪽의 command도 p_command에 추가해줌.
				p_command[p_command_cnt++]= &buf2[p_start];
				pipe_execution(p_command, p_command_cnt);				
			}
			else//pipe가 없는 경우
			{
				Exec(argv0, argv, environ);
			}
			exit(0);
		}
		/* Parent waits for foreground job to terminate */
		else
		{
			if (job->pid > 0)
				setpgid(pid, job->pid);
			else
			{
				job->pid = pid;
				setpgid(pid, job->pid);
			}
			if (!bg)
			{
				tcsetpgrp(0, pid);
				int status;
				while(!fg_end_flag)
					Sigsuspend(&prev_all);
				fg_end_flag=0;

				Signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(0, getpid());
				Signal(SIGTTOU, SIG_DFL);
			}
			else //when there is backgrount process!
			{
				Sio_puts("[");
				Sio_putl((long)(job->job_id));
				Sio_puts("] ");
				Sio_putl((long)(job->pid));
				Sio_puts("\n");
			}
		}
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

	if(!strcmp(argv[0], "jobs"))
	{
		print_jobs();
		return 1;
	}
	if(!strcmp(argv[0], "bg"))
	{
		bg_command(argv);
		return 1;
	}
	if(!strcmp(argv[0], "fg"))
	{
		fg_command(argv);
		return 1;
	}
	if(!strcmp(argv[0], "kill"))
	{
		kill_command(argv);
		return 1;
	}
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
