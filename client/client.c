#include "client.h"

int sockfd;
struct sockaddr_in address;
char user_name[20];

int main() {
	int len;
	int result;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = 8888;
	len = sizeof(address);

	result = connect(sockfd, (struct sockaddr *) &address, len);

	if (result == -1) {
		perror("connect failed!\n");
		exit(1);
	} else
		printf("connect successfully!\n");
		first_page();
	return 0;
}

// 主页，用户可选择注册登录，预先存在的是唯一一个管理员账户，用户名为manager，密码为123
void first_page(){
    printf("1 - login\n");
    printf("2 - registe\n");
    printf("3 - exit\n");
    printf("choose one item:\n");
    int login_choose;
    scanf("%d", &login_choose);
    getchar();
    if(login_choose == 1) {
        login();
    }else if(login_choose == 2){
        registe();
    }else if(login_choose == 3){
        exit(0);
    }
}

// 登录判定
void login(){
    // 获得用户输入
    printf("nickname : \n");
    char nickname[20];   
    gets(nickname);
    printf("password : \n");
    char password[20];
    gets(password);
    
    // 准备要发送的消息
    char operation[20]; 
	user oneUser;
	char checkResult[20];
	memset(&operation, 0, sizeof(operation));
	memset(&oneUser, 0, sizeof(oneUser));
	memset(&checkResult, 0, sizeof(checkResult));
	
	// 给操作填充内容，并将用户结构体发送给服务器;
	strcpy(operation, "login");
	strcpy(oneUser.nickname, nickname);
	strcpy(oneUser.password, password);
	write(sockfd, &operation, sizeof(operation));
	write(sockfd, &oneUser, sizeof(oneUser));
	
	// 读取服务器的返回结果
	read(sockfd, &checkResult, sizeof(checkResult));
    
    if(strcmp(checkResult, "manager") == 0){
        printf("Welcome, manager!\n");
        manager_func();
    }else if(strcmp(checkResult, "user") == 0){
        printf("Welcone, user!\n");
        strcpy(user_name, nickname);
        visit_func();
    }else{
        printf("Wrong!\n");
        first_page();
    }
}

// 注册判定
void registe(){
    printf("nickname : \n");
    char nickname[20];
    gets(nickname);
    printf("password : \n");
    char password[20];
    gets(password);
    
    
    // 准备要发送的消息
    char operation[20]; 
	user oneUser;
	char addResult[20];
	memset(&operation, 0, sizeof(operation));
	memset(&oneUser, 0, sizeof(oneUser));
	memset(&addResult, 0, sizeof(addResult));
	
	// 给操作填充内容，并将用户结构体发送给服务器;
	strcpy(operation, "registe");
	strcpy(oneUser.nickname, nickname);
	strcpy(oneUser.password, password);
	write(sockfd, &operation, sizeof(operation));
	write(sockfd, &oneUser, sizeof(oneUser));
	
	// 读取服务器的返回结果
	read(sockfd, &addResult, sizeof(addResult));

    if(strcmp(addResult, "fail") == 0){
        printf("Nickname repeated!\n");
        registe();
    }else{
        printf("succeed!\n");
        strcpy(user_name, nickname);
        visit_func();
    }    
}

// 管理员主功能界面
void manager_func(){
    printf("1 - add station\n");
    printf("2 - add train\n");
    printf("3 - first page\n");
    printf("4 - exit\n");
    printf("choose one item:\n");
    int manager_choose;
    scanf("%d", &manager_choose);
    getchar();
    if(manager_choose == 1){
        add_station();
    }else if(manager_choose == 2){
        add_train();
    }else if(manager_choose == 3){
        first_page();
    }else if(manager_choose == 4){
        exit(0);
    }
}

