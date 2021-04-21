// c lib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// unix lib
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <errno.h>

#define BUFFER_SIZE 256

void errormsg(const char * msg){
    perror(msg);
    perror("program stop");
    exit(EXIT_FAILURE);
}

 
struct sockaddr_in localSock;
struct ip_mreq group;
int sd,recv_length;
char buffer[BUFFER_SIZE];
 
int main(int argc, char *argv[])
{
/* Create a datagram socket on which to receive. */
	sd = socket(PF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
		errormsg("Opening datagram socket error");
	else
		printf("Opening datagram socket....OK.\n");
		 
	/* Enable SO_REUSEADDR to allow multiple instances of this */
	/* application to receive copies of the multicast datagrams. */
	
	int reuse = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
		errormsg("Setting SO_REUSEADDR error");
	else
		printf("Setting SO_REUSEADDR...OK.\n");
	
	 
	/* Bind to the proper port number with the IP address */
	/* specified as INADDR_ANY. */
	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(4321);
	localSock.sin_addr.s_addr = INADDR_ANY;
	if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock)))
		errormsg("Binding datagram socket error");
	else
		printf("Binding datagram socket...OK.\n");
	 
	/* Join the multicast group 226.1.1.1 on the local address*/
	/* interface. Note that this IP_ADD_MEMBERSHIP option must be */
	/* called for each local interface over which the multicast */
	/* datagrams are to be received. */
	group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
	/* your ip address */ 
	group.imr_interface.s_addr = inet_addr("127.0.0.1"); 
	/* IP_ADD_MEMBERSHIP:  Joins the multicast group specified */ 
	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
		errormsg("Adding multicast group error");
	else
		printf("Adding multicast group...OK.\n");

	struct timeval timeOut;//set recv timer for socket
	timeOut.tv_sec = 5;// 5 sec timeout
	timeOut.tv_usec = 0;
    if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &timeOut, sizeof(timeOut)) < 0)
		errormsg("fail to set socket recv timeout timer\n");

    FILE *fp = fopen("recv.txt", "w");//open write file pointer
    if(fp == NULL)
        errormsg("fail to open write file");

    while(1){
		recv_length = 0;
		recv_length = recvfrom(sd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if(recv_length < 0){
            if(errno == EAGAIN){//timeout error
            	printf("socket reach time limit : %ld sec ,socket closed\n",timeOut.tv_sec);
                break;
            }
            else
            	errormsg("fail to recv data from socket");
        }
        else if(recv_length > 0){
            int write_length = fwrite(buffer, sizeof(char), recv_length, fp);//write buffer to file
            if(write_length < recv_length)
                errormsg("fail to write data from recv buffer to file");
            memset(buffer, 0, BUFFER_SIZE);// clear buffer
       }
    }
    printf("Reading datagram message...OK.\n");
    // close socket and file pointer
	fclose(fp);
	close(sd);

   	struct stat send_st; //get send file size from system
	stat("recv.txt", &send_st);
	float send_filesize = send_st.st_size;
	float send_filesizeMB = send_filesize / 1000.0 / 1000.0; // byte to MB
	printf("file size : %f MB \n",send_filesizeMB);
	return 0;
}
