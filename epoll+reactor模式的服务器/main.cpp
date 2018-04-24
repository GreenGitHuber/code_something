#include <iostream>
#include <signal.h>     //for signal()
#include <unistd.h>
#include <stdlib.h>		//for exit()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "myreactor.h"

CMyReactor g_reator;

void prog_exit(int signo)
{
    std::cout << "program recv signal " << signo << " to exit." << std::endl; 

	g_reator.uninit();
}

void daemon_run()
{
    int pid;
    signal(SIGCHLD, SIG_IGN);

    pid = fork();
    if (pid < 0)
    {
        std:: cout << "fork error" << std::endl;
        exit(-1);
    }
    else if (pid > 0) {
        exit(0);
    }
    setsid();
    int fd;
    fd = open("/dev/null", O_RDWR, 0);
    if (fd != -1)
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
    }
    if (fd > 2)
        close(fd);
}


int main(int argc, char* argv[])
{  
	signal(SIGCHLD, SIG_DFL);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, prog_exit);
	signal(SIGKILL, prog_exit);
	signal(SIGTERM, prog_exit);
	
	short port = 0;
	int ch;
	bool bdaemon = false;
	while ((ch = getopt(argc, argv, "p:d")) != -1)
	{
		switch (ch)
		{
		case 'd':
			bdaemon = true;
			break;
		case 'p':
			port = atol(optarg);
			break;
		}
	}

	if (bdaemon)
		daemon_run();


	if (port == 0)
		port = 12345;

	if (!g_reator.init("0.0.0.0", 12345))
		return -1;
	g_reator.main_loop(&g_reator);
    return 0;
}