// 添加站点，需要输入站点名称
void add_station(){
    printf("enter station name : (enter q to exit)\n");
    char station_name[20];
    gets(station_name);
    while(strcmp(station_name, "q") != 0){
        // 准备交互的信息
        char operation[20];
        char addStationResult[20];
        // 准备信息
        memset(&operation, 0, sizeof(operation));
        memset(&addStationResult, 0 , sizeof(addStationResult));
        strcpy(operation, "addStation");
        // 发送给服务器
        write(sockfd, &operation, sizeof(operation));
        write(sockfd, &station_name, sizeof(station_name));
        // 从服务器接收返回信息
        read(sockfd, &addStationResult, sizeof(addStationResult));

        if(strcmp(addStationResult, "fail") == 0){
            printf("station already exists!\n");
        }else{
            printf("succeed!\n");
        }
        printf("enter station name : (enter q to exit)\n");
        gets(station_name);
    }
    manager_func();
}

// 添加车次
void add_train(){
    printf("enter train number (between 1000 and 9999): \n");
    int train_number;
    scanf("%d",&train_number);
    getchar();
    
    char operation[20];
    char addTrainResult[20];
    
    memset(&operation, 0, sizeof(operation));
    memset(&addTrainResult, 0, sizeof(addTrainResult));
    
    strcpy(operation, "addTrain");
    write(sockfd, &operation, sizeof(operation));
    write(sockfd, &train_number, sizeof(train_number));
    
    read(sockfd, &addTrainResult, sizeof(addTrainResult));
    printf("ke hu duan%s\n", addTrainResult);
    
    if(strcmp(addTrainResult, "fail") == 0){
        printf("train number already exists!\n");
    }else{
        add_station_along_train(train_number);
    }
    manager_func();
}

// 给某个车次添加所经过的站点
void add_station_along_train(int train_number){
    printf("enter station name along the train number (enter q to exit):\n");
    char station_name[20];
    gets(station_name);
    while(strcmp(station_name,"q") != 0){
        char operation[20];
        int station_id;
        
        memset(&operation, 0, sizeof(operation));
        memset(&station_id, 0, sizeof(station_id));
        strcpy(operation, "searchStation");
        
        write(sockfd, &operation, sizeof(operation));
        write(sockfd, &station_name, sizeof(station_name));
        
        read(sockfd, &station_id, sizeof(station_id));
        
        if(station_id == 0){
            printf("no station already found!\n");
        }else if(station_id == 1){
            printf("this station does not exist!\n");
        }else{         
            save_station(train_number, station_id, station_name);
        }
        printf("enter station name along the train number (enter q to exit):\n");
        gets(station_name);
    }
}

// 为一个车次增加一个站点
void save_station(int train_number, int station_id, char *station_name){
    int train_station_id = train_number*10000 + station_id;

    printf("enter arrive time (like 18:32):\n");
    char arrive_time[20];
    gets(arrive_time);
    printf("enter arrive price :\n");
    int arrive_price;
    scanf("%d", &arrive_price);
    getchar();

    char operation[20];
    station oneStation;
    
    memset(&operation, 0, sizeof(operation));
    memset(&oneStation, 0, sizeof(oneStation));
    
    strcpy(operation, "addStationAlongTrain");
    oneStation.train_station_id = train_station_id;
    strcpy(oneStation.station_name, station_name);
    strcpy(oneStation.arrive_time, arrive_time);
    oneStation.arrive_price = arrive_price;
    
    write(sockfd, &operation, sizeof(operation));
    write(sockfd, &oneStation,  sizeof(oneStation));
}

// 非管理员账户主功能界面
void visit_func(){
    printf("1 - buy tickets\n");
    printf("2 - already bought\n");
    printf("3 - first page\n");
    printf("4 - exit\n");
    printf("choose one item:\n");
    int visit_choose;
    scanf("%d", &visit_choose);
    getchar();
    if(visit_choose == 1){
        buy_tickets();
    }else if(visit_choose == 2){
        already_bought();
    }else if(visit_choose == 3){
        first_page();
    }else if(visit_choose == 4){
        exit(0);
    }
}

