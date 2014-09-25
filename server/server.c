#include "server.h"

int server_sockfd, client_sockfd;
struct sockaddr_in server_address;
struct sockaddr_in client_address;
int fd;

int main() {
	int server_len, client_len;
	int result;
	fd_set readfds, testfds;

	unlink("server_socket");
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_address.sin_port = 8888;
	server_len = sizeof(server_address);

	bind(server_sockfd, (struct sockaddr *) &server_address, server_len);

	listen(server_sockfd, 5);

	FD_ZERO(&readfds);
	FD_SET(server_sockfd, &readfds);

	while (1) {
		int nread;

		testfds = readfds;
		printf("server waiting\n");
		result = select(FD_SETSIZE, &testfds, (fd_set *) 0, (fd_set *) 0,
				(struct timeval *) 0);

		if (result < 1) {
			perror("server5\n");
			exit(1);
		}

		for (fd = 0; fd < FD_SETSIZE; fd++) {
			if (FD_ISSET(fd,&testfds)) {
				if (fd == server_sockfd) {
					client_len = sizeof(client_address);
					client_sockfd = accept(server_sockfd,
							(struct sockaddr *) &client_address,
							(socklen_t *) &client_len);
					FD_SET(client_sockfd, &readfds);
					printf("adding client on fd %d\n", client_sockfd);
				} else {
					ioctl(fd, FIONREAD, &nread);

					if (nread == 0) {
						close(fd);
						FD_CLR(fd, &readfds);
						printf("removing client on fd %d\n", fd);
					} else {
						judgeOperation();
					}
				}
			}
		}
	}
	return 0;
}

void judgeOperation() {
	char operation[20];
	memset(&operation, 0, sizeof(operation));
	read(fd, &operation, sizeof(operation));
	if (strcmp(operation, "login") == 0) {
		printf("operation:%s\n", operation);
		checkUser();
	}
	else if (strcmp(operation, "registe") == 0) {
		printf("operation:%s\n", operation);
		addUser();
	}
	else if (strcmp(operation, "addStation") == 0) {
		printf("operation:%s\n", operation);
		addStation();
	}
	else if (strcmp(operation, "addTrain") == 0) {
	  printf("operation:%s\n", operation);
		addTrain();
	}
	else if (strcmp(operation, "searchStation") == 0) {
		printf("operation:%s\n", operation);
		searchStation();
	}
	else if (strcmp(operation, "addStationAlongTrain") == 0) {
		printf("operation:%s\n", operation);
		add_station_along_train();
	}
	else if(strcmp(operation, "view_tickets") == 0) {
	    printf("operation:%s\n", operation);
	    view_tickets();
	}
	else if(strcmp(operation, "buy_tickets") == 0) {
	    printf("operation:%s\n", operation);
	    buy_tickets();
	}
	else if(strcmp(operation, "already_bought") == 0) {
	    printf("operation:%s\n", operation);
	    already_bought();
	}
}

// 登录检查
void checkUser() {
	user oneUser;
	char checkResult[20];
	memset(&oneUser, 0, sizeof(oneUser));
	memset(&checkResult, 0, sizeof(checkResult));
	
	// 读取客户端发过来的用户
	read(fd, &oneUser, sizeof(oneUser));
	
	if(strcmp(oneUser.nickname, "manager") == 0 && strcmp(oneUser.password, "123") == 0){
        strcpy(checkResult, "manager");
        write(fd, &checkResult, sizeof(checkResult));
    }else if(strcmp(oneUser.nickname, "manager") == 0 && strcmp(oneUser.password, "123") != 0){
        strcpy(checkResult, "wrong");
        write(fd, &checkResult, sizeof(checkResult));
    }else{
        FILE *p_read;
        p_read = fopen("identity", "r");
        if(NULL == p_read){
            strcpy(checkResult, "wrong");
            write(fd, &checkResult, sizeof(checkResult));
        }else{
            char temp_name[20];
            char temp_password[20];
            int result = 0;
            while(fscanf(p_read, "%s %s\n", temp_name, temp_password) != EOF){
                if(strcmp(oneUser.nickname, temp_name) == 0){
                    if(strcmp(oneUser.password, temp_password) == 0){
                        result = 1;
                    }
                    break;
                }
            }
            fclose(p_read);
            if(result == 0){
               strcpy(checkResult, "wrong");
               write(fd, &checkResult, sizeof(checkResult));
            }else{
               strcpy(checkResult, "user");
               write(fd, &checkResult, sizeof(checkResult));
            }
        }
    }
}

