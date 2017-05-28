
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void download(int ,char* );
void average(int ,char * );
void precipitation_diary(int ,int ,char * );
int CompareDate(char * , char *  ,int );
int getMonth(char * );
int getDay(char * );
int getYear(char * );
void getData(int , char * , char * );
int main(void)
{
char serverAnswer[2000];
char *data;
int function, variable,station;

printf("%s%c%c\n","Content-Type:text/html;charset=UTF-8",13,10);
printf("<TITLE>AWS SERVER</TITLE>\n");
data = getenv("QUERY_STRING");

if(data == NULL)
  printf("<P>Error! Error in passing data from form to script.");
else if(sscanf(data,"station=%d&function=%d&variable=%d",&station,&function,&variable)!=4)

printf("<body>");
switch(function){

case 0:
	average(variable,serverAnswer);
	break;
case 1:
	precipitation_diary(station,1,serverAnswer);
	break;
case 2:
	precipitation_diary(station,0,serverAnswer);
	break;
case 3:
	download(station,serverAnswer);
	break;
default:
	break;
}
printf("%s",serverAnswer );
printf("</body>");
return 0;
}
void average(int variable,char * serverAnswer){
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
	FILE *file;
	char temporal[500];
	char * data;
	char dataRegister[20];
	file=fopen("/var/www/html/files/datos_meteorologicos.CSV","r");
		fgets (temporal,500,file);
		fgets (temporal,500,file);
		fgets (temporal,500,file);
	while (!feof(file)) { 
		fgets (temporal,200,file);
		data=strdup(temporal);
		getData(0,data,dataRegister);
		newStation=atoi(dataRegister);
        if(newStation==station){
        	getData(variable,data,dataRegister);
			if(!strcmp(dataRegister,"--")){	
				active=0;
		    }else{
		       	active=1;
		    }
			getData(variable,data,dataRegister);
			average+=atof(dataRegister);
			counter++;
       }else{
       		
       		average=average/counter;       
       		if (station!=0 && active==1){
       			sprintf(temporal,"<p>En la estacion %d el promedio pedido es %f</p>",station,average );
       			strcat(serverAnswer,temporal);
       		}else if(station!=0 && active==0){
       			sprintf(temporal,"<p>En la estacion %d no hay datos para el promedio pedido</p>",station );
       			strcat(serverAnswer,temporal);
       		}
       		station=newStation;
       		average=0;
    	}
	}
    average=average/counter; 
	if (station!=0 && active==1){
       	sprintf(temporal,"<p>En la estacion %d el promedio pedido es %f</p>",station,average );
       	strcat(serverAnswer,temporal);
    }else if(station!=0 && active==0){
       	sprintf(temporal,"<p>En la estacion %d no hay datos para el promedio pedido</p>",station );
       	strcat(serverAnswer,temporal);
    }

}
void download(int station,char* serverAnswer){
/**
@brief Crea un archivo "CVS" con los datos de los sensores de la estacion pedida.
@param station numero de estacion de la cual se quieren obtener datos.
**/
	FILE *file;
	FILE *sendFile;
	char temporal[200];	
	char dataRegister[20];
	char *data;
	file=fopen("/var/www/html/files/datos_meteorologicos.CSV","r"); 
    sendFile=fopen("/var/www/html/files/temporal.CSV","w");
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
   strcpy(serverAnswer,"<p><a href=\"/files/temporal.CSV\" target=\"_blank\">Descargar!</a></p>");

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
	char *date="01/02/2017 17:00";
	char * token;
	char *data;
	char  dataRegister[20];
	file=fopen("/var/www/html/files/datos_meteorologicos.CSV","r"); 
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

        			
    				sprintf(temporal,"<p>Estacion %d acumulado del dia %d/%d/%d: %f mm </p>",station,getDay(date),getMonth(date),getYear(date),accumulated );
    				strcat(serverAnswer,temporal);
    			}else{
    				sprintf(temporal,"<p>Estacion %d acumulado del mes %d/%d: %f mm </p>",station,getMonth(date),getYear(date),accumulated );
    				strcat(serverAnswer,temporal);
    			}
    			accumulated=0;
    			date=strdup(newDate);

    			
    		}
    	}
    } 
    if(daily){   			
    //imprimo el ultimo dia, como compara con el siguiente nunca entraria en else sino
    sprintf(temporal,"<p>Estacion %d acumulado del dia %d/%d/%d: %f mm </p>",station,getDay(date),getMonth(date),getYear(date),accumulated );
    strcat(serverAnswer,temporal);
	}else{
    sprintf(temporal,"<p>Estacion %d acumulado del mes %d/%d: %f mm </p>",station,getMonth(date),getYear(date),accumulated );
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