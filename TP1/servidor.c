#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#define TAM 2000

void parseRequest(char* ,char* );
void average(char * ,char *);
void list(char *);
void download(int );
int CompareDate(char * , char * ,int );
int getDay(char * );
int getMonth(char * );
int getYear(char * );
void getData(int , char * , char * );
void precipitation_diary(int ,int,char * );
int parseVariable(char *);
void sendUDP(int );
void getColumnName(int ,char * );

int main( int argc, char *argv[] ) {

	int sockfd, newsockfd, clilen, pid; //solo sockfd es compartido y cilen
	char buffer[TAM]; //de aca
	struct sockaddr_in serv_addr, cli_addr; //cli addr es usado como puntero
	int n; 

	sockfd = socket( AF_INET, SOCK_STREAM, 0);
	if ( sockfd < 0 ) { 
		perror( " apertura de socket ");
		exit( 1 );
	}

	memset( (char *) &serv_addr, 0, sizeof(serv_addr) );
	//cargo el struct con las los datos necesarios para crear el socket
	//puerto = atoi( 6060);
	serv_addr.sin_family = AF_INET; // familia de direcciones que usa el socket unix o inet por ej
	serv_addr.sin_addr.s_addr = INADDR_ANY; //se usa para linkear el socket a todas las interfaces de la pc
	serv_addr.sin_port = htons( 6020 );

	//asigna la direccion al socket
	if ( bind(sockfd, ( struct sockaddr *) &serv_addr, sizeof( serv_addr ) ) < 0 ) {
		perror( "ligadura" );
		exit( 1 );
	}

    printf( "Proceso: %d - socket disponible: %d\n", getpid(), ntohs(serv_addr.sin_port) );

	//pone al socket como socket server para q escuche conexiones
	listen( sockfd, 5 );
	clilen = sizeof( cli_addr );

	//proceso de escuchar y escribir en el socket

	while( 1 ) { 
		//crea un nuevo socket q tiene el primer dato en la cola
		newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );
		if ( newsockfd < 0 ) {
			perror( "accept" );
			exit( 1 );
		}
		pid=fork();
		if(pid<0){
			perror("fork");
			exit(1);
		}

		if ( pid == 0 ) {  // Proceso hijo
			close( sockfd );

			while ( 1 ) {
				//se queda escuchando a newfdsocket
				memset( buffer, 0, TAM );

				n = read( newsockfd, buffer, TAM-1 );
				if ( n < 0 ) {
					perror( "lectura de socket" );
					exit(1);
				}
				if( !strcmp( "desconectar", buffer ) ) {
					printf( "PROCESO %d. Como recibí 'fin', termino la ejecución.\n\n", getpid() );
					exit(0);
				}

				printf( "PROCESO %d. ", getpid() );
				printf( "Recibí:%s%s\n", buffer ,"..");
				char serverAnswer[TAM];
				parseRequest(buffer,serverAnswer);

				n = write( newsockfd, serverAnswer, strlen(serverAnswer) );
				if ( n < 0 ) {
					perror( "escritura en socket" );
					exit( 1 );
				}
				if (!strcmp(serverAnswer,"Descargando..")){	
				n = read( newsockfd, buffer, TAM );
				if ( n < 0 ) {
					perror( "lectura de socket" );
					exit( 1 );
				}if(!strcmp(buffer,"ok")){
				printf("AUN NO ENVIE \n");
				sleep(1);
				char  buffy[5];
				int portUDP=2300+(random()%1000);
				sprintf(buffy,"%d",portUDP);
				printf("%s\n",buffy );
					n = write( newsockfd, buffy, strlen(buffy) );
				if ( n < 0 ) {
					perror( "escritura de socket" );
					exit( 1 );
				}
				printf("ENVIE \n");
				n = read( newsockfd, buffer, TAM );
				if ( n < 0 ) {
					perror( "lectura de socket" );
					exit( 1 );
				}
				printf("LARGO UDP \n");

				sendUDP(portUDP);
				}
				}
			}
		}
		else {
			//sigue escuchando a  fdsocket
			printf( "SERVIDOR: Nuevo cliente, que atiende el proceso hijo: %d\n", pid );
			close( newsockfd );
		}
	}
	return 0; 
} 
void parseRequest(char* request,char* serverAnswer){	
	/**
	@brief Parsea los commandos enviados por el cliente y luego de ser procesados por la funcion correspondientes devuelve la respuesta que luego sera enviada al cliente.
	@param request contiene el commando enviado por el ciente.
	@param serverAnswer es la respuesta que enviara el servidor al cliente.
	**/
	char *buffer;

    buffer =strsep(&request," \n");
    request=strsep(&request,"\n");
	if(!strcmp(buffer,"juanmadelboca")){	
		strcpy(serverAnswer,"usuario existente");
	}else if(!strcmp(buffer,"sistemas")){	
		strcpy(serverAnswer,"clave correcta");
	}else if(!strcmp(buffer,"listar")){	
		list(serverAnswer);
	}else if(!strcmp(buffer,"descargar")){
		download(atoi(request));	
		strcpy(serverAnswer,"Descargando..");
	}else if(!strcmp(buffer,"diario_precipitacion")){
		precipitation_diary(atoi(request),1,serverAnswer);	
	}else if(!strcmp(buffer,"mensual_precipitacion")){	
		precipitation_diary(atoi(request),0,serverAnswer);
	}else if(!strcmp(buffer,"promedio")){	
		average(request,serverAnswer);
	}else {	
		strcpy(serverAnswer,"Error");
									
	}
}
void getColumnName(int column,char * columnName){
	/**
	@brief Devuelve el nombre real de un sensor a partir del valor numerico que este tiene asignado.
	@param columnName nombre real del sensor.
	@param column numero del sensor que busco conocer.
	**/
	
	switch(column){
	case 4:
		strcpy(columnName,"Temperatura");
		break;
	case 5:
		strcpy(columnName,"Humedad");
		break;
	case 6:
		strcpy(columnName,"Punto de Rocio");
		break;
	case 7:
		strcpy(columnName,"Precipitacion");
		break;
	case 8:
		strcpy(columnName,"Velocidad Viento");
		break;
	case 9:
		strcpy(columnName,"Direccion Viento");
		break;
	case 10:
		strcpy(columnName,"Rafaga Maxima");
		break;
	case 11:
		strcpy(columnName,"Presion");
		break;
	case 12:
		strcpy(columnName,"Radiacion Solar");
		break;
	case 13:
		strcpy(columnName,"Temperatura Suelo 1");
		break;
	case 14:
		strcpy(columnName,"Temperatura Suelo 2");
		break;
	case 15:
		strcpy(columnName,"Temperatura Suelo 3");
		break;
	case 16:
		strcpy(columnName,"Humedad Suelo 1");
		break;
	case 17:
		strcpy(columnName,"Humedad Suelo 2");
		break;
	case 18:
		strcpy(columnName,"Humedad Suelo 3");
		break;
	case 19:
		strcpy(columnName,"Humedad de Hoja");
		break;
	default:
		break;
	}
}
void average(char * variable,char * serverAnswer){
/**
@brief Calcula el promedio de los datos de un sensor en todas las estaciones disponibles.
@param columnName valor recibido, nombre del sensor que se usara
@param column numero asignado a dicho sensor
**/
	float average=0;
	int counter=0;
	int station=0;
	int active=0;
	int newStation=0;
	int datum=parseVariable(variable);
	FILE *file;
	char temporal[500];
	char * data;
	char dataRegister[20];
	file=fopen("datos_meteorologicos.CSV","r");
		fgets (temporal,500,file);
		fgets (temporal,500,file);
		fgets (temporal,500,file);
	while (!feof(file)) { 
		fgets (temporal,200,file);
		data=strdup(temporal);
		getData(0,data,dataRegister);
		newStation=atoi(dataRegister);
        if(newStation==station){
        	getData(datum,data,dataRegister);
			if(!strcmp(dataRegister,"--")){	
				active=0;
		    }else{
		       	active=1;
		    }
			getData(datum,data,dataRegister);
			average+=atof(dataRegister);
			counter++;
       }else{
       		
       		average=average/counter;       
       		if (station!=0 && active==1){
       			sprintf(temporal,"En la estacion %d el promedio pedido es %f\n",station,average );
       			strcat(serverAnswer,temporal);
       		}else if(station!=0 && active==0){
       			sprintf(temporal,"En la estacion %d no hay datos para el promedio pedido\n",station );
       			strcat(serverAnswer,temporal);
       		}
       		station=newStation;
       		average=0;
    	}
	}
    average=average/counter; 
	if (station!=0 && active==1){
       	sprintf(temporal,"En la estacion %d el promedio pedido es %f\n",station,average );
       	strcat(serverAnswer,temporal);
    }else if(station!=0 && active==0){
       	sprintf(temporal,"En la estacion %d no hay datos para el promedio pedido\n",station );
       	strcat(serverAnswer,temporal);
    }

}

