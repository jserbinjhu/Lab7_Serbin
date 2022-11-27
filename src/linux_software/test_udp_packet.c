
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 


#define PORT  25344

void create_packet(char *message)
{
	int count = 2; 
	unsigned int test_sample = 0xABACEF12;
	char bytes[4];
	while (count < 1026)
	{
		bytes[0] = (test_sample >> 24) & 0xFF;
		bytes[1] = (test_sample >> 16) & 0xFF;
		bytes[2] = (test_sample >> 8) & 0xFF;
		bytes[3] = test_sample & 0xFF;
	    message[count] = bytes [0];
		count++; 
	    message[count] = bytes [1];
		count++; 
	    message[count] = bytes [2];
		count++;  
	    message[count] = bytes [3];
		count++; 
	}		
}

    
// Driver code 
int main() { 
    int sockfd; 
    char samples[1026];
	char ip[15];
	int16_t packet_count = 62; //place holder 
    struct sockaddr_in servaddr, cliaddr; 
	
    printf("Enter IP address to send to: "); 
	scanf("%s", ip); 
        
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
        
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
        
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
	
	// Filling client information 
    cliaddr.sin_family    = AF_INET; // IPv4 
    cliaddr.sin_addr.s_addr = inet_addr(ip); 
    cliaddr.sin_port = htons(PORT); 
        
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
	
    for (int i =0; i < 10; i++){
	create_packet(samples); 
	samples[0] = (packet_count >> 8) & 0xFF;
	samples[1] = packet_count & 0xFF;
	packet_count++; 
	sendto(sockfd, samples, sizeof(samples), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
	}
	
    printf("10 Sample Packets sent.\n");  
        
    return 0; 
}