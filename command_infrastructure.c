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

#define MAX_MESSAGE 1024

typedef enum packet_type_e{
    RREQ_TYPE = 1u,
    RREP_TYPE = 2u,
    RERR_TYPE = 3u,
    RREP_ACK_TYPE = 4u,
    HELLO_TYPE = 1u
}packet_type;

pthread_t timing_handle, net_input_handle;

void* timing_thread(void *temp);
void* net_input_thread(void *temp);

// open("data/capture_<timestamp>.pcap", O_WRONLY | O_CREAT | O_SYNC, , S_IRUSR | S_IWUSR)

// sudo tshark -i wlan0 -l -w - -Eheader=y > capture_<timestamp>.pcap
int main(int argc, char **argv){

    char folder[80], pcap_path[180], output_path[180], aodv_path[180];
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
    pthread_create(&timing_handle, NULL, timing_thread, NULL);
    pthread_create(&net_input_handle, NULL, net_input_thread, NULL);
    
	int sfpt;
    long file_size;
	struct sockaddr_in serv_addr;
	char buffer[100], addr_buffer[100],ack[30];
	FILE * fpt;
	char * file;
    while(1){
	    sfpt = socket(AF_INET,SOCK_STREAM,0);
        fprintf(stderr, "Input ipaddress (ex. 192.168.1.8): ");
        scanf("%s", addr_buffer);
        fprintf(stderr, "Input File Name: ");
        scanf("%s", buffer);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(26755);
        serv_addr.sin_addr.s_addr = inet_addr(addr_buffer);
        inet_pton(AF_INET,addr_buffer,&serv_addr.sin_addr);
        while(connect(sfpt,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0){
            printf("Initial Connection Failed\n");
            fflush(stdout);
        }
        printf("Connected to %s\n", inet_ntoa(serv_addr.sin_addr));
        fflush(stdout);
        fprintf(stderr, "%s\n", buffer);
        if((fpt = fopen(buffer,"r")) < 0){
            continue;
        }
        fseek(fpt,0,SEEK_END);
        file_size = ftell(fpt);
        fseek(fpt,0,SEEK_SET);
        sprintf(ack,"%ld",file_size);
        file = (char *)malloc(file_size);
        fread(file,1,file_size,fpt);
        clock_t start = clock(), diff;
        send(sfpt,ack,sizeof(ack),0);
        send(sfpt,buffer,strlen(buffer)+1,0);
        recv(sfpt,ack,sizeof(ack),0);
        send(sfpt,file,file_size,0);
        fprintf(stderr, "File Send\n");
        recv(sfpt,ack,sizeof(ack),0);
        fprintf(stderr, "Received: %s\n",ack);
        fclose(fpt);
        diff = clock() - start;
        clock_t msec = diff * 1000 / CLOCKS_PER_SEC;
        printf("Time taken for %ldBytes file %ld/%ld s\n", file_size, diff, CLOCKS_PER_SEC);
        fflush(stdout);
        printf("Total Time = %ldms\n", msec);
        fflush(stdout);
	    close(sfpt);
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
	int sfpt,accepted,file_size,opt = 1,read_amount;
	struct sockaddr_in serv_addr,client_addr;
	char buffer[100],ack[] = "Acknowledged";
	FILE * fpt;
	char * file;
	int addrlen = sizeof(serv_addr);

	sfpt = socket(AF_INET,SOCK_STREAM,0);
	setsockopt(sfpt,SOL_SOCKET, SO_REUSEADDR,&opt,sizeof(opt));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(26755);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bind(sfpt,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	listen(sfpt,2);
    while(1){
        accepted = accept(sfpt,(struct sockaddr*)&client_addr,(socklen_t *) &addrlen);
        printf("Connection from %s\n", inet_ntoa(client_addr.sin_addr));
        fflush(stdout);
        recv(accepted,buffer,99,0);
        file_size = atoi(buffer);
        recv(accepted,buffer,99,0);
        fpt = fopen(buffer,"w");
        file = (char *)malloc(file_size);
        send(accepted,ack,sizeof(ack),0);
        while(read_amount < file_size){
            read_amount += read(accepted,&(file[read_amount]),file_size);
        }
        fwrite(file,1,file_size,fpt);
        send(accepted,ack,sizeof(ack),0);
        fclose(fpt);
        close(accepted);
    }
	close(sfpt);
    return NULL;
}