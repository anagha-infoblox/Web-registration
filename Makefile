CC=gcc  #compiler
SERVERM=serverM
SERVERC=serverC
SERVERCS=serverCS
SERVEREE=serverEE
CLIENT=client


all:
	$(CC) -o $(SERVERM) serverM.c
	$(CC) -o $(SERVERC) serverC.c
	$(CC) -o $(SERVERCS) serverCS.c
	$(CC) -o $(SERVEREE) serverEE.c
	$(CC) -o $(CLIENT) client.c