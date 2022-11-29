#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <sys/wait.h>
	
#define PORT	 21515
#define MAXLINE 1024
#define PASS "Authentication is successful"
#define FAIL_NO_USER "Authentication failed: Username Does not exist"
#define FAIL_PASS_NO_MATCH "Authentication failed: Password does not match"
	
// Driver code
int main() {
	int sock_c;
	char encrypted_username[50], encrypted_password[50], username[50], password[50];
	struct sockaddr_in cservaddr, cliaddr;
		
	// Creating socket file descriptor
	if ( (sock_c = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
	memset(&cservaddr, 0, sizeof(cservaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling credential server information
	cservaddr.sin_family = AF_INET; 
	cservaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	cservaddr.sin_port = htons(PORT);
		
	// Binding the socket with the credential server address
	if ( bind(sock_c, (const struct sockaddr *)&cservaddr,
			sizeof(cservaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	printf("The ServerC is up and running using UDP on port %d.\n", PORT);
    while(1){
	int len, n;
	
	len = sizeof(cliaddr); 
	
	// Receive encrypted username and password from the main server
	n = recvfrom(sock_c, (char *)encrypted_username, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				&len);
	encrypted_username[n] = '\0';
	//printf("Client : %s\n", encrypted_username);

	n = recvfrom(sock_c, (char *)encrypted_password, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				&len);
	encrypted_password[n] = '\0';
	//printf("Client : %s\n", encrypted_password);
	printf("The ServerC received an authentication request from the Main Server.\n");

	// Read cred.txt file

	FILE* fptr;
	char str[500];
	fptr = fopen("cred.txt", "r");
    //char* token = strtok(str, ",");

	if (NULL == fptr) {
		printf("file cred.txt can't be opened \n");
	}

    // Check if user exists
    int result, password_check;
	while (fgets(str, 500, fptr) != NULL) {
		str[strcspn(str, "\n")] = '\0';
		strcpy(username, strtok(str , ","));
        strcpy(password, strtok(NULL, ","));
		//password[strlen(password)-1]=0;
        
        result = strcmp(username, encrypted_username);
        
        if (result==0) {
            password_check = strcmp(password, encrypted_password);
			break;
        }
	}
	//printf("pwd:%s, epwd:%s, %d, %d, %ld, %ld", password, encrypted_password, result, password_check, strlen(password), strlen(encrypted_password));
    // Send appropriate result to the Main server
	if (result!=0) {
		sendto(sock_c, (const char *)FAIL_NO_USER, strlen(FAIL_NO_USER),
		MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
			len);
	}
	else if (password_check != 0) {
		sendto(sock_c, (const char *)FAIL_PASS_NO_MATCH, strlen(FAIL_PASS_NO_MATCH),
		MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
			len);
	}
	else {
		sendto(sock_c, (const char *)PASS, strlen(PASS),
		MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
			len);
	}
	
	fclose(fptr);
	printf("The ServerC finished sending the response to the Main Server.\n");
	}
	close(sock_c);
	return 0;
}
