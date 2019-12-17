#include <gtk/gtk.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include "database.h"
#include "utils.h"

#define MAX 80

enum {
  COLUMN_ID = 0,
  COLUMN_NAME,
  COLUMN_MATH,
  COLUMN_PHYSIC,
  COLUMN_CHEMISTRY,
  N_COLUMNS
};

struct Win {
    GtkWidget       *main;
    GtkWidget       *score;
    GtkWidget       *list;
    GtkWidget       *label;
    GtkWidget       *check_label;
    GtkTreeSelection  *selection;
    int sockfd;
};

GtkWidget       *username_entry;
GtkWidget       *password_entry;
GtkWidget       *login_button;
GtkWidget       *info_button;
GtkWidget       *score_button;
GtkWidget       *log_out_button;

GtkWidget       *add_button;
GtkWidget       *edit_button;
GtkWidget       *delete_button;

GtkWidget       *add_info_window;

Info current_sess = {-1, "", -1, -1};

void set_greeting(GtkWidget * label, char * name);

void buttonFunction(GtkButton * button, gpointer userdata);

void log_in(GtkButton * button, gpointer userdata);

void show_scores(GtkButton * Button, gpointer userdata);

void log_out(GtkButton * button, gpointer userdata);

void init_list(GtkWidget * list){
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *id_column;
    GtkTreeViewColumn *math_column;
    GtkTreeViewColumn *physic_column;
    GtkTreeViewColumn *chemistry_column;
    GtkTreeViewColumn *name_column;
    GtkListStore *store;

    gtk_widget_set_vexpand (GTK_WIDGET (list), TRUE);
    
    renderer = gtk_cell_renderer_text_new ();
    id_column = gtk_tree_view_column_new_with_attributes("ID",
          renderer, "text", COLUMN_ID, NULL);
    gtk_tree_view_column_set_sort_column_id (id_column, COLUMN_ID);
    gtk_tree_view_append_column (GTK_TREE_VIEW(list), id_column);
    gtk_tree_view_column_set_expand (id_column, TRUE);
    gtk_tree_view_column_set_sizing (id_column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width (id_column, 640/10);

    name_column = gtk_tree_view_column_new_with_attributes("Name",
          renderer, "text", COLUMN_NAME, NULL);
    gtk_tree_view_column_set_sort_column_id (name_column, COLUMN_NAME);
    gtk_tree_view_append_column (GTK_TREE_VIEW(list), name_column);
    gtk_tree_view_column_set_expand (name_column, TRUE);
    gtk_tree_view_column_set_sizing (name_column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width (name_column, 640*3/10);

    math_column = gtk_tree_view_column_new_with_attributes("Math",
          renderer, "text", COLUMN_MATH, NULL);
    gtk_tree_view_column_set_sort_column_id (math_column, COLUMN_MATH);
    gtk_tree_view_append_column (GTK_TREE_VIEW(list), math_column);
    gtk_tree_view_column_set_expand (math_column, TRUE);
    gtk_tree_view_column_set_sizing (math_column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width (math_column, 640/5);

    physic_column = gtk_tree_view_column_new_with_attributes("Physic",
          renderer, "text", COLUMN_PHYSIC, NULL);
    gtk_tree_view_column_set_sort_column_id (physic_column, COLUMN_PHYSIC);
    gtk_tree_view_append_column (GTK_TREE_VIEW(list), physic_column);
    gtk_tree_view_column_set_expand (physic_column, TRUE);
    gtk_tree_view_column_set_sizing (physic_column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width (physic_column, 640/5);

    chemistry_column = gtk_tree_view_column_new_with_attributes("Chemistry",
          renderer, "text", COLUMN_CHEMISTRY, NULL);
    gtk_tree_view_column_set_sort_column_id (chemistry_column, COLUMN_CHEMISTRY);
    gtk_tree_view_append_column (GTK_TREE_VIEW(list), chemistry_column);
    gtk_tree_view_column_set_expand (chemistry_column, TRUE);
    gtk_tree_view_column_set_sizing (chemistry_column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width (chemistry_column, 640/5);

    store = gtk_list_store_new(N_COLUMNS,  
                                G_TYPE_INT,
                                G_TYPE_STRING,
                                G_TYPE_DOUBLE,
                                G_TYPE_DOUBLE,
                                G_TYPE_DOUBLE);

    gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));
    g_object_unref(store);
}

void add_to_list(GtkWidget *list,   const gint id, 
                                    const gchar * name,
                                    const gdouble math, 
                                    const gdouble physic,
                                    const gdouble chemistry) {
  GtkListStore *store;
  GtkTreeIter iter;

  store = GTK_LIST_STORE(gtk_tree_view_get_model
      (GTK_TREE_VIEW(list)));

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter,  COLUMN_ID, id, 
                                    COLUMN_NAME, name,
                                    COLUMN_MATH, math, 
                                    COLUMN_PHYSIC, physic, 
                                    COLUMN_CHEMISTRY, chemistry,
                                    -1);
}

void clear_list(GtkWidget *list){
    GtkListStore *store;

    store = GTK_LIST_STORE(gtk_tree_view_get_model
      (GTK_TREE_VIEW(list)));

    gtk_list_store_clear (store);
}

int main(int argc, char *argv[])
{   
    int sockfd; 
    struct sockaddr_in servaddr; 
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 

    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 

    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 

    GtkBuilder      *builder; 
    GtkWidget       *window;
    GtkWidget       *window_score;
    GtkWidget       *list;
    GtkWidget       *vbox;
    GtkWidget       *label;
    GtkWidget       *check_label;
    GtkTreeSelection *selection;

    struct Win w;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "window_main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    add_info_window = GTK_WIDGET(gtk_builder_get_object(builder, "add_info_window"));
    window_score = GTK_WIDGET(gtk_builder_get_object(builder, "window_score"));
    vbox = GTK_WIDGET(gtk_builder_get_object(builder, "my_box"));
    label = GTK_WIDGET(gtk_builder_get_object(builder, "my_label"));
    check_label = GTK_WIDGET(gtk_builder_get_object(builder, "check_label"));

    list = gtk_tree_view_new();
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);

    gtk_window_set_position(GTK_WINDOW(window_score), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window_score), 10);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), list, TRUE, TRUE, 5);
    
    init_list(list);
    gtk_builder_connect_signals(builder, NULL);

    username_entry = GTK_WIDGET(gtk_builder_get_object(builder, "username"));
    password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "password"));
    login_button = GTK_WIDGET(gtk_builder_get_object(builder, "login_button"));
    info_button = GTK_WIDGET(gtk_builder_get_object(builder, "view_info_button"));
    log_out_button = GTK_WIDGET(gtk_builder_get_object(builder, "log_out_button"));
    // add_button = GTK_WIDGET(gtk_builder_get_object(builder, "add_button"));
    // edit_button = GTK_WIDGET(gtk_builder_get_object(builder, "edit_button"));
    // delete_button = GTK_WIDGET(gtk_builder_get_object(builder, "delete_button"));

    w.main = window;
    w.score = window_score;
    w.sockfd = sockfd;
    w.list = list;
    w.label = label;
    w.check_label = check_label;
    w.selection = selection;

    g_signal_connect(G_OBJECT(login_button), "clicked", G_CALLBACK(log_in), &w);
    g_signal_connect(G_OBJECT (window_score), "destroy",G_CALLBACK(gtk_main_quit), &w);
    g_signal_connect(G_OBJECT (log_out_button), "clicked",G_CALLBACK(log_out), &w);
    // g_signal_connect(G_OBJECT (add_button), "clicked",G_CALLBACK(add_view), add_info_window);
    // g_signal_connect(G_OBJECT (delete_button), "clicked",G_CALLBACK(delete_view), &w);

    g_object_unref(builder);

    gtk_widget_show_all(window);                
    gtk_main();
    close(sockfd); 
    return 0;
}

