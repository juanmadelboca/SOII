#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
//printf("%s%c%c\n","Content-Type:text/html;charset=UTF-8",13,10);
printf("Content-type: text/html\n\n<HTML><PRE>");
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
return 0;
}