// 查看已经购买的车票信息
void already_bought(){
    char operation[20];
    memset(&operation, 0, sizeof(operation));
    strcpy(operation, "already_bought");
    write(sockfd, &operation, sizeof(operation));
    
    write(sockfd, &user_name, sizeof(user_name));
    
    printf(" 日期    车次  出发站  达到站  座位号\n");
    while(1){
        char still_has_item[20];
        memset(&still_has_item, 0, sizeof(still_has_item));
        read(sockfd, &still_has_item, sizeof(still_has_item));
        // 判断是否还有条目没有打出来
        if(strcmp(still_has_item, "yes") == 0){
            sold_ticket one_sold_ticket;
            memset(&one_sold_ticket, 0, sizeof(one_sold_ticket));
            read(sockfd, &one_sold_ticket, sizeof(one_sold_ticket));
            printf("%s %d %s %s   %d\n",  one_sold_ticket.departure_time, one_sold_ticket.train_number, one_sold_ticket.start_station_name, one_sold_ticket.destination_station_name, one_sold_ticket.sell_seat_number);
        }else{
            break;
        }
    }
    visit_func();
}

// 根据出发到达站，日期查看座位信息
void view_tickets(char *start_station_name, char *destination_station_name, char *departure_time){    
    char operation[20];
    memset(&operation, 0, sizeof(operation));
    strcpy(operation, "view_tickets");
    write(sockfd, &operation, sizeof(operation));
    
    ticket_info one_ticket_info;
    memset(&one_ticket_info, 0, sizeof(one_ticket_info));
    strcpy(one_ticket_info.start_station_name, start_station_name);
    strcpy(one_ticket_info.destination_station_name, destination_station_name);
    strcpy(one_ticket_info.departure_time, departure_time);
    write(sockfd, &one_ticket_info, sizeof(one_ticket_info));
    
    printf("车次 出发时间 到达时间 票价 余票\n");
    // 打印满足条件的车票信息
    while(1){
        char still_has_ticket[20];
        memset(&still_has_ticket, 0 , sizeof(still_has_ticket));
        read(sockfd, &still_has_ticket, sizeof(still_has_ticket));
        // 判断是否还有车票
        if(strcmp(still_has_ticket, "yes") == 0){
            remain_ticket one_remain_ticket;
            memset(&one_remain_ticket, 0, sizeof(one_remain_ticket));
            read(sockfd, &one_remain_ticket, sizeof(one_remain_ticket));
            printf("%d  %s    %s   %d   %d\n", one_remain_ticket.train_number, one_remain_ticket.start_time, one_remain_ticket.arrive_time, one_remain_ticket.price, one_remain_ticket.tickets);
        }else{
            break;
        }
    } 
}

// 买票
void buy_tickets(){
    printf("enter your start :\n");
    char start_station_name[20];
    gets(start_station_name);
    printf("enter your destination :\n");
    char destination_station_name[20];
    gets(destination_station_name);
    printf("enter your departure date (like 20130226, must be 8-bit interger):\n");
    char departure_time[20];
    gets(departure_time);
    
    view_tickets(start_station_name, destination_station_name, departure_time);
    
    printf("enter the train_number you choose(enter 0 to quit):\n");
    int train_number;
    scanf("%d", &train_number);
    getchar();
    
    // 判断用户是否想买票
    if(train_number != 0) {
        buy_ticket_info one_buy_ticket_info;
        memset(&one_buy_ticket_info, 0, sizeof(one_buy_ticket_info));
        
        one_buy_ticket_info.train_number = train_number;
        strcpy(one_buy_ticket_info.user_name, user_name);
        strcpy(one_buy_ticket_info.start_station_name, start_station_name);
        strcpy(one_buy_ticket_info.destination_station_name, destination_station_name);
        strcpy(one_buy_ticket_info.departure_time, departure_time);
        
        char operation[20];
        memset(&operation, 0, sizeof(operation));
        strcpy(operation, "buy_tickets");
        
        write(sockfd, &operation, sizeof(operation));
        write(sockfd, &one_buy_ticket_info, sizeof(one_buy_ticket_info));
        
        char buy_tickets_result[20];
        memset(&buy_tickets_result, 0, sizeof(buy_tickets_result));
        read(sockfd, &buy_tickets_result, sizeof(buy_tickets_result));
        // 判断买票是否成功
        if(strcmp(buy_tickets_result, "fail") == 0){
            printf("no tickets are available now!\n");
        }else{
            printf("succeed!\n");
        }
    }
    visit_func();
}












    
    

