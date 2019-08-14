//server.h
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

//daemon.h
#include <time.h>
#include <fcntl.h>
#include <syslog.h>

#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/un.h>



/* 创建守护进程函数 */
int daemonize()
{
	int childpid, fd, fdtablesize;
	int error, in, out;
	/* 忽略终端I/O信号，STOP信号 */
	signal(SIGTTOU, SIG_IGN); /* 后台进程写控制终端 */
	signal(SIGTTIN, SIG_IGN); /* 后台进程读控制终端 */
	signal(SIGTSTP, SIG_IGN); /* 终端挂起 */
	/* 父进程退出，子进程成为孤儿进程 */
	if (fork()!=0)
	exit(1);
	/* 设置新会话的领头进程，并与原来的登录会话和进程组脱离 */
	if (setsid() == -1)
	exit(1);
	/* 防止会话组长重新申请控制终端,子进程退出，孙进程没有控制终端了 */
	if (fork()!=0)
	exit(1);
	/* 关闭打开的文件描述符，防止资源浪费和防止引起无法预料的错误 */
	for (fd = 3, fdtablesize = getdtablesize();fd < fdtablesize;fd++){
	close(fd);
	}

	/* 重定向标准输入／标准输出和标准错误输出 */
	error = open("./stderr", O_WRONLY|O_CREAT|O_APPEND, 0600);
	dup2(error, 2);
	close(error);
	in = open("./stdin", O_RDONLY|O_CREAT,0600);
	dup2(in, 0);
	close(in);
	out = open("./stdout", O_WRONLY|O_CREAT|O_APPEND,0600);
	dup2(out, 1);
	close(out);

	/* 可以改变工作目录 */
	chdir("/"); 
	/* 重设文件创建掩模 */
	umask(0);
	/* 忽略SIGCHLD信号, 防止僵尸进程 */
	signal(SIGCHLD, SIG_IGN);
	return 0;
}

/* 单实例实现, 成功返回0, 失败程序退出, 打印错误或已运行进程号 */
int already_running(const char *szLockFilePath)
{
	static int fd;
	char szBuf[256] = { 0x00 };
	fd = open(szLockFilePath, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd < 0){
	memset(szBuf, 0x00, sizeof(szBuf));
	sprintf(szBuf, "open LockFile failed!LockfilePath=[%s]/n", szLockFilePath);
	perror(szBuf);
	exit(1);
	}
	
	/* 尝试获取文件锁 */
	struct flock fl;
	memset(&fl, 0x00, sizeof(struct flock));
	if (fcntl(fd, F_GETLK, &fl) < 0){
	perror("fcntl get LockState failed!/n");
	close(fd);
	exit(1);
	}
	
	if (fl.l_type != F_UNLCK){
	memset(szBuf, 0x00, sizeof(szBuf));
	sprintf(szBuf, "已经有一个运行实例了。进程号为[%ld]/n", (long)fl.l_pid);
	perror(szBuf);
	close(fd);
	exit(1);
	}
	
	fl.l_whence = SEEK_SET;/* 决定l_start的位置 */
	fl.l_start = 0; /* 设置锁定区域开头位置 */
	fl.l_len = 0; /* 设置锁定区域长度 */
	fl.l_type = F_WRLCK;/* 设置锁定状态为写锁 */
	fl.l_pid = getpid(); /* 设置锁进程号 */
	/* 阻塞式加锁 */
	if (fcntl(fd, F_SETLK, &fl) < 0){
	perror("fcntl set Lockfile failed!/n");
	close(fd);
	exit(1);
	}
	
	/* 把进程号写入文件 */
	ftruncate(fd, 0);
	memset(szBuf, 0x00, sizeof(szBuf));
	sprintf(szBuf, "%ld", (long)getpid());
	write(fd, szBuf, strlen(szBuf)+1);
	return 0;
}








int main()
{
		int server_sockfd, client_sockfd;
		int server_len, client_len;
		
		struct 	sockaddr_in server_address;
		struct 	sockaddr_in client_address;
		int result;
		fd_set readfds, testfds;
		
		// Create an AF_UNIX stream socket       
   		server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		
		//判断创建socket是否失败
		if(server_sockfd < 0)
		{
			printf("Request socket failed!\n");
			return -1;
		}
		
		//地址结构配置
		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = htonl(INADDR_ANY);
		server_address.sin_port = htons(9734);
		server_len = sizeof(server_address);
		
		//bind
		
	
		if( bind(server_sockfd, (struct sockaddr *)&server_address, server_len) < 0 )
		{
			printf("bind failed!\n");
			return -1;
		}
		
		//listen
		
		if(listen(server_sockfd,5)< 0)
		{
			printf("listen failed!\n");
			close(server_sockfd);
			exit(-1);
		}
		
		//初始化readfds以处理来自server_sockfd的输入
		FD_ZERO(&readfds);
		FD_SET(server_sockfd, &readfds);
		
		//等待客户请求的到来
		
		//signal(SIGCHLD, SIG_IGN);
		
		
		//daemon
		time_t now;
		daemonize();
		already_running("./MyDaemon.pid");
		openlog("MyMsgDaemon", LOG_CONS | LOG_PID, 0);
		syslog(LOG_DEBUG, "守护进程测试/n");
		
		while(1)
		{
			char ch;
			int fd;
			int nread;
			
			time(&now);
			syslog(LOG_DEBUG, "守护进程测试, 当前时间：[%s]/n", ctime(&now));
			sleep(6);
			
			
			
			
			//给select函数使用的变量testfds赋值
			testfds = readfds;
			
			printf("server waiting\n");	
			
			//FD_SETSIZE默认大小是1024，意味最多1024个socket连接
			result = select(FD_SETSIZE, &testfds, (fd_set *)0,
			(fd_set *)0, (struct timeval *)0);
			if(result<1)
			{
				perror("server");
				exit(1);
			}
			
			//活动发生，用FD_ISSET检查描述符，看活动发生在哪个描述符
			for(fd = 0; fd < FD_SETSIZE; fd++)
			{
				//FD_ISSET有变化返回真，无变化返回假
				if(FD_ISSET(fd,&testfds))
				{
					//如果活动是发生在套接字server_sockfd上，它肯定是一个
					//新的连接请求，你就把相关的client_sockfd添加到描述符集合中
					if(fd == server_sockfd)
					{
						client_len = sizeof(client_address);
						client_sockfd = accept(server_sockfd,
							(struct sockaddr *)&client_address, &client_len);
						FD_SET(client_sockfd, &readfds);
						printf("adding client on fd %d \n",client_sockfd);
					}
					//如果活动不是发生在套接字server_sockfd上，它肯定是一个
					//客户的活动，如果接收到的是close，说明客户已经离开，可以把
					//该客户的套接字从描述符集合中删除。
					else
					{
						ioctl(fd, FIONREAD, &nread);
						
						if(nread == 0)
						{
							close(fd);
							FD_CLR(fd, &readfds);
							printf("removing client on fd %d\n",fd);
							
						}
						else
						{
							read(fd, &ch, 1);
							sleep(5);
							printf("serving client on fd %d\n",fd);
							ch++;
							write(fd, &ch, 1);
						}
					}
					
				}
			
			}
			
			
			
		}
}		