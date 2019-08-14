#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/un.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>

int main()
{
	int		sockfd;
	int		len;
	struct	sockaddr_in address;
	int		result;
	char	ch = 'A';
	char	send_buf[80];
	char	recv_buf[80];
	
	//为客户创建一个套接字
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		printf("创建失败");
		return -1;
	}
	else
	{
		printf("创建成功,等待连接\n");
	}
	sleep(1);	
	
	//根据服务器的情况给套接字命名
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(9734);
	len = sizeof(address);
	
	//将客户端的套接字连接到服务器的套接字
	result = connect(sockfd, (struct sockaddr *)&address, len);
	if(result == -1)
	{
		perror("oops: client");
		exit(1);
	}
	else
	{
		printf("连接成功\n");
		printf("请编辑“序号”加内容\n");
		printf("1.字符原样返回\n");
		printf("2.字符大写返回\n");
		printf("3.字符小写返回\n");
	}

    //输入要发送的数据 
    printf("Enter message to be sent:\n");
    gets(send_buf);

    // 发送数据缓存           
    len = send(sockfd, send_buf, strlen(send_buf) + 1, 0);
	//len = write(sockfd, send_buf, strlen(send_buf));
    if (len != strlen(send_buf)+1)
    {
		perror("send");
    }
	printf("%d bytes sent\n", len);

	// 接受数据缓存     
	len = read(sockfd, recv_buf, sizeof(recv_buf));
	printf("%d bytes received\n", len);
	printf("返回信息:%s \n", recv_buf);
	
	//通讯结束
	close(sockfd); 
	exit(0);
}
	
	
	
	
	