int parseVariable(char * variable){
/**
@brief Devuelve el valor numerico el cual tiene asignado el sensor que recibe.
@param variable valor recibido, nombre del sensor que se usara.
@returns numero asignado a dicho sensor.
**/
	if(!strcmp(variable,"Temperatura")){	
		return 4;
	}else if(!strcmp(variable,"Humedad")){	
		return 5;
	}else if(!strcmp(variable,"Punto de Rocio")){	
		return 6;
	}else if(!strcmp(variable,"Precipitacion")){	
		return 7;
	}else if(!strcmp(variable,"Velocidad Viento")){	
		return 8;
	}else if(!strcmp(variable,"Direccion Viento")){	
		return 9;
	}else if(!strcmp(variable,"Rafaga Maxima")){	
		return 10;
	}else if(!strcmp(variable,"Presion")){	
		return 11;
	}else if(!strcmp(variable,"Radiacion Solar")){	
		return 12;
	}else if(!strcmp(variable,"Temperatura Suelo 1")){	
		return 13;
	}else if(!strcmp(variable,"Temperatura Suelo 2")){	
		return 14;
	}else if(!strcmp(variable,"Temperatura Suelo 3")){	
		return 15;
	}else if(!strcmp(variable,"Humedad Suelo 1")){	
		return 16;
	}else if(!strcmp(variable,"Humedad Suelo 2")){	
		return 17;
	}else if(!strcmp(variable,"Humedad Suelo 3")){	
		return 18;
	}else if(!strcmp(variable,"Humedad de Hoja")){	
		return 19;
	}else{
		return 4;
	}
}

