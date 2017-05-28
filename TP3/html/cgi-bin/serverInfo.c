#include <stdio.h>
#include <string.h>
#include <time.h>
struct serverData{

	char cpuType[20];
	char cpuModel[100];
	char uptime[50];
	char date[50];
	int memoryFree;
	int memoryTotal;
};

	FILE *fs;

char* upTime(char texto[],int div);
void loadInfo(struct serverData *srv);
void setWeb(struct serverData *srv);
char* search(const char searchedWord[]);
char* getTime();
int main (){

	struct serverData srv;
	loadInfo(&srv);
	setWeb(&srv);
	return 0;
}
void loadInfo(struct serverData *srv){

	fs=fopen("/proc/cpuinfo","r");
	strcpy(srv->cpuType, search("vendor_id"));
	rewind(fs);
	strcpy(srv->cpuModel, search("model name"));
	fclose(fs);

	fs=fopen("/proc/uptime","r");
	strcpy(srv->uptime, upTime(search("."),1));
	fclose(fs);

	fs=fopen("/proc/meminfo","r");
	sscanf( search("MemTotal"),"%d",&srv->memoryTotal);
	rewind(fs);
	sscanf( search("MemFree"),"%d",&srv->memoryFree);
	fclose(fs);
	strcpy(srv->date,getTime());
	
}
void setWeb(struct serverData *srv){
	printf("%s%c%c\n","Content-Type:text/html;charset=UTF-8",13,10);
	printf("<html><header><title>Server Info</title></header><body><h1>Server Info</h1>");

	printf("<p><b>Marca de procesador: </b>%s</p>",srv->cpuType);
	printf("<p><b>Modelo del procesador: </b>%s</p>",srv->cpuModel);
	printf("<p><b>uptime: </b>%s</p>",srv->uptime);
	printf("<p><b>Fecha y Hora: </b>%s</p>",srv->date);
	printf("<p><b>Memoria libre: </b>%d</p>",srv->memoryFree);
	printf("<p><b>Memoria total: </b>%d</p>",srv->memoryTotal);
	printf("</body></html>");
}
char* search(const char searchedWord[]) {
	//recibe una palabra de busqueda y devuelve un array con los datos 

	int tmp1, tmp2;
	static char* token;
	static char text[150];
	while (feof(fs) == 0)

	{
		fgets(text, 150, fs); //lee una linea de el archivo

		for (int i = 0; i < strlen(text); i++) {

			if (searchedWord[0] == text[i]) {

				tmp1 = 0;
				tmp2 = i;

				while ((searchedWord[tmp1] == text[tmp2])
						&& (tmp2 < strlen(text))
						&& (tmp1 != strlen(searchedWord))) {

					tmp1++;
					tmp2++;

					if (tmp1 == strlen(searchedWord)) {
						if (!strcmp(searchedWord, ".")) {
							return text;
						} else {
							token = strtok(text, ":");
							token = strtok(NULL, "");
							return token;
						}

					}
				}
			}
		}

	}
	return '\0';

}
char* upTime(char texto[],int div) {
	//toma la linea de texto y me la devuelve con el formato deseado
	float uptime, seconds;
	int days, hours, minutes;
	static char formatedUpTime[25];
	sscanf(texto, "%f ", &uptime);
	uptime= uptime/div;
	days = uptime / 86400;
	hours = (uptime / 3600) - (days * 24);
	minutes = (uptime / 60) - ((days * 24 + hours) * 60);
	seconds = uptime - ((((days * 24 + hours) * 60) + minutes) * 60);
	snprintf(formatedUpTime, sizeof formatedUpTime, " %dD %d:%02d:%02.2f \n",
			days, hours, minutes, seconds);
	return formatedUpTime;
}
char* getTime(){
	time_t timer;
  	struct tm * timeinfo;
	time (&timer);
 	timeinfo = localtime (&timer);
 	return asctime(timeinfo);

}
