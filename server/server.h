#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

# include "message.h"

int initial();

void judgeOperation();

void checkUser();

void addUser();

int search_identity(char *nickname, char *password);

void addStation();

int search_station(char *station_name);

int view_station(int visible);

void addTrain();

int search_train(int train_number);

void searchStation();

void add_station_along_train();

void view_tickets();

node* find_useful_train(char *start_station_name, char *destination_station_name, char *departure_time);

node_seat* get_remaining_tickets(int train_number, int start_station_id, int destination_station_id, char *departure_time);

node* insert_train_item(node *item_list, int new_train_number, char *new_start_time, char *new_arrive_time, int new_price, int new_tickets);

node_seat* insert_seat_item(node_seat *item_list, int new_seat_number);

node_seat* delete_seat_item(node_seat *item_list, int old_seat_number);

void print_node(node *head);

void print_node_seat(node_seat *head);

void buy_tickets();

void sell_seat(int train_number, char *start_station_name, char *destination_station_name, char *departure_time, char *user_name);

void copy_file(char *des, char *src);

void already_bought();
