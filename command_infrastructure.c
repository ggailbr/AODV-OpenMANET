#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_MESSAGE 1024

typedef enum packet_type_e{
    RREQ_TYPE = 1u,
    RREP_TYPE = 2u,
    RERR_TYPE = 3u,
    RREP_ACK_TYPE = 4u,
    HELLO_TYPE = 1u
}packet_type;

#define NODE_NUMBER 3

char *node_list[] = {"192.168.1.5", "192.168.1.9", "192.168.1.8"};

pthread_t timing_handle, net_input_handle;

void* timing_thread(void *temp);
void* net_input_thread(void *temp);
void* net_input_thread_2(void *temp);


char *mangle_rules[] = {
    NULL, // 0
    NULL, // 1
    NULL, // 2
    NULL, // 3
    NULL, // 4
    "sudo iptables -t mangle -A PREROUTING -m mac --mac-source e4:5f:01:47:25:4f -j DROP\n\0", // 5
    NULL, // 6
    "sudo iptables -t mangle -A PREROUTING -m mac --mac-source e4:5f:01:47:55:be -j DROP\n\0", // 7
    "sudo iptables -t mangle -A PREROUTING -m mac --mac-source e4:5f:01:47:5d:e6 -j DROP\n\0", // 8
    "sudo iptables -t mangle -A PREROUTING -m mac --mac-source e4:5f:01:47:56:f6 -j DROP\n\0", // 9

};

// open("data/capture_<timestamp>.pcap", O_WRONLY | O_CREAT | O_SYNC, , S_IRUSR | S_IWUSR)

