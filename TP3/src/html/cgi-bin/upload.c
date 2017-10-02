#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void setWeb(char * );
int checkModule(char *);

int main(void)
{
FILE *out;
char *rawdata;
char *data;
unsigned long length;
unsigned long writelength;
char *pos;
int result;
char message[50];
out= fopen ("/var/www/html/files/modulo.ko","wb+");
length=atol(getenv("CONTENT_LENGTH"));
writelength=length;
rawdata=malloc(length+1);
fread(rawdata, length, 1, stdin); //leo lo que llega por el form y lo cargo en un buffer

//ahora busco un doble salto de linea +retorno de carro que es agregado por el post de html
//luego corto apartir de ahi y guardo el puntero en data
for (pos=rawdata; pos<(rawdata+length-4); pos++)
{
 writelength--;
 if ( (pos[0]==13) && (pos[1]==10) && (pos[2]==13) && (pos[3]==10)  )
 {
  data=pos+4;
  pos=rawdata+length+2;
  writelength-=3;
 }
}
fwrite(data, 1, writelength, out);
 
fclose(out);
free(rawdata);
fflush(0);
result = system("sudo insmod /var/www/html/files/modulo.ko");	
if(result == 0){
strcpy(message,"El modulo ha sido instalado");
}else{
	strcpy(message,"El archivo subido no es un modulo, o ya se encuentra instalado");
}
setWeb(message);

return 0; // exit

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

