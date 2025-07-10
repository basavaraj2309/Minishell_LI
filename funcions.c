#include "header.h"

extern int status;

//builtin commands
char *builtins[] = {"echo", "printf","clear", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval","set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source","exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};


char *get_command(char *input_string)
{
    char *cmd=malloc(20);
    int i;
    for(i=0;input_string[i];i++)
    {
	if(input_string[i]!=' ')
	{
	    cmd[i]=input_string[i];
	}
	else
	{
	    break;
	}
    }
    cmd[i]='\0';
    return cmd;
}


int check_command_type(char *command)
{
    int i=0;
    while(builtins[i])
    {
	if(!strcmp(builtins[i],command))
	    return BUILTIN;
	i++;
    }

    int fd=open("ext_cmds.txt",O_RDONLY);
    char word[20],ch;
    i=0;
    int rd=read(fd,&ch,1);
    while(rd>0)
    {
	if(ch != '\n')
	{
	    word[i++]=ch;
	}
	else
	{
	    word[i]='\0';
	    if(!strcmp(word,command))
		return EXTERNAL;
	    i=0;
	}
	rd=read(fd,&ch,1);
    }

    return NO_COMMAND;
}


void echo(char *input_string, int status)
{
    if(!strcmp(input_string,"echo $?"))
    {
	printf("%d\n",status);
    }
    else if(!strcmp(input_string,"echo $$"))
    {
	printf("%d\n",getpid());
    }
    else if(!strcmp(input_string,"echo $SHELL"))
    {
	printf("%s\n",getenv("SHELL"));
    }
}


void execute_internal_commands(char *input_string)
{
    //to ge pwd
    if(!strcmp(input_string,"pwd"))
    {
	char buf[200];
	printf("%s\n",getcwd(buf,200));
    }


    //to exit
    if(!strncmp(input_string,"exit",4))
    {
	printf("exiting\n");
	exit(0);
    }

    //to change directory
    if(!strncmp(input_string,"cd",2))
    {
	char *ptr=strchr(input_string,'/');
	//input_string++;
	chdir(ptr);
    }

    //to clear
    if(!strncmp(input_string,"echo",4))
    {
	echo(input_string,status);
    }
    if(!strcmp(input_string,"clear"))
    {
	system("clear");
    }
}



void replace_blank(char str[])
{
    int len=0;                            // to find len
    while(str[len]!='\0')
        len++;
    for(int i=0;i<len;i++)
    {
        if(str[i]==' ' && str[i+1]==' ') // to check the condition
        {
            for(int j=i;j<len;j++)
            {
                str[j]=str[j+1];        // initializing the next ch to prev ch
            }
            i--;                        // to check again in the same place
        }
    }
}

