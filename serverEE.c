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
	
#define PORT	 23515
#define MAXLINE 1024

struct EECourseDetails {
    char course_code[500][100];
    char credit[500][100];
    char professor[500][100];
    char days[500][100];
    char course[500][200];
};

// Driver code
int main() {
	int sock_ee;
	char course_code[100], category[100];
	struct sockaddr_in eeservaddr, cliaddr;
		
	// Creating socket file descriptor for EE server, referred from Beej's guide
	if ( (sock_ee = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
	memset(&eeservaddr, 0, sizeof(eeservaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling EE Server information, referred from Beej's guide
	eeservaddr.sin_family = AF_INET; 
	eeservaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	eeservaddr.sin_port = htons(PORT);
		
	// Bind the socket with the server address, referred from Beej's guide
	if ( bind(sock_ee, (const struct sockaddr *)&eeservaddr,
			sizeof(eeservaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	printf("The ServerEE is up and running using UDP on port %d.\n", PORT);
    while(1){
	int len, n;
	
	len = sizeof(cliaddr);
	
	n = recvfrom(sock_ee, (char *)course_code, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				&len);
	course_code[n] = '\0';
	//printf("Client : %s\n", encrypted_username);

	n = recvfrom(sock_ee, (char *)category, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				&len);
	category[n] = '\0';
    printf("The ServerEE received a request from the Main Server about the %s of %s.\n", category, course_code);
	//printf("Client : %s\n", encrypted_password);
	//printf("The ServerC received an authentication request from the Main Server.\n");

	//read ee.txt file

	FILE* fptr;
	char str[500];
    struct EECourseDetails course_details;
    int count = 0, i;
	fptr = fopen("ee.txt", "r");

	if (NULL == fptr) {
		printf("file ee.txt can't be opened \n");
	}

    // check if course code exists
	while (fgets(str, 500, fptr) != NULL) {
        str[strcspn(str, "\n")] = '\0';
		strcpy(course_details.course_code[count], strtok(str , ","));
        strcpy(course_details.credit[count], strtok(NULL, ","));
        strcpy(course_details.professor[count], strtok(NULL, ","));
        strcpy(course_details.days[count], strtok(NULL, ","));
        strcpy(course_details.course[count], strtok(NULL, ","));
        count++;
	}
	//printf("count: %d\n",count);
    for (i=0;i<count;i++) {
        if(strcmp(course_code, course_details.course_code[i])==0) {
            if (strcasecmp(category, "Credit")==0) {
                printf("The course information has been found: The %s of %s is %s.\n", category, course_code, course_details.credit[i]);
                sendto(sock_ee, (const char *)course_details.credit[i], strlen(course_details.credit[i]),
	 	        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
                break;
            }
            else if (strcasecmp(category, "Professor")==0) {
                printf("The course information has been found: The %s of %s is %s.\n", category, course_code, course_details.professor[i]);
                sendto(sock_ee, (const char *)course_details.professor[i], strlen(course_details.professor[i]),
	 	        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
                break;
            }
            else if (strcasecmp(category, "Days")==0) {
                printf("The course information has been found: The %s of %s is %s.\n", category, course_code, course_details.days[i]);
                sendto(sock_ee, (const char *)course_details.days[i], strlen(course_details.days[i]),
	 	        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
                break;
            }
            else if (strcasecmp(category, "CourseName")==0) {
                printf("The course information has been found: The %s of %s is %s.\n", category, course_code, course_details.course[i]);
                sendto(sock_ee, (const char *)course_details.course[i], strlen(course_details.course[i]),
	 	        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
                break;
            }
            else if (strcmp(category, "all details")==0) {
                char details[200]={'\0'};
                strcat(details, course_details.course_code[i]);
                strcat(details, ": ");
                strcat(details, course_details.credit[i]);
                strcat(details, ", ");
                strcat(details, course_details.professor[i]);
                strcat(details, ", ");
                strcat(details, course_details.days[i]);
                strcat(details, ", ");
                strcat(details, course_details.course[i]);
                sendto(sock_ee, (const char *)details, strlen(details),
	 	        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
                break;
            }
        }
    }
    if (i==count) {
        char fail_msg[50]={'\0'};
        printf("Didn’t find the course: %s\n", course_code);
        strcat(fail_msg, "Didn’t find the course: ");
        strcat(fail_msg, course_code);
        sendto(sock_ee, (const char *)fail_msg, strlen(fail_msg),
	    MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
    }

	fclose(fptr);
	printf("The ServerEE finished sending the response to the Main Server.\n");
	}
    close(sock_ee);
	return 0;
}
