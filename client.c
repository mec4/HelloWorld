#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MY_PORT		5000
#define PI	3.14

int sockfd;

int main(int argc, char *argv[])
{
	int ret, c;
	struct sockaddr_in serv_addr;

	if(argc != 2)
	{
		printf("erreur d'utilisation, usage: ./client ip_addr\n");
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		printf("erreur socket\n");
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(MY_PORT);
	ret = inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);
	if(ret < 0)
	{
		printf("erreur inet_pton\n");
		return -1;
	}

	ret = connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(ret < 0)
	{
		printf("erreur connect\n");
		//perror("connect\n");
		return -1;
	}
	printf("demande envoyée");
char buf[256];
//On ouvre le fichier
int fd;
fd = open("data.csv", O_CREAT|O_WRONLY|O_TRUNC);
	if(fd < 0)
	{
		printf("erreur open\n");
		return -1;
	}

	while(1)
	{
	ret = read(sockfd, &buf, sizeof(buf));
	printf("%s", buf);
	if(ret < 0){
		printf("erreur read\n");
		return -1;
	}
	else if(ret==0){
		printf("client deconnecté \n");
		return 0;
	}
	else{	
puts("writing\n");		
		ret = write(fd, buf, strlen(buf));
		if(ret <= 0){
			printf("erreur write\n");
			return -1;
		}
	}
	}
	return 0;
}

