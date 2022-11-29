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

#define SERVER_C_PORT 21515
#define SERVER_CS_PORT 22515
#define SERVER_EE_PORT 23515
#define TCPPORT 25515
#define UDPPORT 24515
#define PASS "Authentication is successful"
#define FAIL_NO_USER "Authentication failed: Username Does not exist"
#define FAIL_PASS_NO_MATCH "Authentication failed: Password does not match"

int main(void)
{
    int socket_desc, client_sock, client_size, n, len;
    struct sockaddr_in server_addr, client_addr;
    char server_message[256], client_message[256], client_cmp[256], client_message1[256], client_course_msg[256], client_category_msg[256];
    
    // Clean cred_server_resps
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
    
    // Create TCP Main server socket, referred from Beej's guide
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socket_desc < 0){
        printf("Error while creating main server's TCP socket\n");
        return -1;
    }
    //printf("Socket created successfully\n");
    
    // Set port and IP for the Main server, referred from Beej's guide
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCPPORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Bind the socket created to the set port and IP
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("Could not bind the the TCP socket to the port\n");
        return -1;
    }
    
    // Listen for clients, referred from Beej's guide
    if(listen(socket_desc, 1) < 0){
        printf("Error while listening\n");
        return -1;
    }
    printf("The main server is up and running.\n");    

    // UDP Client socket
    int sock_udp;
	char cred_server_resp[1024], username[50], password[50];
	struct sockaddr_in credservaddr;
	
	// Creating socket file descriptor for UDP client, referred from Beej's guide
	if ( (sock_udp = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("Error while creating main server's UDP socket");
		exit(EXIT_FAILURE);
	}
    struct sockaddr_in udp_client;
    udp_client.sin_family = AF_INET;
    udp_client.sin_port = htons(UDPPORT);
    udp_client.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(bind(sock_udp, (struct sockaddr*)&udp_client, sizeof(server_addr))<0){
        printf("Could not bind the UDP socket to the port\n");
        return -1;
    }
	
	memset(&credservaddr, 0, sizeof(credservaddr));
		
	// Filling credential server information
	credservaddr.sin_family = AF_INET;
	credservaddr.sin_port = htons(SERVER_C_PORT);
	credservaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // EE Server details
	char ee_server_resp[1024];
	struct sockaddr_in eeservaddr;
	
	memset(&eeservaddr, 0, sizeof(eeservaddr));
		
	// Filling EE server information
	eeservaddr.sin_family = AF_INET;
	eeservaddr.sin_port = htons(SERVER_EE_PORT);
	eeservaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // CS Server details
	char cs_server_resp[1024];
	struct sockaddr_in csservaddr;
	
	memset(&csservaddr, 0, sizeof(csservaddr));
		
	// Filling CS server information
	csservaddr.sin_family = AF_INET;
	csservaddr.sin_port = htons(SERVER_CS_PORT);
	csservaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    while(1) {
    // Accept an incoming connection and open a child socket for the client
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    
    //printf("\n(%s , %d)\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
    if (client_sock < 0){
        printf("Can't accept\n");
        return -1;
    }
    while (1){
    memset(client_message, '\0', sizeof(client_message));
    if (recv(client_sock, client_message, sizeof(client_message), 0) < 0){
        break;
    }
    if (strcmp(client_message, "authenticate")==0) {
    while(1) {
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));
    // memset(client_course_msg, '\0', sizeof(client_course_msg));
    // memset(client_category_msg, '\0', sizeof(client_category_msg));
    // Receive client's message, referred from Beej's guide
    if (recv(client_sock, client_message, sizeof(client_message), 0) < 0){
        printf("Couldn't receive\n");
        return -1;
    }
    if (strcmp(client_message,"")==0) {
        break;
    }
    //printf("Msg from client: %s\n", client_message);
    strcpy(client_message1, client_message);
    strtok(client_message1 , "++++++++++");
    printf("The main server received the authentication for %s using TCP over port %d.\n", client_message1, TCPPORT);
    
    char encrypted_string[100] = {0};
    for(int i=0;i<strlen(client_message);i++)
    {
        int ascii_value=client_message[i];
        if (ascii_value>=48 && ascii_value<=57) 
        {
            char x = client_message[i];
            int a = atoi(&x);
            a=(a+4)%10;
            
            char c[2]={0};
            sprintf(c, "%d", a);
            encrypted_string[i]=*c;
        }

        else if ((ascii_value>=65 && ascii_value<=86) || (ascii_value>=97 && ascii_value<=118))
        {
            int a = ascii_value+4;
            char c=a;
            encrypted_string[i]=c;
        }

        else if ((ascii_value>=87 && ascii_value<=90) || (ascii_value>=119 && ascii_value<=122))
        {
            int a = ascii_value-22;
            char c=a;
            encrypted_string[i]=c;
        }
        else
        {
            char c = client_message[i];
            encrypted_string[i]=c;
        }
    }
    //char* token = strtok(encrypted_string, ",");
    strcpy(username, strtok(encrypted_string , "++++++++++"));
    strcpy(password, strtok(NULL, "++++++++++"));
    //printf("hii,%s,%s",username,password);
    //char cred_server_resp[1024];
	//char hello[50] = "Hello from client";
    
		
	sendto(sock_udp, username, strlen(username),
		MSG_CONFIRM, (const struct sockaddr *) &credservaddr,
			sizeof(credservaddr));
	//printf("Username sent.\n");
			

    // send password to credentials server
	sendto(sock_udp, password, strlen(password),
		MSG_CONFIRM, (const struct sockaddr *) &credservaddr,
			sizeof(credservaddr));
	printf("The main server sent an authentication request to serverC.\n");
	
    n = recvfrom(sock_udp, (char *)cred_server_resp, 1024,
			MSG_WAITALL, (struct sockaddr *) &credservaddr,
			&len);
	cred_server_resp[n] = '\0';
	printf("The main server received the result of the authentication request from ServerC using UDP over port %d.\n", UDPPORT);


    // Respond to client:
    //strcpy(server_message, "This is the server's message.");
    //printf("Encrypted string: %s\n", encrypted_string);
    
    if (strcmp(cred_server_resp, FAIL_NO_USER) == 0) {
    if (send(client_sock, FAIL_NO_USER, strlen(FAIL_NO_USER), 0) < 0){
        printf("Can't send\n");
        return -1;
    }
    }
    else if (strcmp(cred_server_resp, FAIL_PASS_NO_MATCH) == 0) {
    if (send(client_sock, FAIL_PASS_NO_MATCH, strlen(FAIL_PASS_NO_MATCH), 0) < 0){
        printf("Can't send\n");
        return -1;
    }
    }
    else {
    if (send(client_sock, PASS, strlen(PASS), 0) < 0){
        printf("Can't send\n");
        return -1;
    }
    }
    printf("The main server sent the authentication result to the client.\n");
    break;
    }
    }
    else if (strcmp(client_message, "repeated query")==0){
    while(1) {
    memset(client_cmp, '\0', sizeof(client_cmp));
    memset(client_course_msg, '\0', sizeof(client_course_msg));
    memset(client_category_msg, '\0', sizeof(client_category_msg));
    if (recv(client_sock, client_cmp, sizeof(client_cmp), 0) < 0){
        printf("Couldn't receive\n");
        return -1;
    }
    if (strcmp(client_cmp,"single query")==0) {
    // Receive client's message:
    if (recv(client_sock, client_course_msg, sizeof(client_course_msg), 0) < 0){
        printf("Couldn't receive\n");
        return -1;
    }
    //printf("hii, %s\n", client_course_msg);
    if (recv(client_sock, client_category_msg, sizeof(client_category_msg), 0) < 0){
        printf("Couldn't receive\n");
        return -1;
    }
    //printf("client message: %s, %s\n", client_course_msg, client_category_msg);
    printf("The main server received from %s to query course %s about %s using TCP over port %u.\n", client_message1, client_course_msg, client_category_msg, TCPPORT);
    
    char dept[5]={'\0'};
    memcpy(dept, client_course_msg, 2);
    if (strcmp(dept,"EE")==0)
    {
    sendto(sock_udp, client_course_msg, strlen(client_course_msg),
	MSG_CONFIRM, (const struct sockaddr *) &eeservaddr,
		sizeof(eeservaddr));

	sendto(sock_udp, client_category_msg, strlen(client_category_msg),
		MSG_CONFIRM, (const struct sockaddr *) &eeservaddr,
			sizeof(eeservaddr));
    printf("The main server sent a request to server%s.\n", dept);

    n = recvfrom(sock_udp, (char *)ee_server_resp, 1024,
			MSG_WAITALL, (struct sockaddr *) &eeservaddr,
			&len);
	ee_server_resp[n] = '\0';
    printf("The main server received the response from server%s using UDP over port %d.\n", dept, SERVER_EE_PORT);
    if (send(client_sock, ee_server_resp, strlen(ee_server_resp), 0) < 0){
        printf("Can't send\n");
        return -1;
    }
    printf("The main server sent the query information to the client.\n");
    }
    else if (strcmp(dept,"CS")==0) {
    sendto(sock_udp, client_course_msg, strlen(client_course_msg),
	MSG_CONFIRM, (const struct sockaddr *) &csservaddr,
		sizeof(csservaddr));

	sendto(sock_udp, client_category_msg, strlen(client_category_msg),
		MSG_CONFIRM, (const struct sockaddr *) &csservaddr,
			sizeof(csservaddr));
    printf("The main server sent a request to server%s.\n", dept);

    n = recvfrom(sock_udp, (char *)cs_server_resp, 1024,
			MSG_WAITALL, (struct sockaddr *) &csservaddr,
			&len);
	cs_server_resp[n] = '\0';
    printf("The main server received the response from server%s using UDP over port %d.\n", dept, UDPPORT);
    if (send(client_sock, cs_server_resp, strlen(cs_server_resp), 0) < 0){
        printf("Can't send\n");
        return -1;
    }
    printf("The main server sent the query information to the client.\n");
    }
    else {
        char fail_msg[50]={'\0'};
        printf("Didn’t find the course: %s\n", client_course_msg);
        strcat(fail_msg, "Didn’t find the course: ");
        strcat(fail_msg, client_course_msg);
        if (send(client_sock, fail_msg, strlen(fail_msg), 0) < 0){
        printf("Can't send\n");
        return -1;
        printf("The main server sent the query information to the client.\n");
    }
    }
    break;
    }
    else if (strcmp(client_cmp, "")!=0) {
    memset(client_course_msg, '\0', sizeof(client_course_msg));
    int count;
    char array[10][50];
    count=atoi(client_cmp);
    //printf("count:%d, %s\n",count, client_cmp);
    // Receive client's message:
    for (int i=0;i<count;i++) {
    if (recv(client_sock, client_course_msg, sizeof(client_course_msg), 0) < 0){
        printf("Couldn't receive\n");
        return -1;
    }
    //printf("client message: %s\n", client_course_msg);
        char dept[5]={'\0'};
    memcpy(dept, client_course_msg, 2);
    if (strcmp(dept, "EE")==0) {
    sendto(sock_udp, client_course_msg, strlen(client_course_msg),
		MSG_CONFIRM, (const struct sockaddr *) &eeservaddr,
			sizeof(eeservaddr));
    sendto(sock_udp, "all details", strlen("all details"),
		MSG_CONFIRM, (const struct sockaddr *) &eeservaddr,
			sizeof(eeservaddr));
    printf("The main server sent a request to server%s.\n", dept);
    memset(ee_server_resp, '\0', sizeof(ee_server_resp));
    n = recvfrom(sock_udp, (char *)ee_server_resp, 1024,
			MSG_WAITALL, (struct sockaddr *) &eeservaddr,
			&len);
    printf("The main server received the response from server%s using UDP over port %d.\n", dept, UDPPORT);
    //printf("response: %s\n", ee_server_resp);
    if (send(client_sock, ee_server_resp, strlen(ee_server_resp), 0) < 0){
        printf("Can't send\n");
        return -1;
    }
    }
    else if (strcmp(dept, "CS")==0) {
    sendto(sock_udp, client_course_msg, strlen(client_course_msg),
		MSG_CONFIRM, (const struct sockaddr *) &csservaddr,
			sizeof(csservaddr));
    sendto(sock_udp, "all details", strlen("all details"),
		MSG_CONFIRM, (const struct sockaddr *) &csservaddr,
			sizeof(csservaddr));
    printf("The main server sent a course request to server%s.\n", dept);

    memset(cs_server_resp, '\0', sizeof(cs_server_resp));
    n = recvfrom(sock_udp, (char *)cs_server_resp, 1024,
			MSG_WAITALL, (struct sockaddr *) &csservaddr,
			&len);
    printf("The main server received the response from server%s using UDP over port %d.\n", dept, UDPPORT);    
    //printf("response: %s\n", cs_server_resp);
    if (send(client_sock, cs_server_resp, strlen(cs_server_resp), 0) < 0){
        printf("Can't send\n");
        return -1;
    }        
    }
    else {
        char fail_msg[50]={'\0'};
        printf("Didn’t find the course: %s\n", client_course_msg);
        strcat(fail_msg, "Didn’t find the course: ");
        strcat(fail_msg, client_course_msg);
        if (send(client_sock, fail_msg, strlen(fail_msg), 0) < 0){
        printf("Can't send\n");
        return -1;
    }
    }
    }
    break;
    }
    break;
    }
    }
    else {
        close(client_sock);
    }
    }
    }
    
    // Closing the socket:
    
    close(socket_desc);
    
    return 0;
}