void set_greeting(GtkWidget * label, char * name){
    char greeting[128];
    concat(greeting, "Xin chào", name, 1);
    char *boldGreeting = (char*)malloc(sizeof(char) * 128); 
    boldGreeting = make_bold(greeting);
    gtk_label_set_markup (GTK_LABEL (label), boldGreeting);
}

void log_in(GtkButton * button, gpointer userdata){
    struct Win *w = (struct Win*)userdata;
    int sockfd = w->sockfd;
    Account acc;
    char buff[MAX];
    send_request(sockfd, LOGIN);
    bzero(buff, MAX);
    if (check_request(sockfd, REQ)){
        strcpy(acc.username , gtk_entry_get_text(GTK_ENTRY(username_entry)));
        strcpy(acc.password , gtk_entry_get_text(GTK_ENTRY(password_entry)));
        memcpy(buff, (unsigned char*)&acc, sizeof(acc));
        write(sockfd, buff, sizeof(buff)); 
        bzero(buff, MAX);
        if (read(sockfd, buff, sizeof(buff)) > 0){
            if (strcmp(buff, "This account has been loged in in another place.\n") == 0){
                printf("%s", buff);
                gtk_label_set_markup (GTK_LABEL (w->check_label), buff);
            }else{
                memcpy((unsigned char*)&current_sess, buff, sizeof(current_sess));
                set_greeting(w->label, current_sess.name);
                Score user;
                clear_list(w->list); 
                send_request(sockfd, SHOW);
                if (check_request(sockfd, REQ))
                    send_num(current_sess.id, sockfd);
                int num = receive_num(sockfd);
                for (int i=0; i< num; i++){
                    bzero(buff, MAX); 
                    bzero((unsigned char*)&user, sizeof(user));
                    if (read(sockfd, buff, sizeof(buff)) > 0){
                        memcpy((unsigned char*)&user, buff, sizeof(user));
                        add_to_list(w->list, user.id, user.name, user.math, user.physics, user.chemistry);
                    }
                }
                gtk_widget_hide(w->main);    
                gtk_widget_show_all(w->score);
            }
        }
    }
}


// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}

void on_window_score_destroy(gpointer userdata)
{   
    struct Win *w = (struct Win*)userdata;
    int sockfd = w->sockfd;
    char buff[MAX];
    bzero(buff, MAX);
    write(sockfd, buff, sizeof(buff));
    gtk_main_quit();
    exit(0);
}

void log_out(GtkButton * button, gpointer userdata){
    struct Win *w = (struct Win*)userdata;
    int sockfd = w->sockfd;
    send_request(sockfd, LOGOUT);
    if (check_request(sockfd, REQ)){
        send_num(current_sess.id, sockfd);
        clear_list(w->list); 
        gtk_widget_hide(w->score);   
        gtk_widget_show_all(w->main);
        current_sess.id = -1;
        strcpy(current_sess.name,"");
        current_sess.role =-1;
        current_sess.status = -1;
    }
}

