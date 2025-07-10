#include "header.h"



pid_t pid;
int status;



int main()
{
    char prompt[30]="MINISHELL";
    char input_string[100];
    system("clear");
    scan_input(prompt,input_string);
}



void scan_input(char *prompt, char *input_string)
{
    //signal 
    signal(SIGINT,signal_handler);


    while(1)
    {
	printf(ANSI_COLOR_GREEN"%s$"ANSI_COLOR_RESET,prompt);
	scanf("%[^\n]",input_string);
//	getchar();


	//to change user
	if(strstr(input_string,"PS1") != NULL)
	{
	    if(input_string[3] == '=' && input_string[4] != ' ')
	    {
		strcpy(prompt,input_string+4);
	    }
	    else
	    {
		printf("%s:  command not found\n",input_string);
	    }
	}

	
	
	//to check cmd
	char *word=get_command(input_string);
	int ret=check_command_type(word);

	if(ret==BUILTIN)
	{
	    execute_internal_commands(input_string);
	}
	else if(ret==EXTERNAL)
	{
	    pid=fork();
	    //child
	    if(pid==0)
	    {
		execute_external_commands(input_string);
		exit(0);
	    }
	    else
	    {
		//parent
		pid = wait(&status);
		if(WIFEXITED(status))
		{
		    printf("Child with pid : %d terminated normal\n",pid);
		}
		else if(WIFSIGNALED(status))
		{
		    printf("Child with pid : %d terminated by a signal\n",pid);
		}
	    }

	}
	else if(ret==NO_COMMAND)
	{
	    if(word[0]== 0);
	    else
		printf("%s:  command not found\n",input_string);
	}

	//to clear input_string
	memset(input_string,0,100);

	__fpurge(stdin);

    }
}




void signal_handler(int sig_num)
{
    if(pid != 0)
    {
	if(sig_num==SIGINT)
	{
	    kill(pid,SIGKILL);
	}
    }
}


void execute_external_commands(char *input_string)
{

    replace_blank(input_string);

    char *argv[20]={NULL};
    int a=1;
    char *token=strtok(input_string," ");
    while(token != NULL)
    {
	argv[a++]=token;
	token=strtok(NULL," ");
    }
    int i=1,j=0,k=1,count=1;
    int pos[10],cmd[10];
    cmd[0]=1;
    while(argv[i])
    {
	if(!strcmp(argv[i],"|"))
	{
	    pos[j++]=i;
	    count++;
	    argv[i]=NULL;
	    cmd[k++]=i+1;
	}
	i++;
    }
    for(int i=0;i<count;i++)
    {
	int fd[2];
	pipe(fd);
	pid_t pid=fork();
	if(pid==0)
	{
	    //child
	    close(fd[0]);
	    if(i != count-1)
		dup2(fd[1],1);
	    execvp(argv[cmd[i]],argv+cmd[i]);
	}
	else
	{
	    //parent
	    wait(&status);
	    close(fd[1]);
	    dup2(fd[0],0);
	}
    }
}