// sudo tshark -i wlan0 -l -w - -Eheader=y > capture_<timestamp>.pcap
int main(int argc, char **argv){

    char folder[80], pcap_path[180], output_path[180], aodv_path[180];
	int sfpt;
	struct sockaddr_in serv_addr;
	char ack[30] = {0}, target_ip[50];
    int position;


    sprintf(folder, "%ld/", time(NULL));
    struct stat st = {0};
    strcpy(pcap_path, folder);
    strcpy(output_path, folder);
    strcpy(aodv_path, folder);
    strcat(pcap_path, "capture.pcap");
    strcat(output_path, "log.txt");
    strcat(aodv_path, "AODV.txt");
    // Creating the folder
    if (stat(folder, &st) == -1) {
        mkdir(folder, 0777);
    }
    // Spawning the packet capture
    if(fork() == 0){
        prctl(PR_SET_PDEATHSIG, SIGHUP);
        int fd = open(pcap_path, O_RDWR | O_CREAT, S_IRWXO);
        int nfd = open("/dev/null", O_WRONLY);
        fsync(STDOUT_FILENO);
        dup2(fd, 1);
        dup2(nfd, 2);
        dup2(nfd, 3);
        close(fd);
        close(nfd);
        system("sudo tshark -i wlan0 -l -w -");
    }
    // Spawning AODV
    if(fork() == 0){
        prctl(PR_SET_PDEATHSIG, SIGHUP);
        int fd = open(aodv_path, O_RDWR | O_CREAT, S_IRWXO);
        // int nfd = open("/dev/null", O_WRONLY);
        fsync(STDOUT_FILENO);
        dup2(fd, 1);
        dup2(fd, 2);
        // dup2(nfd, 3);
        close(fd);
        // close(nfd);
        system("sudo LD_LIBRARY_PATH=/home/pi/AODV-OpenMANET/MANET-Testbed:$LD_LIBRARY_PATH /home/pi/AODV-OpenMANET/Build/AODV.out");
    }

    int fd = open(output_path, O_RDWR | O_CREAT, S_IRWXO);
    fsync(STDOUT_FILENO);
    dup2(fd, STDOUT_FILENO);
    close(fd);

    fprintf(stderr, "Are you 192.168.1.7?: ");
    scanf("%s", ack);

    pthread_create(&timing_handle, NULL, timing_thread, NULL);

    if(ack[0] != 'y' && ack[0] != 'Y'){
        fprintf(stderr, "Not 7\n");
        pthread_create(&net_input_handle, NULL, net_input_thread, NULL);
    }
    // else{
    //     fprintf(stderr, "7\n");
    //     pthread_create(&net_input_handle, NULL, net_input_thread_2, NULL);
    // }
    while(1){
        if(ack[0] == 'y' || ack[0] == 'Y'){
            // 7598
            while(1){
                memset(ack, '\0', 30);
                fgets(ack, 30, stdin);
                // if running a ping
                if(ack[0] == 'p' || ack[0] == 'P'){
                    fprintf(stderr, "------------PING----------------\n");
                    for(int i = 0; i < NODE_NUMBER; i++){
                        if((sfpt = socket(AF_INET,SOCK_STREAM,0)) < 0){
                            fprintf(stderr, "Failed to create Socket\n");
                        }
                        serv_addr.sin_family = AF_INET;
                        serv_addr.sin_port = htons(26755);
                        serv_addr.sin_addr.s_addr = inet_addr(node_list[i]);
                        inet_pton(AF_INET,node_list[i],&serv_addr.sin_addr);
                        if(connect(sfpt,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0){
                            fprintf(stderr, "Initial Connection Failed\n");
                            fflush(stdout);
                            close(sfpt);
                            continue;
                        }
                        fprintf(stderr, "Connected to %s\n", inet_ntoa(serv_addr.sin_addr));
                        send(sfpt,ack,sizeof(ack),0);
                        fflush(stdout);
                        close(sfpt);
                    }
                }
                else if(ack[0] == 'c' || ack[0] == 'C'){
                    fprintf(stderr, "------------CONF----------------\n");
                    for(int i = NODE_NUMBER - 1; i >= 0; i--){
                        if((sfpt = socket(AF_INET,SOCK_STREAM,0)) < 0){
                            fprintf(stderr, "Failed to create Socket\n");
                        }
                        serv_addr.sin_family = AF_INET;
                        serv_addr.sin_port = htons(26755);
                        serv_addr.sin_addr.s_addr = inet_addr(node_list[i]);
                        inet_pton(AF_INET,node_list[i],&serv_addr.sin_addr);
                        if(connect(sfpt,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0){
                            fprintf(stderr, "Initial Connection Failed\n");
                            fflush(stdout);
                            close(sfpt);
                            continue;
                        }
                        fprintf(stderr, "Connected to %s\n", inet_ntoa(serv_addr.sin_addr));
                        if(i == 0){
                            send(sfpt,"5 8",4,0);
                        }
                        if(i == 1){
                            send(sfpt,"9 7",4,0);
                        }
                        if(i == 2){
                            send(sfpt,"8 7 5",6,0);
                        }
                        fflush(stdout);
                        close(sfpt);
                    }
                    strcpy(ack, "7 8 9");
                    position = 1;
                    while(ack[position] != '\0'){
                        if(ack[position] != ' ' && ack[position] >= '0' && ack[position] <= '9'){
                            system(mangle_rules[ack[position]-'0']);
                        }
                        if(ack[position] == 'f' || ack[position] == 'F'){
                            system("sudo iptables -t mangle -F");
                        }
                        position++;
                    }
                }
                else if(ack[0] == 'f' || ack[0] == 'F'){
                    fprintf(stderr, "------------FLUS----------------\n");
                    for(int i = NODE_NUMBER - 1; i >= 0; i--){
                        if((sfpt = socket(AF_INET,SOCK_STREAM,0)) < 0){
                            fprintf(stderr, "Failed to create Socket\n");
                        }
                        serv_addr.sin_family = AF_INET;
                        serv_addr.sin_port = htons(26755);
                        serv_addr.sin_addr.s_addr = inet_addr(node_list[i]);
                        inet_pton(AF_INET,node_list[i],&serv_addr.sin_addr);
                        if(connect(sfpt,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0){
                            fprintf(stderr, "Initial Connection Failed\n");
                            fflush(stdout);
                            close(sfpt);
                            continue;
                        }
                        send(sfpt,"0 F",4,0);
                        fprintf(stderr, "Connected to %s\n", inet_ntoa(serv_addr.sin_addr));
                        fflush(stdout);
                        close(sfpt);
                    }
                    system("sudo iptables -t mangle -F");
                }
                // If sending a removal
                else if(ack[0] >= '0' && ack[0] <= '9'){
                    fprintf(stderr, "------------SETM----------------\n");
                    position = 0;
                    if(ack[0] == '7'){
                        position = 1;
                        while(ack[position] != '\0'){
                            if(ack[position] != ' ' && ack[position] >= '0' && ack[position] <= '9'){
                                system(mangle_rules[ack[position]-'0']);
                            }
                            if(ack[position] == 'f' || ack[position] == 'F'){
                                system("sudo iptables -t mangle -F");
                            }
                            position++;
                        }
                    }
                    else{
                        sprintf(target_ip, "192.168.1.%d", ack[0]-'0');
                        if((sfpt = socket(AF_INET,SOCK_STREAM,0)) < 0){
                            fprintf(stderr, "Failed to create Socket\n");
                        }
                        printf(target_ip);
                        serv_addr.sin_family = AF_INET;
                        serv_addr.sin_port = htons(26755);
                        serv_addr.sin_addr.s_addr = inet_addr(target_ip);
                        inet_pton(AF_INET,target_ip,&serv_addr.sin_addr);
                        if(connect(sfpt,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0){
                            fprintf(stderr, "Initial Connection Failed\n");
                            fflush(stdout);
                            close(sfpt);
                            continue;
                        }
                        fprintf(stderr, "Connected to %s\n", inet_ntoa(serv_addr.sin_addr));
                        fflush(stdout);
                        send(sfpt,ack,sizeof(ack),0);
                        close(sfpt);
                    }
                }
            }
        }
    }
    return 1;
}
void* timing_thread(void *__temp){
    int temp, fd = socket(AF_INET, SOCK_DGRAM, 0);
    char message[MAX_MESSAGE];
    size_t size;
    if(fd == -1){
        fprintf(stderr, "Failed to create socket\n");
        return NULL;
    }
	struct sockaddr_in serv_addr, incoming_addr;
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(269);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	temp = bind(fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if(temp == -1){
        fprintf(stderr, "Failed to bind socket\n");
        return NULL;
    }
    size = sizeof (incoming_addr);
    while(1){
        temp = recvfrom(fd, message, MAX_MESSAGE, 0,
                         (struct sockaddr *) & incoming_addr, &size);
        if(temp < 0){
            fprintf(stderr, "Error Reading\n");
            return NULL;
        }
        if(temp > 0){
            // fprintf(stderr, "Read %d bytes\n", temp);
            // If it is a string
            if(message[0] == 0x5b){
                message[81] = '\0';
                printf(message);
                fflush(stdout);
            }
        }
    }
}
void* net_input_thread(void *temp){
	int sfpt, s_fpt, accepted, opt = 1;
	struct sockaddr_in serv_addr,client_addr;
	int addrlen = sizeof(serv_addr), position;
    char buff[100], ack[] = "Acknowledged\n";

	sfpt = socket(AF_INET,SOCK_STREAM,0);
	setsockopt(sfpt,SOL_SOCKET, SO_REUSEADDR,&opt,sizeof(opt));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(26755);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bind(sfpt,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	listen(sfpt,2);
    while(1){
        accepted = accept(sfpt,(struct sockaddr*)&client_addr,(socklen_t *) &addrlen);
        fprintf(stderr, "Connection from %s\n", inet_ntoa(client_addr.sin_addr));
        fflush(stdout);
        while(recv(accepted,buff,99,0) == 0);
        close(accepted);
        if(buff[0] != 'p'){
            position = 1;
            while(buff[position] != '\0'){
                if(buff[position] != ' ' && buff[position] >= '0' && buff[position] <= '9'){
                    system(mangle_rules[buff[position]-'0']);
                }
                if(buff[position] == 'f' || buff[position] == 'F'){
                    system("sudo iptables -t mangle -F");
                }
                position++;
            }
        }
        else{
            if(fork() == 0){
                prctl(PR_SET_PDEATHSIG, SIGHUP);
                s_fpt = socket(AF_INET,SOCK_STREAM,0);
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(26755);
                serv_addr.sin_addr.s_addr = inet_addr("192.168.1.7");
                inet_pton(AF_INET,"192.168.1.7",&serv_addr.sin_addr);
                if(connect(s_fpt,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0){
                    fprintf(stderr, "Initial Connection Failed\n");
                    fflush(stdout);
                    close(s_fpt);
                    return NULL;
                }
                fprintf(stderr, "Connected to %s\n", inet_ntoa(serv_addr.sin_addr));
                fflush(stdout);
                fsync(STDOUT_FILENO);
                dup2(s_fpt, 1);
                system("sudo ip route");
                fflush(stdout);
                send(s_fpt,ack,sizeof(ack),0);
                close(s_fpt);
            }
            wait(NULL);
        }
    }
	close(sfpt);
    return NULL;
}