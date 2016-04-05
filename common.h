#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <unistd.h>

#define ADDSIZE 128
#define REQUESTSIZE 3000
#define RESPONSESIZE 65536
#define MAXNBROW 20
