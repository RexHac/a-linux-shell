#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<ctype.h>
#include<pwd.h>
#include<errno.h>
#include<string.h>
char buildin[2][10] = {"cd", "exit"};
void process_command(char *cmd,char **para)
{
    int i = 1;
//255 needs to be reconsidering
    ssize_t s = read(0, cmd, 255);
    if (s > 0)
    {
        cmd[s - 1] = '\0';
        char *start = cmd;
	para[0] = cmd;
        while(*start)
        {
            if(!isspace(*start))
            {
                start ++;
            }
            else
            {
                *start  = '\0';
                start ++;
                para[i ++] = start;
            }
        }
        para[i] = NULL;
    }
}

void type_prompt()
{
    char prompt[200], username[50], hostname[50], path[100];
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    gethostname(hostname,sizeof(hostname));
    getcwd(path,sizeof(path));
	printf("[myshell]\e[1;33m%s@%s:\033[0m", pwd->pw_name,hostname);
    if(getuid() == 0)
    {
        printf("\e[1;34m%s\033[0m# ", path);
    }
    else
    {
        printf("\e[1;34m%s\033[0m$ ", path);
    }
}

void cd(char **para)
{
    if(chdir(para[1]) < 0)
    {
        printf("errno: %d, %s: %s", errno, para[1], "no such file or directory\n");
    }
}

int isBuildinCmd(char **cmd)
{
    int len = sizeof(buildin) / 10;
    for (int i  = 0; i < len; i ++)
    {
        if(strcmp(cmd[0],buildin[i]) == 0)
        {   
			switch(i)
			{
				case 0:
				{
					cd(cmd);
					break;
				}
				case 1:
				{
					exit(2);
					break;
				}
			}   
            return 1;
        }
    }
    return 0;
}

int main()
{
    char cmd[256];
    char *para[32];
    while(1)
    {
        /*printf("RexLee@ubuntu:[myshell]#");*/
        type_prompt();
        fflush(stdout);
        sleep(5);
        process_command(cmd,para);
        if(isBuildinCmd(para))
        {
            // cd(para);
        }
        else
        {
            if(fork()!=0)
            {
                int status = 0;
                waitpid(-1,&status,0);
            } 
            else
            {
                execvp(para[0],para);
                exit(2);
            }
        }

    }
}