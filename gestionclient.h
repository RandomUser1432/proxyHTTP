

//Prototype de fonction
int readmsg(int currentsocket, char *msg);
int readresponse(int currentsocket, char *response);
void readmsgHTTP(char *msg, char **request);
void gettypemsg(char *header, char *type);
int creategetsocket(char *hostname, char *port);