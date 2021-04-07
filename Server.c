/**********************************
tcp_ser.c: the source file of the server in tcp transmission 
***********************************/


#include "headsock.h"

#define BACKLOG 10

void str_ser(int sockfd);                                                        // transmitting and receiving function

int main(void)
{
	int sockfd;
	struct sockaddr_in my_addr;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {			//create socket
		printf("error in socket");
		exit(1);
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYUDP_PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero), 8);
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {           //bind socket
		printf("error in binding");
		exit(1);
	}
	printf("start receiving\n");

	while (1)
	{
		printf("waiting for data\n");
		str_ser(sockfd);   
		close(sockfd);
		exit(0);                                       //receive packet and response                                      //parent process
	}
}

void str_ser(int sockfd)
{
	char buf[BUFSIZE];
	FILE *fp;
	char recvs[DATALEN];
	struct ack_so ack;
	int end, n = 0, len;
	long lseek=0;
	struct sockaddr_in addr;
	
	len = sizeof (struct sockaddr_in);

	printf("receiving data!\n");

	int count = 1;
	int packetReceived = 0;
	ack.num = 0;
	ack.len = 0;
	while(!end)
	{	
		if ((n =recvfrom(sockfd, &recvs, DATALEN, 0, (struct sockaddr *)&addr, &len))==-1)                                   //receive the packet
		{
			printf("error when receiving\n");
			exit(1);
		}
		packetReceived += 1;
		
		if (recvs[n-1] == '\0')									//if it is the end of the file
		{
			end = 1;
			n --;
		}
		memcpy((buf+lseek), recvs, n);
		lseek += n;

		if(packetReceived == count)
		{
			ack.num += 1;
			//printf("%d\n", ack.num);
			if ((n = sendto(sockfd, &ack, 2, 0, (struct sockaddr *)&addr, len))==-1)
			{
				printf("send error!");								
				exit(1);
			}
			packetReceived = 0;
			if(count == 3)
			{
				count = 1;
			}
			else
			{
				count += 1;
			}
		}
	}

	if ((fp = fopen ("myUDPreceive.txt","wt")) == NULL)
	{
		printf("File doesn't exit\n");
		exit(0);
	}
	fwrite (buf , 1 , lseek , fp);					//write data into file
	fclose(fp);
	printf("a file has been successfully received!\nthe total data received is %d bytes\n", (int)lseek);
}
