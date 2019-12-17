#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <mysql/mysql.h>
#include "database.h"


MYSQL_RES * get_scores(int id, MYSQL * conn){
    char queryString[1024];
    snprintf(queryString, sizeof(queryString),"SELECT users.id, users.name, scores.math, scores.physics, scores.chemistry, users.role FROM users INNER JOIN scores WHERE id = %d;", id) ;
    if (mysql_query(conn, queryString)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      mysql_close(conn);
      exit(1);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        finish_with_error(conn);
    }
    return result;
}


MYSQL_RES * get_all_scores(MYSQL * conn){
    char queryString[] = "SELECT users.id, users.name, scores.math, scores.physics, scores.chemistry, users.role FROM users INNER JOIN scores WHERE users.id = scores.id;";
    if (mysql_query(conn, queryString)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      mysql_close(conn);
      exit(1);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        finish_with_error(conn);
    }
    return result;
}

MYSQL_RES * get_score_by_id(int id, MYSQL * conn){
    char queryString[1024];
    snprintf(queryString, sizeof(queryString),"SELECT scores.id, users.name, scores.math, scores.physics, scores.chemistry FROM users INNER JOIN scores WHERE users.id = scores.id AND users.id = %d;", id);
    if (mysql_query(conn, queryString)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      mysql_close(conn);
      exit(1);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        finish_with_error(conn);
    }
    return result;
}

Score * fetch_score_by_id(MYSQL_RES * result){
    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    MYSQL_FIELD *field;
    if ((row = mysql_fetch_row(result))){
        return fetch_score(row);
    } 
    return NULL;
}

MYSQL_RES * get_account_info(char * username, char * password, MYSQL * conn){
    char queryString[128];
    snprintf(queryString, sizeof(queryString)+1, "SELECT * FROM users WHERE username = \'%s\' AND password = \'%s\';", username, password);
    if (mysql_query(conn, queryString)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      mysql_close(conn);
      exit(1);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        finish_with_error(conn);
    }
    return result;
}

Info * fetch_info(MYSQL_ROW row){
    Info * new = (Info*)malloc(sizeof(Info));
    new->id = atoi(row[0]);
    strcpy(new->name, row[1]);
    new->role = atoi(row[4]); 
    new->status = atoi(row[5]);
    // printf("%d %s %d\n", new->id, new->name, new->role);
    return new;
}   

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);        
}
// Function designed for chat between client and server. 

void fetchAllResult(MYSQL_RES * result){
    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    MYSQL_FIELD *field;
    while ((row = mysql_fetch_row(result))) 
    {   
        for(int i = 0; i < num_fields; i++) { 
            if (i == 0) {              
                while(field = mysql_fetch_field(result)) 
                {
                    printf("%s ", field->name);
                }        
                printf("\n\n");           
            }
            printf("%s ", row[i] ? row[i] : "NULL"); 
        } 
        printf("\n"); 
    }
}

Score * fetch_score(MYSQL_ROW row){
    Score * new = (Score*)malloc(sizeof(Score));
    new->id = atoi(row[0]);
    strcpy(new->name, row[1]);
    sscanf(row[2], "%f", &new->math);
    sscanf(row[3], "%f", &new->physics);
    sscanf(row[4], "%f", &new->chemistry); 
    // printf("%2.f %.2f %.2f\n", new->math, new->physics, new->chemistry);
    return new;
}   


void * fetch_first_result(MYSQL_RES * result, int flag){
    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    // mysql_fetch_row(result);
    if ((row = mysql_fetch_row(result))){
        for(int i = 0; i < num_fields; i++){ 
            printf("%s ", row[i] ? row[i] : "NULL"); 
        } 
        printf("\n"); 
        switch(flag){
            case DB_INFO: 
                return fetch_info(row);
            case DB_SCORE:
                return fetch_score(row);
        }
    }else 
        return NULL;
}

Scores * fetch_all_scores(MYSQL_RES * result){
    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    Scores * listUser = (Scores*)malloc(sizeof(listUser));
    int count = 0;
    while ((row = mysql_fetch_row(result))){
        for(int i = 0; i < num_fields; i++){ 
            printf("%s ", row[i] ? row[i] : "NULL"); 
        } 
        listUser->list[count] = fetch_score(row);
        count ++;
        if (count == 10)
            break;
        printf("\n"); 
    }
    listUser->num = count;
    return listUser;
}

void set_log_in(int id, MYSQL * conn){
    char queryString[64]; 
    snprintf(queryString, sizeof(queryString), "UPDATE users SET status = 1 WHERE id = %d;", id);
    if (mysql_query(conn, queryString)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      mysql_close(conn);
      exit(1);
    }
}

void set_log_out(int id, MYSQL * conn){
    char queryString[64]; 
    snprintf(queryString, sizeof(queryString), "UPDATE users SET status = 0 WHERE id = %d;", id);
    if (mysql_query(conn, queryString)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      mysql_close(conn);
      exit(1);
    }
}

int get_role_by_id(int id, MYSQL * conn){
    char queryString[1024];
    MYSQL_ROW row;
    snprintf(queryString, sizeof(queryString),"SELECT role FROM users WHERE id = %d;", id);
    if (mysql_query(conn, queryString)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      mysql_close(conn);
      exit(1);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        finish_with_error(conn);
    }
    if ((row = mysql_fetch_row(result))){
        return atoi(row[0]);
    }
    return -1;
}