#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <netdb.h> 
#include <sys/socket.h> 
#include "database.h"
#include "utils.h"

void send_num(int num, int sockfd){
    char buff[MAX];
    bzero(buff, MAX);
    snprintf(buff, sizeof(int)+1, "%d", num);
    write(sockfd, buff, sizeof(buff));
}

int receive_num(int sockfd){
    char buff[MAX];
    bzero(buff, MAX);
    if (read(sockfd, buff, sizeof(buff)) > 0)
        return atoi(buff);
    return -1;
}

void decode(char * str, char*str1, char*str2){
    int n = 0;
    while(str[n] != ' '){
        str1[n] = str[n];
        n++;
    }
    strcpy(str2, str + n + 1);
}

int check_request(int sockfd, Func func){
    char buff[MAX];
    bzero(buff, MAX);
    if (read(sockfd, buff, sizeof(buff)) > 0)
        if (atoi(buff) == func)
            return 1;
    return 0;
}

void send_request(int sockfd, Func func){
    char buff[MAX];
    bzero(buff, MAX);
    snprintf(buff, sizeof(Func)+1, "%d", func);
    write(sockfd, buff, sizeof(buff));
}

char * concat(char * buffer, char * str1, char * str2, int padding){
    strcpy(buffer, str1);
    if (padding){
        buffer[strlen(str1)] = ' ';
        strcpy(buffer + strlen(str1) + 1, str2);
        buffer[strlen(str1) + strlen(str2) + 1] = '\0';
    }else{
        strcpy(buffer + strlen(str1), str2);
        buffer[strlen(str1) + strlen(str2)] = '\0';
    }
}

char * make_bold(char * source){
    char * target  = (char*)malloc(sizeof(char) * 128); 
    strcpy(target, "<b>");
    strcpy(target + 3, source);
    strcpy(target + 3 + strlen(source), "</b>");
    return target;
}