#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int main(void)
{
FILE *out;
char *rawdata;
char *data;
unsigned long length;
unsigned long writelength;
char *pos;
 
printf("Content-type: text/html\n\n<HTML><PRE>");
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
//chequeo q sea un modulo lo que han subido
if(checkModule("/var/www/html/files/modulo.ko")){
	
	fflush(0);
	system("sudo insmod /var/www/html/files/modulo.ko");
	printf("%s\n","MODULO INSTALADO CORRECTAMENTE" );

}else{

	printf("%s\n","NO ES UN MODULO INTENTA DE NUEVO" );
}
setWeb();
return 0; // exit

}
void setWeb(){
//printf("%s%c%c\n","Content-Type:text/html;charset=UTF-8",13,10);
printf("<html><head>");
printf("<TITLE>MODULES</TITLE></head>\n");
printf("<body>");
printf("<form enctype=\"multipart/form-data\" action=\"upload.cgi\" method=\"POST\"><input name=\"upload\" type=\"file\"/><input type=\"submit\" value=\"Subir\"/></form>");
printf("<p><form action=\"/cgi-bin/delete.cgi\"   METHOD=\"GET\"><input name=\"module\" type=\"text\"/><input  type=\"submit\" value=\"Borrar Modulo\"  /></form></p>");
printf("<p>");
fflush(0);
system("lsmod");
printf("</p>");
printf("</body></html>");
}

int checkModule(char * path){
FILE *fs;
fs=fopen(path,"rb");
if(getc(fs)==127){
return 1;
}else{
	return 0;
}
}

