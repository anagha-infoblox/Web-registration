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
#include <ctype.h>

#define SERVER_M_PORT 25515
#define PASS "Authentication is successful"
#define FAIL_NO_USER "Authentication failed: Username Does not exist"
#define FAIL_PASS_NO_MATCH "Authentication failed: Password does not match"

int main(void)
{
    int client_sock;
    struct sockaddr_in server_m_addr;
    char server_m_message[2000], username[50], password[50], username1[50], course_code[50], category[50]={'\0'};
    
    // Clearing the buffers of M Server message and for username & password
    memset(server_m_message,'\0',sizeof(server_m_message));
    memset(username,'\0',sizeof(username));
    memset(password,'\0',sizeof(password));
    
    // Create TCP client socket, referred from Beej's guide
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if(client_sock < 0){
        printf("Could not create the socket\n");
        return -1;
    }
    
    // Set Main Server's port and IP, referred from Beej's guide
    server_m_addr.sin_family = AF_INET;
    server_m_addr.sin_port = htons(SERVER_M_PORT);
    server_m_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send connection request to serverM, referred from Beej's guide
    if(connect(client_sock, (struct sockaddr*)&server_m_addr, sizeof(server_m_addr)) < 0){
        printf("Could not connect\n");
        return -1;
    }

    printf("The client is up and running\n");

    // Get dynamic port of the TCP client, referred from EE450_Project_Fall_2022.pdf page 17
    struct sockaddr_in client_address;
    int addr_size = sizeof(client_address);
    getsockname(client_sock, (struct sockaddr *) &client_address, &addr_size);
    
    // If the authentication fails, the user will be allowed to make 2 more attempts (overall 3 attempts)
    int attempts =3;
    while(1){
    // referred from Beej's guide
    if(send(client_sock, "authenticate", strlen("authenticate"), 0) < 0){
        printf("Could not send message to main server\n");
        return -1;
    }
    
    // Get input(username and password) from the user
    while(1) {
    int flag=0, len_flag=0;
    printf("Please enter the username: ");
    scanf("%[^\n]%*c",username);
    // Checks if the username contains uppercase characters and if the length of username is (5~50) chars
    for (int i=0;i<strlen(username);i++) {
        if(isupper(username[i])) {
            flag=1;
            printf("The username can contain only lowercase letters. Please enter again.\n");
            break;
        }
    }
    if((strlen(username)<5)||(strlen(username)>50)) {
            len_flag=1;
            printf("The username must be between 5 and 50 letters.\n");
    }
    // Get input from the user:
    printf("Please enter the password: ");
    scanf("%[^\n]%*c",password);
    if((strlen(password)<5)||(strlen(password)>50)) {
            len_flag=1;
            printf("The password must be between 5 and 50 letters.\n");
    }
    if(flag==0 && len_flag==0){
    break;
    }
    }

    printf("%s sent an authentication request to the main server.\n", username);
    strcpy(username1, username);
    
    strcat(strcat(username, "++++++++++"),password);
    // Send the credentials to the Main server
    if(send(client_sock, username, strlen(username), 0) < 0){
        printf("Could not send message to main server\n");
        return -1;
    }
    memset(server_m_message,'\0',sizeof(server_m_message));
    // Receive the Main server's response
    if(recv(client_sock, server_m_message, sizeof(server_m_message), 0) < 0){
        printf("Could not receive main server's message\n");
        return -1;
    }
    
    printf("%s received the result of authentication using TCP over port %u. %s\n", username1, ntohs(client_address.sin_port), server_m_message);
    if ((strcmp(server_m_message, FAIL_NO_USER) == 0) || (strcmp(server_m_message, FAIL_PASS_NO_MATCH) == 0)) {
        attempts--;
        printf("Attempts remaining:%d\n", attempts);
    }
    if (attempts==0){
        printf("Authentication Failed for 3 attempts. Client will shut down.\n");
        break;
    }

    // Upon successful authentication, single course query with category or multiple course queries can be made
    if (strcmp(server_m_message, PASS) == 0) {
        while (1) {
        if(send(client_sock, "repeated query", strlen("repeated query"), 0) < 0) {
            printf("Could not send message to main server\n");
            return -1;
        }
        printf("Please enter the course code to query: ");
        scanf("%[^\n]%*c",course_code);

        int count=0;
        char *p = strtok (course_code, " ");
        char *array[10];
        
        while (p != NULL)
        {
            array[count++] = p;
            p = strtok (NULL, " ");
        }
        if (count>1) {
            char count1[10]={'\0'};
            sprintf(count1, "%d", count);
            if(send(client_sock, count1, strlen(count1), 0) < 0) {
            printf("Could not send message to main server\n");
            return -1;
            }
            printf("%s sent a request with multiple CourseCode to the main server.\n", username1);
            printf("The client received the response from the Main server using TCP over port %u.\n", ntohs(client_address.sin_port));
            printf("CourseCode: Credits, Professor, Days, Course Name\n");
            sleep(0);
            for (int i=0;i<count;i++) {
                if(send(client_sock, array[i], strlen(array[i]), 0) < 0) {
                    printf("Could not send message to main server\n");
                    return -1;
                }
            sleep(1);
            memset(server_m_message,'\0',sizeof(server_m_message));
            // Receive the main server's response:
            if(recv(client_sock, server_m_message, sizeof(server_m_message), 0) < 0){
            printf("Could not receive main server's message\n");
            return -1;
            }
            printf("%s\n", server_m_message);
            }
            sleep(1);
            
        }
        else {
        if(send(client_sock, "single query", strlen("single query"), 0) < 0) {
            printf("Could not send message to main server\n");
            return -1;
        }
        while(1) {
        printf("Please enter the category (Credit / Professor / Days / CourseName): ");
        scanf("%[^\n]%*c",category);
        // Error will be prompted if the category is not entered correctly (it is not case sensitive but it is spelling and space sensitive)
        if((strcasecmp(category, "Credit")==0) || (strcasecmp(category, "Professor")==0) ||
        (strcasecmp(category, "Days")==0) || (strcasecmp(category, "CourseName")==0)) {
            break;
        }
        else {
            printf("Please enter the category (Credit / Professor / Days / CourseName). Please re-check the spelling and space(for CourseName).\n");
        }
        }
        if(send(client_sock, course_code, strlen(course_code), 0) < 0) {
            printf("Could not send message to main server\n");
            return -1;
        }
        sleep(0);
        
        if(send(client_sock, category, strlen(category), 0) < 0) {
            printf("Could not send message to main server\n");
            return -1;
        }
        printf("%s sent a request to the main server.\n", username1);
    

    memset(server_m_message,'\0',sizeof(server_m_message));
    // Receive the main server's response
    if(recv(client_sock, server_m_message, sizeof(server_m_message), 0) < 0){
        printf("Could not receive main server's message\n");
        return -1;
    }
    printf("The client received the response from the Main server using TCP over port %u.\n", ntohs(client_address.sin_port));
    char cmp[50];
    memset(cmp,'\0',sizeof(cmp));
    strcat(cmp, "Didn’t find the course: ");
    strcat(cmp, course_code);
    if(strcmp(server_m_message, cmp)==0) {
        printf("%s\n",server_m_message);
    } else {
    printf("The %s of %s is %s.\n", category, course_code, server_m_message);}
    }
    printf("\n-----Start a new request-----\n");
    }
    }
    }

    // Close the socket:
    close(client_sock);
    
    return 0;
}