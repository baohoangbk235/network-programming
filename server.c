#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <mysql/mysql.h>
#include "database.h"
#include "utils.h"

static char * host = "localhost";
static char * usr = "baohg";
static char * pass = "2344";
static char * dbname = "NetworkProgramming";
unsigned int port = 3306;
static char * unix_socket = NULL;
unsigned int flag = 0;

void handle_add_score(int sockfd, MYSQL * conn){
    char buff[MAX];
    Student st;
    send_request(sockfd, REQ);
    bzero(buff, MAX);
    if (read(sockfd, buff, sizeof(buff)) > 0){
        memcpy((unsigned char*)&st, buff, sizeof(Student));
        st.id = add_user(st.name, st.username, st.password, st.role, conn);
        add_score(st.id, st.math, st.physic, st.chemistry, conn);
    }
}

void handle_delete_score(int sockfd, MYSQL * conn){
    send_request(sockfd, REQ);
    int id = receive_num(sockfd);
    delete_score(id, conn);
}

int handle_login(int sockfd, MYSQL * conn) 
{   
    send_request(sockfd, REQ);
    char buff[MAX];
    bzero(buff, MAX);
    if (read(sockfd, buff, sizeof(buff)) > 0){
        Account acc;
        memcpy((unsigned char*)&acc, buff, sizeof(Account));
        MYSQL_RES * result = get_account_info(acc.username, acc.password, conn);
        bzero(buff, MAX);
        Info * info = (Info*)fetch_first_result(result, DB_INFO);
        if (info != NULL){
            if (info->status == 0){
                info->status = 1;
                set_log_in(info->id, conn);
                printf("Client %s has been loged in.\n", info->name);
                bzero(buff, MAX);
                memcpy(buff,(const unsigned char*)info,sizeof(Info));
                write(sockfd, buff, sizeof(buff));
                return info->id;
            }else{
                strcpy(buff, "This account has been loged in in another place.\n");
                write(sockfd, buff, sizeof(buff));
                return -1;
            }
        }else{
            strcpy(buff, "Invalid username or password. Please try again.\n");
            write(sockfd, buff, sizeof(buff));
        }
        mysql_free_result(result);
    }
    return -1;
} 

void handle_check_score(int sockfd, MYSQL * conn){
    send_request(sockfd, REQ);
    int id = receive_num(sockfd);
    char buffer[MAX];
    MYSQL_RES * result;
    Score * score;
    Scores * scores = (Scores*)malloc(sizeof(Scores));
    int  num;
    int role = get_role_by_id(id, conn);
    if (role == 0){
        result = get_score_by_id(id, conn);
        scores->list[0] = fetch_score_by_id(result);
        scores->num = 1;
    }else{
        result = get_all_scores(conn);
        scores =  fetch_all_scores(result);
    }
    num = scores->num;
    send_num(num, sockfd);
    for(int i=0; i < num; i++){        
        bzero(buffer, MAX);
        score = scores->list[i];
        memcpy(buffer,(unsigned char*)score,sizeof(Score));
        write(sockfd, buffer, sizeof(buffer));
    }
    mysql_free_result(result);
}

void handle_log_out(int sockfd, MYSQL * conn){
    send_request(sockfd, REQ);
    int id = receive_num(sockfd);
    set_log_out(id, conn);
}

int main() 
{ 
    int i, maxi, maxfd, listenfd;
    int nready, client[__FD_SETSIZE], client_id[__FD_SETSIZE];
    ssize_t n;
    socklen_t clilen;
    MYSQL * conn;
    fd_set rset, allset;

    conn = mysql_init(NULL);
    if (mysql_real_connect(conn, host, usr, pass, dbname, port, unix_socket, flag) == NULL){
        fprintf(stderr, "\nError: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
        exit(1);
    }
    printf("Connect database succesfully!\n\n");
    // printf("MySQL Connection Info: %s \n", mysql_get_host_info(conn));
    // printf("MySQL Client Info: %s \n", mysql_get_client_info());
    // printf("MySQL Server Info: %s \n", mysql_get_server_info(conn));


    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    listenfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (listenfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(listenfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(listenfd, 10)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    
    maxfd = listenfd;
    maxi = -1;
    for (int i=0; i < __FD_SETSIZE; i++){
        client[i] = -1;
        client_id[i] = -1;
    }
    
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    for ( ; ; ){
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &rset)){
            clilen = sizeof(cli);
            connfd = accept(listenfd, (SA*)&cli, &clilen);
            if (connfd < 0) { 
                printf("server acccept failed...\n"); 
                exit(0); 
            } 
            else
                printf("server acccept the client...\n");

            for(i=0; i < __FD_SETSIZE; i++){
                if (client[i] < 0){
                    client[i] = connfd;
                    break;
                }
            }
            
            if (i == __FD_SETSIZE){
                printf("Too many clients!\n");
                exit(1);
            }

            FD_SET(connfd, &allset);
            if (connfd > maxfd)
                maxfd = connfd;
            
            if (i > maxi)
                maxi = i;

            if (--nready <= 0)
                continue;
        }

        for (i=0; i <= maxi; i++){
            if ((sockfd = client[i]) < 0){
                continue;
            }

            if (FD_ISSET(sockfd, &rset)){
                char buff[MAX]; 
                bzero(buff, MAX); 
                if ((n=read(sockfd, buff, sizeof(buff))) == 0){
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                    set_log_out(client_id[i], conn);
                    client_id[i] = -1;
                    printf("Client %d has been terminated.\n", i);
                } else{
                    switch (atoi(buff)) {
                        case LOGIN:
                            client_id[i] = handle_login(sockfd, conn);
                            break;

                        case SHOW:
                            handle_check_score(sockfd, conn);
                            break;

                        case LOGOUT:
                            handle_log_out(sockfd, conn);
                            client_id[i] = -1;
                            break;
                        
                        case DEL:
                            handle_delete_score(sockfd, conn);
                            break;
                        
                        case ADD:
                            handle_add_score(sockfd, conn);
                            break;
                    }
                }

                if (--nready <= 0)
                    break;
            }   
        }
    }
    close(listenfd);
    mysql_close(conn); 
} 