void list(char * serverAnswer){
/**
@brief Devuelve una lista con las estaciones disponibles y todos sus sensores activos.
@param serverAnswer puntero a char por donde se devuelve el la lista.
**/
	FILE *file;
	char temporal[200];
	char dataRegister[20];
	char *data;
	char string[200];
	int station=0;
	file=fopen("datos_meteorologicos.CSV","r"); 
	while (!feof(file)) {
		fgets (temporal,200,file);
		data=strdup(temporal);
		getData(0,data,dataRegister);
        if(atoi(dataRegister)!=station){

	       	station =atoi(dataRegister);
	        sprintf(string,"Estacion %d,sensores activos: ",station);
	       	strcat(serverAnswer,string);
	        for (int i = 4; i < 20; ++i){
	        	getData(i,data,dataRegister);
				if(strcmp(dataRegister,"--")){
		        	getColumnName(i,dataRegister);
		        	strcat(serverAnswer,dataRegister);
		        	strcat(serverAnswer,",");			
		        }
		    }
			serverAnswer[strlen(serverAnswer)-1] = '\n';
       }
    }
    fclose(file);
}

void download(int station){
/**
@brief Crea un archivo "CVS" con los datos de los sensores de la estacion pedida.
@param station numero de estacion de la cual se quieren obtener datos.
**/
	FILE *file;
	FILE *sendFile;
	char temporal[200];	
	char dataRegister[20];
	char *data;
	file=fopen("datos_meteorologicos.CSV","r"); 
    sendFile=fopen("temporal.CSV","w");
	while (!feof(file)) {
        
		fgets (temporal,200,file);
		data=strdup(temporal);
		getData(0,data,dataRegister);
        if(atoi(dataRegister)==station){
        	
        	fprintf(sendFile, "%s", temporal);
       }
    }
    fclose(sendFile);
    fclose(file);
    //aca envio los datos
}
void precipitation_diary(int station,int daily,char * serverAnswer){
/**
@brief Devuelve una lista de acumulados de precipitacion, diarios o mensuales.
@param station numero de estacion de la cual se quieren obtener datos.
@param diario valor que define si sera precipitacion diaria (1) o mensual (0).
@param serverAnswer puntero a char por donde se devuelve el resultado para luego ser enviado por el socket.
**/
	FILE *file;
	char temporal[200];
	float accumulated=0.0;
	char * newDate;
	char *date="00/02/2017 17:00";
	char * token;
	char *data;
	char  dataRegister[20];
	file=fopen("datos_meteorologicos.CSV","r"); 
	while (!feof(file)) {
        fgets (temporal,200,file);
        data=strdup(temporal);
        token =strsep(&data,",");
        if(atoi(token)==station){
        	getData(2,data,dataRegister);
        	newDate=strdup(dataRegister);
        	if(CompareDate(date,newDate,daily)){
        		getData(6,data,dataRegister); 	
        		accumulated+=atof(dataRegister);
    		}else{       	
    			if(daily){

        			
    				sprintf(temporal,"Estacion %d acumulado del dia %d/%d/%d: %f mm\n",station,getDay(date),getMonth(date),getYear(date),accumulated );
    				strcat(serverAnswer,temporal);
    			}else{
    				sprintf(temporal,"Estacion %d acumulado del mes %d/%d: %f mm\n",station,getMonth(date),getYear(date),accumulated );
    				strcat(serverAnswer,temporal);
    			}
    			accumulated=0;
    			date=strdup(newDate);

    			
    		}
    	}
    } 
    if(daily){   			
    //imprimo el ultimo dia, como compara con el siguiente nunca entraria en else sino
    sprintf(temporal,"Estacion %d acumulado del dia %d/%d/%d: %f mm\n",station,getDay(date),getMonth(date),getYear(date),accumulated );
    strcat(serverAnswer,temporal);
	}else{
    sprintf(temporal,"Estacion %d acumulado del mes %d/%d: %f mm\n",station,getMonth(date),getYear(date),accumulated );
    strcat(serverAnswer,temporal);
	}
    fclose(file);
}

