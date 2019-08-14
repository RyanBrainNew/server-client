#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

/* 创建守护进程函数 */
int daemonize()
{
int childpid, fd, fdtablesize;
int error, in, out;
/* 忽略终端I/O信号，STOP信号 */
signal(SIGTTOU, SIG_IGN); /* 后台进程写控制终端 */
signal(SIGTTIN, SIG_IGN); /* 后台进程读控制终端 */
signal(SIGTSTP, SIG_IGN); /* 终端挂起 */
signal(SIGHUP, SIG_IGN); /* 进程组长退出时向所有会议成员发出的 */
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
int fd;
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

int main(void){
	
time_t now;
daemonize();
already_running("./MyDaemon.pid");
openlog("MyMsgDaemon", LOG_CONS | LOG_PID, 0);
syslog(LOG_DEBUG, "守护进程测试/n");
while (1){
time(&now);
syslog(LOG_DEBUG, "守护进程测试, 当前时间：[%s]/n", ctime(&now));
sleep(6);
}
return 0;
}