// 注册判定
void addUser() {
    user oneUser;
	char addResult[20];
	memset(&oneUser, 0, sizeof(oneUser));
	memset(&addResult, 0, sizeof(addResult));
	
	// 读取客户端发过来的用户
	read(fd, &oneUser, sizeof(oneUser));
	
	int result = search_identity(oneUser.nickname, oneUser.password);
    if(result == 1){
        strcpy(addResult, "fail");
        write(fd, &addResult, sizeof(addResult));
    }else{
        FILE *p_write;
        p_write = fopen("identity", "a");
        if(NULL == p_write){
            return;
        }
        fprintf(p_write, "%s %s\n", oneUser.nickname, oneUser.password);
        fclose(p_write);
        strcpy(addResult, "succeed");
        write(fd, &addResult, sizeof(addResult));
    }    
}

// 搜索某个账户是否已经存在，存在返回1
int search_identity(char *nickname, char *password){
    if(strcmp(nickname, "manager") == 0){
        return 1;
    }
    FILE *p_read;
    p_read = fopen("identity", "r");
    if(NULL == p_read){
        return 0;
    }
    char temp_name[20];
    char temp_password[20];
    while(fscanf(p_read, "%s %s\n", temp_name, temp_password) != EOF){
        if(strcmp(nickname, temp_name) == 0){
            fclose(p_read);
            return 1;
        }
    }
    fclose(p_read);
    return 0;
}

// 添加站点
void addStation() {
    char station_name[20];
    char addStationResult[20];
    memset(&station_name, 0, sizeof(station_name));
    memset(&addStationResult, 0, sizeof(addStationResult));
    
    read(fd, &station_name, sizeof(station_name));
    
    int search_station_result = search_station(station_name);
    
    if(search_station_result != 0 && search_station_result != 1){
        // printf("station already exists!\n");
        strcpy(addStationResult, "fail");
        write(fd, &addStationResult, sizeof(addStationResult));
    }else{
        int station_id = 1000;
        if(search_station_result == 1){
            station_id = view_station(0) + 1;
        }
        FILE *p_write;
        p_write = fopen("station", "a");
        if(NULL == p_write){
            return;
        }
        fprintf(p_write, "%d %s\n", station_id, station_name);
        fclose(p_write);
        strcpy(addStationResult, "succeed");
        write(fd, &addStationResult, sizeof(addStationResult));
    }
}

// 查看某站点是否存在，不存在文件返回0，存在文件没有站点返回1， 存在站点返回站点ID
int search_station(char *station_name){
    FILE *p_read;
    p_read = fopen("station", "r");
    if(NULL == p_read){
        return 0;
    }
    int temp_station_id;
    char temp_station_name[20];
    while(fscanf(p_read, "%d %s\n", &temp_station_id, temp_station_name) != EOF){
        if(strcmp(temp_station_name, station_name) == 0){
            fclose(p_read);
            return temp_station_id;
        }
    }
    fclose(p_read);
    return 1;
}

// 查看站点信息的函数；如果参数为1，打印所有站点信息，否则不打印； 返回最后一个站点的ID，若没有站点返回0
int view_station(int visible){
    FILE *p_read;
    p_read = fopen("station", "r");
    if(NULL == p_read){
        return 0;
    }
    int temp_station_id;
    char temp_station_name[20];
    while(fscanf(p_read, "%d %s\n", &temp_station_id, temp_station_name) != EOF){
        if(visible == 1){
            printf("%d  ", temp_station_id);
            printf("%s\n", temp_station_name);
        }
    }
    fclose(p_read);
    return temp_station_id;
}

// 添加车次
void addTrain(){
    int train_number;
    char addTrainResult[20];
    memset(&train_number, 0, sizeof(train_number));
    memset(&addTrainResult, 0, sizeof(addTrainResult));

    read(fd, &train_number, sizeof(train_number));
    
    int search_train_result = search_train(train_number);
    if(search_train_result != 0){
        strcpy(addTrainResult, "fail");
        write(fd, &addTrainResult, sizeof(addTrainResult));
    }else{
        strcpy(addTrainResult, "succeed");
        write(fd, &addTrainResult, sizeof(addTrainResult));
    }
}

