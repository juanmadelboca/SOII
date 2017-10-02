#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void setWeb(char *);

int main(void)
{
	char *data;
	char module[50];
	char message[50];
	int result;
	data = getenv("QUERY_STRING");
	sscanf(data,"module=%s",module);
	char command[200]="sudo rmmod ";
	strcat(command,module);
	fflush(0);
	result = system(command);	
	if(result == 0){
	strcpy(message,"El modulo ha sido removido");
	}else{
		strcpy(message,"El modulo que se quiere borrar no existe");
	}
	setWeb(message);
	return 0;
}
void setWeb(char * message){
/**
@brief provee el codigo html para que el cliente visualice la pagina
**/
printf("Content-type: text/html\n\n<pre>");
printf("<html><head><title>\"AWS Service\"</title><meta charset=\"UTF-8\">");
printf("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
printf("<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\"");
printf("integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">");
printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"./../css/main.css\" media=\"screen\" /></head><body><div class=\"main-container text-align\">");
printf("<div>%s </div>",message);
printf("<form enctype=\"multipart/form-data\" action=\"upload.cgi\" method=\"POST\" class=\"row\" ><div class=\"col-md-6\">");
printf("<label class=\"btn btn-grey-form\">Buscar <input type=\"file\" name=\"upload\" style=\"display: none;\"></label></div>");
printf("<div class=\"col-md-6\"><input type=\"submit\" value=\"Subir\" class=\"btn btn-grey-form\" /></div></form>");
printf("<p><form action=\"/cgi-bin/delete.cgi\" METHOD=\"GET\" class=\"row\"><div class=\"col-md-6\">");
printf("<input name=\"module\" type=\"text\" placeholder=\"Nombre del modulo\" class=\"form-control\"></div><div class=\"col-md-6\">");
printf("<input  type=\"submit\" value=\"Borrar Modulo\"  class=\"btn btn-grey-form\"/></div></form></p><div class=\"text-lsmod\">");
fflush(0);
system("lsmod");
printf("</div></div></body></html>");
}