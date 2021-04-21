// c lib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// unix lib
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>

#define BUFFER_SIZE 256

void errormsg(const char * msg){
    perror(msg);
    perror("program stop");
    exit(EXIT_FAILURE);
}


struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd,file_block_length;
char buffer[BUFFER_SIZE];
 
int main (int argc, char *argv[ ])
{
/* Create a datagram socket on which to send. */
	sd = socket(PF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
		errormsg("Opening datagram socket error");
	else
	  	printf("Opening the datagram socket...OK.\n");
	 
	/* Initialize the group sockaddr structure with a */
	/* group address of 226.1.1.1 and port 4321. */

	memset((char *) &groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
	groupSock.sin_port = htons(5566);
	 
	/* Set local interface for outbound multicast datagrams. */
	/* The IP address specified must be associated with a local, */
	/* multicast capable interface. */
	localInterface.s_addr = inet_addr("127.0.0.1");
	
	/* IP_MULTICAST_IF:  Sets the interface over which outgoing multicast datagrams are sent. */
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
		errormsg("Setting local interface error");
	else
	  	printf("Setting the local interface...OK\n");
	/* Send a message to the multicast group specified by the*/
	/* groupSock sockaddr structure. */

	FILE *fp = fopen(argv[1],"r"); //open file to be sent and create file pointer
    if(fp == NULL)
        errormsg("fail to open file");
   	else{
		struct stat send_st; //get send file size from system
		stat(argv[1], &send_st);
		float send_filesize = send_st.st_size;
		float send_filesizeMB = send_filesize / 1000.0 / 1000.0; // byte to MB
   		
   		memset(buffer, 0, BUFFER_SIZE);// clear buffer
	    while( (file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0){//read file into buffer
	        if(sendto(sd, buffer, file_block_length, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)//send file to socket
	            errormsg("Sending datagram message error");

	 		memset(buffer, 0, BUFFER_SIZE);// clear buffer for next loop
	    }
	    //file transfer finish
	    printf("Sending datagram message...OK\n");
	    printf("file size : %f MB \n",send_filesizeMB);
    }
	// close socket and file pointer
	fclose(fp);
	close(sd);
	return 0;
}