// 判断某个车次是否已经存在
int search_train(int train_number){
    FILE *p_read;
    p_read = fopen("train", "r");
    if(NULL == p_read){
        return 0;
    }
    int train_station_id;
    char station_name[20];
    char arrive_time[20];
    int arrive_price;
    while(fscanf(p_read, "%d %s %s %d\n", &train_station_id, station_name, arrive_time, &arrive_price) != EOF){
        int temp_train_number = train_station_id/10000;
        if(temp_train_number == train_number){
            fclose(p_read);
            return 1;
        }
    }
    fclose(p_read);
    return 0;
}

// 查看某个站点是否存在，并且把站点的ID写给客户端
void searchStation(){
    char station_name[20];
    int station_id;
    
    memset(&station_name, 0, sizeof(station_name));
    memset(&station_id, 0, sizeof(station_id));
    
    read(fd, &station_name, sizeof(station_name));
    
    station_id = search_station(station_name);
    
    write(fd, &station_id, sizeof(station_id));
    
}

// 为某个车次添加站点
void add_station_along_train(){
    station oneStation;
    
    memset(&oneStation, 0, sizeof(oneStation));
    
    read(fd, &oneStation, sizeof(oneStation));
    
    FILE *p_write, *p_write2;
    p_write = fopen("train", "a");
    if(NULL == p_write){
        return;
    }
    
    fprintf(p_write, "%d %s %s %d\n", oneStation.train_station_id, oneStation.station_name, oneStation.arrive_time, oneStation.arrive_price);
    
    fclose(p_write);
    p_write2 = fopen("ticket", "a");
    if(NULL == p_write2){
        return;
    }
    fprintf(p_write2, "%d\n", oneStation.train_station_id);
    int i;
    for(i=0; i<SEATS; i++){
        int j = 1;
        fprintf(p_write2, "%d ", j);
    }
    fprintf(p_write2, "\n");
    fclose(p_write2);
}

// 购买车票
void buy_tickets(){
    buy_ticket_info one_buy_ticket_info;
    memset(&one_buy_ticket_info, 0, sizeof(one_buy_ticket_info));

    read(fd, &one_buy_ticket_info, sizeof(one_buy_ticket_info));
    
    sell_seat(one_buy_ticket_info.train_number, one_buy_ticket_info.start_station_name, one_buy_ticket_info.destination_station_name, one_buy_ticket_info.departure_time, one_buy_ticket_info.user_name);

}

// 自动选择座位，并把已被购买的座位标注信息
void sell_seat(int train_number, char *start_station_name, char *destination_station_name, char *departure_time, char *user_name){

    int start_station_id, destination_station_id;
    start_station_id = search_station(start_station_name);
    destination_station_id = search_station(destination_station_name);    
    
    node_seat *available_seats;
    available_seats = get_remaining_tickets(train_number, start_station_id, destination_station_id, departure_time);
    
    char buy_tickets_result[20];
    memset(&buy_tickets_result, 0, sizeof(buy_tickets_result));
    
    if(available_seats == NULL){
        strcpy(buy_tickets_result, "fail");
    }else{
        int sell_seat_number;
        sell_seat_number = available_seats->seat_number;
        
        // 记录下这个用户购买的车票信息 哪个人 哪天 哪辆车 哪个出发站 哪个终点站 哪个座位号
        FILE *note_write;
        note_write = fopen("sold_tickets", "a");
        if(NULL == note_write){
            return;
        }
        fprintf(note_write, "%s %s %d %s %s %d\n", user_name, departure_time, train_number, start_station_name, destination_station_name, sell_seat_number);
        fclose(note_write);
        
        
        
        FILE *p_read, *p_write;
        p_read = fopen(departure_time, "r");
        p_write = fopen("temp_file_for_train_system", "w+");
        
        int need_to_change = 0;
        int judge, temp_train_number, temp_station_id, read_count;
        
        while(fscanf(p_read, "%d\n",&judge) != EOF){
            fprintf(p_write, "%d\n",judge);
            temp_train_number = judge/10000;
            temp_station_id = judge%10000;
            if(temp_train_number != train_number){
                for(read_count=0; read_count<SEATS; read_count++){
                    fscanf(p_read, "%d ",&judge);
                    fprintf(p_write, "%d ",judge);
                }
                fscanf(p_read, "\n");
                fprintf(p_write, "\n");
                continue;
            }else{
                // 读到了开始的那个站
                if(temp_station_id == start_station_id){
                    need_to_change = 1;
                    for(read_count=0; read_count<SEATS; read_count++){
                        if(read_count == sell_seat_number){
                            fscanf(p_read, "%d ",&judge);
                            judge = 0;
                            fprintf(p_write, "%d ",judge);
                        }else{
                            fscanf(p_read, "%d ",&judge);
                            fprintf(p_write, "%d ",judge);
                        }
                    }
                    fscanf(p_read, "\n");
                    fprintf(p_write, "\n");
                    continue;
                }else if(temp_station_id == destination_station_id){
                    need_to_change = 0;
                    for(read_count=0; read_count<SEATS; read_count++){
                        fscanf(p_read, "%d ",&judge);
                        fprintf(p_write, "%d ",judge);
                    }
                    fscanf(p_read, "\n");
                    fprintf(p_write, "\n");
                    continue;
                }else if(need_to_change == 1){
                    for(read_count=0; read_count<SEATS; read_count++){
                        if(read_count == sell_seat_number){
                            fscanf(p_read, "%d ",&judge);
                            judge = 0;
                            fprintf(p_write, "%d ",judge);
                        }else{
                            fscanf(p_read, "%d ",&judge);
                            fprintf(p_write, "%d ",judge);
                        }
                    }
                    fscanf(p_read, "\n");
                    fprintf(p_write, "\n");
                    continue;
                }else{
                    for(read_count=0; read_count<SEATS; read_count++){
                        fscanf(p_read, "%d ",&judge);
                        fprintf(p_write, "%d ",judge);
                    }
                    fscanf(p_read, "\n");
                    fprintf(p_write, "\n");
                    continue;
                }
            }
        }
        
        fclose(p_read);
        fclose(p_write);
        
        copy_file(departure_time, "temp_file_for_train_system");
        strcpy(buy_tickets_result, "succeed");
    }
    
    write(fd, &buy_tickets_result, sizeof(buy_tickets_result));
}


