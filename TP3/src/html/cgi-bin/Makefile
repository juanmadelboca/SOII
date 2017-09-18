all: awsinfo.cgi delete.cgi modules.cgi srvinfo.cgi upload.cgi
awsinfo.cgi: awsinfo.c
	cc -std=c99 -g -Wall -pedantic -o awsinfo.cgi awsinfo.c

delete.cgi: delete.c
	cc -std=c99 -g -Wall -pedantic -o delete.cgi delete.c

modules.cgi: modules.c
	cc -std=c99 -g -Wall -pedantic -o modules.cgi modules.c

srvinfo.cgi: serverInfo.c
	cc -std=c99 -g -Wall -pedantic -o srvinfo.cgi serverInfo.c

upload.cgi: upload.c
	cc -std=c99 -g -Wall -pedantic -o upload.cgi upload.c
