#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#define TAM 2000

#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDCYAN    "\033[1m\033[36m"
#define BLUE    "\033[34m"      /* Blue */
#define RESET   "\033[0m"

//mejor no usar globales ponerlas como variable local en main
int portTCP;
char ipAddress[20];
int connectTCP(char*);
int parseConnection (char*);
void connectUDP(int );
int parseCommand(char * );

int main( int argc, char *argv[] ) {
	int condition=0;
	char login[60];
	char user[20];
	char * token;
	while(1){
		if(condition==0){
			printf("%s","login promp: " );
			fgets(login,60,stdin);
			if(parseConnection(login)){
				strcpy(user,login);
				condition=connectTCP(login);
				if (condition==0){
				printf("%s\n", "nombre de usuario y/o contraseña incorrecto");
				}
			}
		}else{
			printf("%s","password: " );
			fgets(login,60,stdin);
			token=strtok(login,"\n");
			if(connectTCP(token)){
				printf("%s\n","Comandos disponibles:\n 	*listar\n 	*descargar no_estacion\n 	*diario_precipitacion no_estacion\n 	*mensual_precipitacion no_estacion\n 	*desconectar\n" );
				break;
			}
			printf("%s\n", "nombre de usuario y/o contraseña incorrecto");


		}
	}		
	//comandos de la aplicacion
	char command[40];
	do{
		printf("%s%s@%s%s#%s ",BOLDCYAN, user,ipAddress,BLUE,RESET);
		fgets(command,40,stdin);
		token=strtok(command,"\n");

		
		connectTCP(command);
		//}
	}while(strcmp(command,"fin\n"));
	
}
int parseCommand(char * command){
/**
@brief Chequea rapidamente que el usuario no introduzca un comando incorrecto.
@param command Comando que sera parseado por la funcion.
**/
	char* token;
	token=strtok(command," \n");	
	if (!strcmp(token,"listar")||!strcmp(token,"desconectar")||!strcmp(token,"descargar")||!strcmp(token,"diario_precipitacion")||!strcmp(token,"mensual_precipitacion")||!strcmp(token,"promedio")){
		return 1;
	}else{
		return 0;
	}


}
int parseConnection (char* connection){
/**
@brief Parsea el comando de conexion, para poder enviar el usuario al servidor y armar el socket.
@param connection Comando que sera parseado por la funcion.
**/
	char * token;	
	char user[20];
	if(!strcmp(connection,"\n")){
	printf("Comando de conexion incorrecto \n");
		return 0;
	}
	token=strtok(connection," @:\n");
	if(!strcmp(token,"connect")){
	token=strtok(NULL," @:\n");
	strcpy(user,token);
	token= strtok(NULL," @:\n");
	strcpy(ipAddress,token);
	token=strtok(NULL," @:\n");
	portTCP= atoi(token); 
	strcpy(connection,user);
	return 1;
	}else{
	printf("Comando de conexion incorrecto \n");
	return 0;
	}
}

int connectTCP(char* data){
	
/**
@brief Crea una conexion a un socket TCP, y envia los datos recibidos.
@param data Datos que seran enviados por el socket.
@returns Devuelve 1 cuando la autenticacion fue correcta y cero sino lo fue.
**/
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[TAM];
	
	
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd < 0 ) {
		perror( "ERROR apertura de socket" );
		exit( 1 );
	}

	server = gethostbyname( ipAddress );
	if (server == NULL) {
		fprintf( stderr,"Error, no existe el host\n" );
		exit( 0 );
	}
	memset( (char *) &serv_addr, '0', sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length );
	serv_addr.sin_port = htons( portTCP );
	if ( connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0 ) {
		perror( "conexion" );
		exit( 1 );
	}
		n = write( sockfd, data, strlen(data) );
		if ( n < 0 ) {
			perror( "escritura de socket" );
			exit( 1 );
		}

		if( !strcmp( "desconectar", data ) ) {
					printf( "Desconectando del servidor..\n" );
					exit(0);
				}

		memset( buffer, '\0', TAM );
		n = read( sockfd, buffer, TAM );
		if ( n < 0 ) {
			perror( "lectura de socket" );
			exit( 1 );
		}
		printf("%s\n",buffer );
		if( !strcmp( "Descargando..", buffer ) ) {
			n = write( sockfd, "ok", 3 );
			if ( n < 0 ) {
				perror( "escritura en socket" );
				exit( 1 );
			}
			memset( buffer, '\0', 5 );
			n = read( sockfd, buffer, 4 );
			if ( n < 0 ) {
				perror( "lectura de socket" );
				exit(1);
			}
			int portUDP;
			portUDP=atoi(buffer);
			n = write( sockfd, buffer, strlen(buffer) );
			if ( n < 0 ) {
				perror( "escritura en socket" );
				exit( 1 );
			}
			
			connectUDP(portUDP);
			
			
		}
		
		if(!strcmp(buffer,"clave correcta")||!strcmp(buffer,"usuario existente")){
			return 1;
		}else{
			return 0;
		}
}
void connectUDP(int portUDP){
/**
@brief Se conecta y administra un socket sin conexion, para recibir un archivo.
@param portUDP puerto que se utiliza para conectarse al socket.
**/
	sleep(1);
	int sockfd, n, tamano_direccion;
	struct sockaddr_in dest_addr;
	struct hostent *server;
	char buffer[TAM];


    //creo un socket de inet y UDP (SOCK_DGRAM)
    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
    if (sockfd < 0) { 
        perror("ERROR en apertura de socket");
        exit( 1 );
    }
    server = gethostbyname( ipAddress);
	if ( server == NULL ) {
		fprintf( stderr, "ERROR, no existe el host\n");
		exit(0);
	}

    dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons( portUDP );
	dest_addr.sin_addr = *( (struct in_addr *)server->h_addr );
	memset( &(dest_addr.sin_zero), '\0', 8 );


    tamano_direccion = sizeof( dest_addr );
    int end=0;

    FILE *file;
    //creaba un problema al abrir y escribir directamente asi que abro como lectura para borrarlo y abro append
    file=fopen("cliente.CSV","w");
    fclose(file);
    file=fopen("cliente.CSV","a");
   	while ( end==0 ) {
       	memset( buffer, 0, TAM );
       	n = sendto( sockfd, (void *)"Envie mensaje", 14, 0, (struct sockaddr *)&dest_addr, tamano_direccion );
		if ( n < 0 ) {
			perror( "Escritura en socket" );
			exit( 1 );
		}

       n = recvfrom( sockfd, (void *)buffer, TAM, 0, (struct sockaddr *)&dest_addr, &tamano_direccion );
		if ( n < 0 ) {
			perror( "Lectura de socket" );
			exit( 1 );
		}

		//printf("%s\n",buffer );
        if(!strcmp(buffer,"end")){
        	end=1;
        	//continue;
        }
        fprintf(file, "%s", buffer);

      	

    }
    fclose(file);
}