// 查看剩余车票的信息
void view_tickets() {
    ticket_info one_ticket_info;
    memset(&one_ticket_info, 0, sizeof(one_ticket_info));
    read(fd, &one_ticket_info, sizeof(one_ticket_info));
    
    node* useful_trains;
    useful_trains = find_useful_train(one_ticket_info.start_station_name, one_ticket_info.destination_station_name, one_ticket_info.departure_time);
    print_node(useful_trains);
}

// 查找满足用户搜索条件的所有车次及相关信息
node* find_useful_train(char *start_station_name, char *destination_station_name, char *departure_time){
    node *head;
    head = NULL;
    int start_station_id, destination_station_id;
    start_station_id = search_station(start_station_name);
    destination_station_id = search_station(destination_station_name);
    
    FILE *p_read;
    p_read = fopen("train", "r");
    if(NULL == p_read){
        return;
    }
    int temp_train_station_id;
    char temp_station_name[20];
    char temp_arrive_time[20];
    int temp_arrive_price;
    
    int possible_train_number;
    char possible_start_time[20];
    char possible_arrive_time[20];
    int possible_price;
    int possible_tickets = 0;
    
    while(fscanf(p_read, "%d %s %s %d\n", &temp_train_station_id, temp_station_name, temp_arrive_time, &temp_arrive_price) != EOF){
        int temp_train_number = temp_train_station_id/10000;
        int temp_station_id = temp_train_station_id%10000;
        if(temp_train_number != possible_train_number && temp_station_id == start_station_id){
            possible_train_number = temp_train_number;
            strcpy(possible_start_time, temp_arrive_time);
            possible_price = 0 - temp_arrive_price;
        }else if(temp_train_number == possible_train_number && temp_station_id == destination_station_id){
            strcpy(possible_arrive_time, temp_arrive_time);
            possible_price += temp_arrive_price;
            node_seat *possible_tickets_node, *possible_tickets_node_pointer;
            possible_tickets_node = get_remaining_tickets(possible_train_number, start_station_id, destination_station_id, departure_time);
            possible_tickets_node_pointer = possible_tickets_node;
            while(possible_tickets_node_pointer != NULL){
                possible_tickets ++ ;
                possible_tickets_node_pointer = possible_tickets_node_pointer->next;
            }          
            head = insert_train_item(head, possible_train_number, possible_start_time, possible_arrive_time, possible_price, possible_tickets);
            possible_train_number = 0;
            int i, j;
            for(i = 0; i<20; i++){
                possible_start_time[i] = '\0';
            }
            for(j = 0; j<20; j++){
                possible_start_time[j] = '\0';
            }
            possible_price = 0;
            possible_tickets = 0;
        }
    }
    fclose(p_read);
    return head;   
}

