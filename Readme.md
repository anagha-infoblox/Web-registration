COMPUTER NETWORKING

SOCKET PROGRAMMING PROJECT, FALL 2022

NAME:	ANAGHA HAREESH

FILES INCLUDED:

	1.	client.c
	2.	serverM.c
	3.	serverC.c
	4.	serverCS.c
	5.	serverEE.c
	6.	Makefile
	7.	Readme.md

Note:	Executables are not included as instructed by the Problem Statement.
		'make' or 'make all' command in the terminal generates the executables required.

Note:	Please read the NOTEs as they contain vital information about the overall flow of the project. 

-------------------------------------------------------------------------------

What I have done in the Project:

1. I have IMPLEMENTED the optional part for the extra credit.

2. Code Files and their Description:
	
	client.c:

			In the client.c program file, I have taken inputs of 'Username' & 'Password' as STRING type.

			They are then compared with the CONSTRAINTS as prescribed by the Problem Statement. 

			>>Username has to be of lower case and 5-50 chars long. Failing which the client will prompt to re-enter the username that is 5~50 characters long. Number of attempts are not reduced in this case as it is related to length of the username.
			>>Password has to be between 5-50 chars long. Failing which the client will prompt to re-enter the password that is 5~50 characters long. Number of attempts are not reduced in this case as it is related to length of the password.

			The Username and Password combo are sent to the Main server for Authentication.

			The Result of Authentication is received as:

			1.	"PASS"				 : stating the fact that the Username and Password combination are correct and it is encoded as 'PASS' for further processes.
			2.	"FAIL_NO_USER"		 : stating the fact that the Username was wrong and it is encoded as 'FAIL_NO_USER' for further process. The number of tries is reduced by 1 in this case.
			3.	"FAIL_PASS_NO_MATCH" : stating the fact that the Password was wrong and it is then encoded as 'FAIL_PASS_NO_MATCH' for further process. The number of tries is reduced by 1 in this case.

			If all 3 attempts have completed with unsuccessful Authentication, the client SHUTS DOWN.

			If the user has passed the Authntication, we proceed to make either single query by passing the Course Code and Category or query for multiple course codes(extra credit). The limit is the user can query upto 10 course codes as mentioned in the constraints.

			Course Code and Category are received as STRINGS. The requested information is sent to the Main server for query. 

			The Information of the Query requested is then Displayed. 

            For Multiple course queries(extra credit):
            When the user passes multiple course codes, the client iterates through the entered value and 'count' variable is used to hold the number of course codes entered by the user. The 'count' and the course codes are passed to the main server and all the details obtained are displayed. Upon displaying details of the multiple course codes entered by the user, the client prompts for a new request.

			NOTE 1:	The client later prompts for starting a new request. The client can only be terminated by entering Ctrl+C. A prompt for new request will be prompted unless and until Ctrl+C is pressed to terminate.
            NOTE 2: The Course code is case sensitive. For example, if the user enters ee450 instead of EE450, the user will obtain the error "Didn't find the course: ee450" as it is case sensitive.

	
	serverM.c:

			The Main server firstly receives the Username and Password sent by the client. 

			The Password and Username are then encrypted as instructed by the problem statement. I have done it using ASCII values, after obtaining the ASCII value for each of the characters:

            A check is made if the ASCII value is between 48 and 57(0~9). If yes, the numbers are encoded by adding an offset value of 4. 
            Following this, a check is made to check if the ASCII value of the character exists between 65 and 86(A~V) or 97 and 118(a~v). If yes, the alphabets are encoded by adding 4 to the ASCII value.
            Following this, a check is made to check if the ASCII value of the character exists between 87 and 90(W~Z) or 119 and 122(w~z). If yes, the alphabets are encoded by subtracting 22 to the ASCII value as W~Z or w~z requires encoding from A~D or a~d respectively.
            The specials characters are not encoded as mentioned in the constarints of the encryption algorithm.

			The encrtpted Username and encrypted Password is sent as UDP Datagram to serverC, and the result is received as a STRING and compared with the macros defined(FAIL_NO_USER, FAIL_PASS_NO_MATCH, PASS)

			The coursecode and Category are then received from the client as STRINGS.

			The first two letters in the coursecode (EE or CS) are found and compared. If 'EE', it is re-directed to serverEE and if 'CS', it is re-directed to serverCS. If the Courses are not from 'CS' or 'EE' then an error message is desplayed. Thus, the Query Request is sent to Appropriate servers and the Information is received as STRINGS.

			The information is Sent to Client.

            For Multiple course queries(extra credit):
            When the user passes multiple course codes, the client passes the individual codes to the serverM, the serverM re-directs to the respective EE or CS servers. Uopn obtaining the response from the departmental servers, serverM sends back the response with all the other details of the course code to the client.


			NOTE: The serverM does not shut down until the user manually enters Ctrl+C. Upon termination of the client, the connection is teared down and the child socket is closed. The serverM continues to listen and ready to accept new client connections. To terminate serverM, Ctrl+C has to be entered,

	
	serverC.c:

			memset was used to clear the strings as lots of garbage character was being retained and caused errors in code.

			The server receives the Encrypted username and Encrypted password from the serverM. Here the UDP server receives the following:
				1. encrypted_username[50] - maximum 50 chars (STRING)
				2. encrypted_password[50] - maximum 50 chars (STRING)

			The 'str' variable is used to store the values of the lines obtained upon parsing the cred.txt file. It can parse upto 500 credentials. 
            The credentials stored in 'cred.txt' file are extracted from the 'str' variable and stored in the variables: 'username' and 'password'. These are compared with the 'encrypted_username' and 'encrypted_password'. 
			

			It checks for the following criterias:
				1. FAIL_NO_USER 	  - username does not exist
				2. FAIL_PASS_NO_MATCH - password not matched
				3. PASS               - both username and password matched.

			The Credential server first matches the username, if matched, it checks if the Password is also Matched. If both the criterias are matched, it passes the "PASS" authentication result. If not it sends the appropriate reason for authentication failure to serverM.

			I then send the result which is a STRING used to store the authentication result back to the Main server to continue with the process.

			NOTE: The serverC (Credential server) does not shut down until the user enters Ctrl+C.

	serverEE/serverCS:

			Both serverCS and serverEE contain the EXACT same code with the naming changes to depict 'CS' and 'EE' departments. Their functions REMAIN THE SAME.

			memset was used to clear the strings as lots of garbage character was being retained and caused errors in code.

			I used a structure called 'EECourseDetails' (or CSCourseDetails) to store the values of the "ee.txt" (or cs.txt). The structre is capable of holding upto 500 courses and their information.
			The structure's attributes are:
				1. CourseCode[500][100]	(STRING)
				2. Prof[500][100]		(STRING)
				3. Days[500][100]		(STRING)
				4. CourseName[500][200]	(STRING)
				5. Credits[500][100] 	(STRING)

			The same structure is used for both 'CS' and 'EE' files.

			strtok() is a function used to tokenize a given length of string till a specified character. In our case a comma ','.

			The Coursecode and Category are received firstly, as STRINGS. Initially the Coursecode is compared. If the coursecode exists, then the categories are compared to pass appropriate information the the main server. Thus the Program searches the requested information by matching the coursecode with the requested coursecode.

			If the coursecode doesnt Match an error is sent to the Main server else the actual Information requested is sent to the main server.

            For Multiple course queries(extra credit):
            When the user passes multiple course codes, the serverM sends the UDP datagram with the course code and the category option set to 'all details' to the appropriate departmental servers. Since, the category is 'all details', the server sends a message to the server with all the necessary details(Credit, Professor, Days and CourseName) of the course code to the main server.

			NOTE: The serverEE and serverCS does not shut down until the user enters Ctrl+C.


	Makefile:

			It is a file containing the compiling codes of all the files, It creates an executable as mentioned by the Problem Statement.
			Command: 'make all'


3.	Idiosyncrasy:
	
			After complete checking of the Project, I'm Happy to inform that the project doesn't fail under any conditions, however the Coursecode has to be entered MAINTAINING case sensitivity. The categories have to be entered correctly(it is not case sensitive but spaces and spelling must be entered correctly). For example, coursename or CourseName or CourseName can be entered. But if 'course name'(with a space) is entered, an error will be prompted to re-check the spelling and spaces in the entered information. 


4.	Reused Code:
			
			1.	No code was reused from any project on the internet/copied.
			2.	Syntaxes for UDP and TCP connections were referred from Beej's Handout.
			3.	Additional usage of Pre-defined functions are mentioned in the Readme file as well as commented in the code.

Thanks For reviewing my Project!

----------------------------------------------------------------------------
