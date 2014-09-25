// 描述搜索满足条件的车次结果的结构体
typedef struct node_str{
    int train_number;
    char start_time[8];
    char arrive_time[8];
    int price;
    int tickets;
    struct node_str *next;
}node;

// 可以坐的位置的结构体
typedef struct node_seat_str{
    int seat_number;
    struct node_seat_str *next;
}node_seat;

// 用户帐号密码的结构体
typedef struct user_str{
    char nickname[20];
    char password[20];
}user;

// 某个车次上的站点
typedef struct station_str{
    int train_station_id;
    char station_name[20];
    char arrive_time[20];
    int arrive_price;
}station;

// 只包含int的结构体
typedef struct intstr_str{
    int value;
}intstr;

// 用户需要查询的信息
typedef struct ticket_info_str{
    char start_station_name[20];
    char destination_station_name[20];
    char departure_time[20];
}ticket_info;

// 用户买票所需要的信息
typedef struct buy_ticket_info_str{
    int train_number;
    char user_name[20];
    char start_station_name[20];
    char destination_station_name[20];
    char departure_time[20];
}buy_ticket_info;

// 服务器返回给客户端的车票信息
typedef struct remain_ticket_str {
    int train_number;
    char start_time[20];
    char arrive_time[20];
    int price;
    int tickets;
}remain_ticket;

// 售票日志中的一条
typedef struct sold_ticket_str {
    char user_name[20]; 
    char departure_time[20];
    int train_number; 
    char start_station_name[20]; 
    char destination_station_name[20]; 
    int sell_seat_number;
}sold_ticket;

#define SEATS 16
#define LEN sizeof(node)
#define LEN_SEAT sizeof(node_seat)

#define BUFFER_SIZE 1024
