// 查看满足条件的余票信息，返回余票链表
node_seat* get_remaining_tickets(int train_number, int start_station_id, int destination_station_id, char *departure_time){
    FILE *p_read;
    p_read = fopen(departure_time, "r");
    if(p_read == NULL){
        FILE *fps, *fpm;
        fps = fopen("ticket", "r");
        fpm = fopen(departure_time, "w+");
        //移动指针到文件最后
        fseek(fps, 0, SEEK_END);
           //取得文件长度
        long int len = ftell(fps);
           //移动指针到文件头
        fseek(fps, 0, SEEK_SET);
           //定义缓冲区长度，虽然fgets第一个参数是：char *;但是这里需要给这个参数指定长度，所以只能用char buffer[len]来声明，否则会报段错误
        char buffer[len];
        char *str = NULL;
           //读取文件按行读取遇到\n停止
        while( (str = fgets(buffer,len,fps)) != NULL){
               //将读取的每行写入文件中
            fputs(str,fpm);
        }
           //关闭打开的文件释放资源
        fclose(fps);
        fclose(fpm);
        p_read = fopen(departure_time, "r");
    }
    node_seat* head;
    head = NULL;
    
    int start_train_station_id, destination_train_station_id;
    start_train_station_id = train_number*10000 + start_station_id;
    destination_train_station_id = train_number*10000 + destination_station_id;
    
    int judge;
    while(fscanf(p_read, "%d\n", &judge)){
        if(judge == start_train_station_id){
            break;
        }
    }
    
    int i;
    int temp_seats[16];
    
    // 读开始的位置情况
    for(i=0; i<SEATS; i++){
        fscanf(p_read, "%d ", &temp_seats[i]);
        if(temp_seats[i] == 1){
            head = insert_seat_item(head, i);
        }
    }
    
    while(head != NULL && fscanf(p_read, "%d ", &judge)){
        if(judge == destination_train_station_id){
        /*
            fscanf(p_read, "\n");
            for(i=0; i<SEATS; i++){
                fscanf(p_read, "%d ", &temp_seats[i]);
            }
            fscanf(p_read, "\n");
            node_seat* pointer;
            pointer = head;
            while(pointer != NULL){
                int temp_seat = pointer->seat_number;
                printf("seat : %d\n", temp_seat);
                pointer = pointer->next;
                if(temp_seats[temp_seat] != 1){
                    printf("value : %d , delete : %d\n", temp_seats[temp_seat], temp_seat);
                    head = delete_seat_item(head, temp_seat);
                }
            }
            */
            break;
        }else if(judge == 0 || judge == 1){
            temp_seats[0] = judge;
            for(i=1; i<SEATS; i++){
                fscanf(p_read, "%d ", &temp_seats[i]);
            }
            fscanf(p_read, "\n");
            node_seat* pointer;
            pointer = head;
            while(pointer != NULL){
                int temp_seat = pointer->seat_number;
                pointer = pointer->next;
                if(temp_seats[temp_seat] != 1){
                    head = delete_seat_item(head, temp_seat);
                }
            }
        }else{
            fscanf(p_read, "\n");
        }
    }
    
    fclose(p_read);
    return head;
}

// 链表中插入一个新的车次及相关信息
node* insert_train_item(node *item_list, int new_train_number, char *new_start_time, char *new_arrive_time, int new_price, int new_tickets){
    if(item_list == NULL){
        item_list = (node *)malloc(LEN);
        item_list->train_number = new_train_number;
        strcpy(item_list->start_time, new_start_time);
        strcpy(item_list->arrive_time, new_arrive_time);
        item_list->price = new_price;
        item_list->tickets = new_tickets;
        item_list->next = NULL;
        return item_list;
    }
    node *pointer;
    pointer = (node *)malloc(LEN);
    pointer->train_number = new_train_number;
    strcpy(pointer->start_time, new_start_time);
    strcpy(pointer->arrive_time, new_arrive_time);
    pointer->price = new_price;
    pointer->tickets = new_tickets;
    pointer->next = item_list;
    item_list = pointer;
    return item_list;
}

// 链表中插入一个新的座位
node_seat* insert_seat_item(node_seat *item_list, int new_seat_number){
    if(item_list == NULL){
        item_list = (node_seat *)malloc(LEN_SEAT);
        item_list->seat_number = new_seat_number;
        item_list->next = NULL;
        return item_list;
    }
    node_seat *pointer;
    pointer = (node_seat *)malloc(LEN_SEAT);
    pointer->seat_number = new_seat_number;
    pointer->next = item_list;
    item_list = pointer;
    return item_list;
}

