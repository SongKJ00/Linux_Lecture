/*
	Simple HTTP server program 
	author : Keunjae Song
	date : 2017. 09. 06
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
	int sock_fd, client_fd;
	int port;
	int opt = 1;
	struct sockaddr_in addr;
	struct sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	char buffer[1024];
	
	if(argc < 2)
	{
		perror("arguments are not enough");
		exit(1);
	}

	port = atoi(argv[1]);

	//addr information setting(initialize)
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//open socket
	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
	    perror("socket() error");
		exit(1);
	}

	//socket option setting
	//SOL_SOCKET : 소켓 옵션 레벨 설정 
	//SO_REUSEADDR : 통신이 끊겨도 이미 사용된 주소를 rebind 하도록 설정
	if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt)) != 0)
	{
		perror("setsockopt() error");
		exit(1);
	}

	//addr information bind
	if(bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		  perror("bind() error");
		  exit(1);
	}

	//listen client
	if(listen(sock_fd, 5) == -1)
	{
		perror("listen() error");
		exit(1);
	}

	printf("waiting for clinet.....\n");

	//accept client
	if((client_fd = accept(sock_fd, (struct sockaddr*)&client_addr, &client_addr_size)) == -1)
	{
		perror("accept() error");
		exit(1);
	}

	printf("connected by client\n\n");

	//receive HTTP request from client(web browser)
	if(recv(client_fd, buffer, sizeof(buffer), 0) > 0)
	{
		  printf("%s", buffer);
	}

	//send HTTP response to client(web browser)
	http_respond(client_fd);

	close(sock_fd);
	close(client_fd);

	return 0;
}

//HTTP response send function
void http_respond(int client)
{
	char *path;
	char *response_packet;
	char send_buffer[1024];
	int fd;
	int read_byte; 

	path = "./index.html";
	response_packet = "HTTP/1.1 200 OK\n\n";

	if((fd = open(path, O_RDONLY)) != -1) 
	{
		send(client, response_packet, strlen(response_packet), 0);
		while((read_byte = read(fd, send_buffer, 1024)) > 0)
			send(client, send_buffer, strlen(send_buffer), 0);
	}

	else 
	{
		perror("file open failed");
	}
	
	close(fd);
}
