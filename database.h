#include<mysql/mysql.h>

#define MAX 80 
#define PORT 3000 
#define SA struct sockaddr 
enum{
    DB_INFO=0,
    DB_SCORE
};

typedef enum{
    LOGIN=0,
    LOGOUT,
    SHOW,
    DEL,
    ADD,
    REQ
}Func;

typedef struct user_score{
    int id;
    char name[32];
    float math;
    float physics;
    float chemistry;
}Score;

typedef struct {
    Score * list[10]; 
    int num;
}Scores;

typedef enum{ADMIN=0, USER} Role;

typedef struct user_info{
    int id;
    char name[32];
    Role role;
    int status;
}Info;

typedef struct account{
    char username[32];
    char password[32];
}Account;

typedef struct request{
    Func func;
    int param_id;
    char param_str[32];
}Request;

typedef struct student{
    int id;
    char name[32];
    char username[32];
    char password[32];
    float math;
    float physic;
    float chemistry;
    Role role;
    int status;
}Student;

MYSQL_RES * get_scores(int id, MYSQL * conn);
MYSQL_RES * get_account_info(char * username, char * password, MYSQL * conn);
MYSQL_RES * get_all_scores(MYSQL * conn);

void fetchAllResult(MYSQL_RES * result);
Score * fetch_score(MYSQL_ROW row);
Info * fetch_info(MYSQL_ROW row);

void * fetch_first_result(MYSQL_RES * result, int flag);
Scores * fetch_all_scores(MYSQL_RES * result);

void set_log_in(int id, MYSQL * conn);
void set_log_out(int id, MYSQL * conn);

MYSQL_RES * get_score_by_id(int id, MYSQL * conn);
Score * fetch_score_by_id(MYSQL_RES * result);

int get_role_by_id(int id, MYSQL * conn);

int add_user(char*name, char*username, char*password, int role, MYSQL * conn);

void add_score(int id, float math, float physic, float chemistry, MYSQL * conn);

void delete_score(int id, MYSQL * conn);

void finish_with_error(MYSQL *con);