// 链表中删除一个不符合要求的座位
node_seat* delete_seat_item(node_seat *item_list, int old_seat_number){
    if(item_list->seat_number == old_seat_number){
        item_list = item_list->next;
        return item_list;
    }
    node_seat *p1;
    node_seat *p2;
    p1 = item_list;
    p2 = item_list->next;
    while(p2->seat_number != old_seat_number){
        p1 = p1->next;
        p2 = p2->next;
    }
    p1->next = p2->next;
    return item_list;
}

// 打印满足搜索结果的车次的信息
void print_node(node *head){
    node *pointer;
    pointer = head;
    if(head != NULL){
        do{
            char still_has_ticket[20];
            memset(&still_has_ticket, 0, sizeof(still_has_ticket));
            strcpy(still_has_ticket, "yes");
            write(fd, &still_has_ticket, sizeof(still_has_ticket));
            
            remain_ticket one_remain_ticket;
            memset(&one_remain_ticket, 0, sizeof(one_remain_ticket));
            one_remain_ticket.train_number = pointer->train_number;
            strcpy(one_remain_ticket.start_time, pointer->start_time);
            strcpy(one_remain_ticket.arrive_time, pointer->arrive_time);
            one_remain_ticket.price = pointer->price;
            one_remain_ticket.tickets = pointer->tickets;
            
            write(fd, &one_remain_ticket, sizeof(one_remain_ticket));
            
            pointer = pointer->next;
        }while(pointer != NULL);
    }
    char still_has_ticket[20];
    memset(&still_has_ticket, 0 , sizeof(still_has_ticket));
    strcpy(still_has_ticket, "no");
    write(fd, &still_has_ticket, sizeof(still_has_ticket));
}

// 打印满足条件的座位的信息
void print_node_seat(node_seat *head){
    node_seat *pointer;
    pointer = head;
    if(head != NULL){
        do{
            printf("%d\n", pointer->seat_number);
            pointer = pointer->next;
        }while(pointer != NULL);
    }
}

// 将src中的内容复制到des
void copy_file(char *des, char *src){
    FILE *fps, *fpm;
    fps = fopen(src, "r");
    fpm = fopen(des, "w+");
    //移动指针到文件最后
    fseek(fps, 0, SEEK_END);
    //取得文件长度
    long int len = ftell(fps);
    //移动指针到文件头
    fseek(fps, 0, SEEK_SET);
    //定义缓冲区长度，虽然fgets第一个参数是：char *;但是这里需要给这个参数指定长度，所以只能用char buffer[len]来声明，否则会报段错误
    char buffer[len];
    char *str = NULL;
    //读取文件按行读取遇到\n停止
    while( (str = fgets(buffer,len,fps)) != NULL){
        //将读取的每行写入文件中
        fputs(str,fpm);
    }
    //关闭打开的文件释放资源
    fclose(fps);
    fclose(fpm);
}

// 查看某个客户已经购买的车票信息
void already_bought(){
    char user_name[20];
    memset(&user_name, 0, sizeof(user_name));
    read(fd, &user_name, sizeof(user_name));
    
    FILE *note_read;
    note_read = fopen("sold_tickets", "r");
    if(NULL != note_read){     
        sold_ticket one_sold_ticket;
        memset(&one_sold_ticket, 0, sizeof(one_sold_ticket));
        while(fscanf(note_read, "%s %s %d %s %s %d\n", one_sold_ticket.user_name, one_sold_ticket.departure_time, &one_sold_ticket.train_number, one_sold_ticket.start_station_name, one_sold_ticket.destination_station_name, &one_sold_ticket.sell_seat_number) != EOF){
            if(strcmp(one_sold_ticket.user_name, user_name) == 0){
                char still_has_item[20];
                memset(&still_has_item, 0, sizeof(still_has_item));
                strcpy(still_has_item, "yes");
                write(fd, &still_has_item, sizeof(still_has_item));
            
                write(fd, &one_sold_ticket, sizeof(one_sold_ticket));
            }            
        } 
        fclose(note_read);      
    }
    char still_has_item[20];
    memset(&still_has_item, 0, sizeof(still_has_item));
    strcpy(still_has_item, "no");
    write(fd, &still_has_item, sizeof(still_has_item));
}








