#include <netdb.h> 
#include <sys/socket.h> 

#define PORT 3000 
#define SA struct sockaddr 

char * concat(char * buffer, char * str1, char * str2, int padding);
void decode(char * str, char*str1, char*str2);

void send_request(int sockfd, Func func);
int check_request(int sockfd, Func func);

void send_num(int num, int sockfd);
int receive_num(int sockfd);

char * make_bold(char * source);