int CompareDate(char * date1, char * date2 ,int dayCompare){
/**
@brief Compara dos arreglos de caracteres que contienen fechas para saber si son iguales.
@param date1 contiene la primera fecha a comparar.
@param date2 contiene la segunda fecha a comparar.
@param dayCompare un entero que al estar en cero anula la comparacion del dia.
**/
	if(getDay(date1)==getDay(date2)||dayCompare==0){
		//dia correcto
		if(getMonth(date1)==getMonth(date2)){
			//mes correcto
			if(getYear(date1)==getYear(date2)){
			//mes correcto
				return 1;
			}
		}
	}
	return 0;
}
int getDay(char * date){
/**
@brief A partir de un arreglo de caracteres,que contiene una fecha, obtiene el numero correspondiente al dia.
@param date un arreglo de caracteres,que contiene una fecha.
@returns Devuelve el valor del dia.
**/
	char * temporal;
	char * token;
	temporal=strdup(date);
	token=strtok(temporal,"/");
	return atoi(token);


}
int getMonth(char * date){
/**
@brief A partir de un arreglo de caracteres,que contiene una fecha, obtiene el numero correspondiente al mes.
@param date un arreglo de caracteres,que contiene una fecha.
@returns Devuelve el valor del mes.
**/
	char * temporal;
	char * token;
	temporal=strdup(date);
	token=strtok(temporal,"/");
	token=strtok(NULL,"/");
	return atoi(token);
}
int getYear(char * date){
/**
@brief A partir de un arreglo de caracteres,que contiene una fecha, obtiene el numero correspondiente al año.
@param date un arreglo de caracteres,que contiene una fecha.
@returns Devuelve el valor del año.
**/
	char * temporal;
	char * token;
	temporal=strdup(date);
	token=strtok(temporal,"/");
	token=strtok(NULL,"/");
	token=strtok(NULL,"/");
	return atoi(token);
}

void getData(int column, char * data, char * result){
/**
@brief Devuelve el dato que corresponde a la coliumna pedida, cada columna estando delimitada por una coma..
@param column Columna de dato a la cual quiero acceder.
@param data Arreglo de caracteres en el cual busca la columna indicada.
@param result puntero a char por donde devuelve valor del dato de la columna pedida.

**/
	char * temporal;
	char *token;
	temporal=strdup(data);
	token=strtok(temporal,",");
	for (int i = 0; i <column; ++i)
    {
   		token=strtok(NULL,",");
    }        

	strcpy(result,token);
}
void sendUDP(int portUDP){
/**
@brief Crea y administra un socket sin conexion, para enviar un archivo.
@param portUDP puerto que se utiliza para crear el socket.
**/
    int sockfd, tamano_direccion;
    char buffer[ TAM ];
    struct sockaddr_in serv_addr;
    int n;


	sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
	if (sockfd < 0) {
		perror( "apertura de socket" );
		exit( 1 );
	}
	memset( &serv_addr, 0, sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( portUDP );
	memset( &(serv_addr.sin_zero), '\0', 8 );

	if( bind( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0 ) {
		perror( "ERROR en binding" );
		exit( 1 );
	}
	FILE *sendFile;
	sendFile=fopen("temporal.CSV","r");
	char temporal[200];
	while (!feof(sendFile)) {

		
	tamano_direccion = sizeof( struct sockaddr );
	
	memset( buffer, 0, TAM );
		n = recvfrom( sockfd, buffer, TAM-1, 0, (struct sockaddr *)&serv_addr, &tamano_direccion );
		if ( n < 0 ) {
			perror( "lectura de socket" );
			exit( 1 );
		}

		fgets (temporal,200,sendFile);
		n = sendto( sockfd, (void *)temporal, 200, 0, (struct sockaddr *)&serv_addr, tamano_direccion  );
		if ( n < 0 ) {
			perror( "escritura en socket" );
			exit( 1 );
		}
	}

	
	fclose(sendFile);
	char end[5];
	strcpy(end,"end");
	printf(".END \n");
	n = sendto( sockfd, (void *)end, strlen(end), 0, (struct sockaddr *)&serv_addr, tamano_direccion  );
		if ( n < 0 ) {
			perror( "escritura en socket" );
		}

    close( sockfd );
}
