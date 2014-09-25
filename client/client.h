#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>

#include "message.h"

void first_page();
void login();
void registe();
int search_identity(char *nickname, char *password);

void visit_func();
void view_tickets(char *start_station_name, char *destination_station_name, char *departure_time);
node* find_useful_train(char *start_station_name, char *destination_station_name, char *departure_time);
node_seat* get_remaining_tickets(int train_number, int start_station_id, int destination_station_id, char *departure_time);
void buy_tickets();
void sell_seat(int train_number, int start_station_id, int destination_station_id, char *departure_time);
void already_bought();

void manager_func();
void add_station();
int view_station(int visible);
int search_station(char *station_name);
void add_train();
int search_train(int train_number);
void add_station_along_train(int train_number);
void save_station(int train_number, int station_id, char *station_name);

void copy_file(char *des, char *src);
