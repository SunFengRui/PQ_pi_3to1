#include "socket_send.h"
#include "workthread.h"
#include "main.h"
#include <sys/socket.h>  //socket
#include <netinet/in.h>  //struct sockaddr_in
#include <arpa/inet.h>   //inet_pton
#include <unistd.h>      //sleep


int send_client_count=0;
char bind_flag=0;
int index_800=0;
char send_flag_temp=0;
extern short output1;
void *SocketThreadFunc(void *arg)
{
    int sock;
    int ret=-1;
    sock=socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in server_addr,client_addr;
    int n=sizeof(client_addr);
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(1401);
    inet_pton(AF_INET,"192.168.1.15",(void *)&server_addr.sin_addr.s_addr);

    client_addr.sin_family=AF_INET;
    client_addr.sin_port=htons(8084);
    inet_pton(AF_INET,"192.168.1.111",(void *)&client_addr.sin_addr.s_addr);

/*
必须指定发送目标的IP（不能为192.168.1.255）以及端口
设置为192.168.1.255不能发送返回-1 错误码13
绑定时会查询该IP所属网口有没有链接，有则绑定成功，否则绑定失败
绑定失败也可以发送，UDP自动选择IP以及网口发送，发送完sendto返回-1，错误码99。
*/
    while(ret==-1)
    {
        ret=bind(sock,(struct sockaddr *)&server_addr,sizeof(server_addr));
        sleep(1);
    }
    bind_flag=1;
    while(1)
    {
        sem_wait(&data_send_sem);
        static int haha=0;
        haha++;
        //send_client_count++;
        //indicators2union();
        //if(bind_flag)
        //sendto(sock, measuring_results.indicators_array_char, sizeof(measuring_results.indicators_array_char), 0, (sockaddr*)&client_addr, n);
        //if(send_flag_temp==0)
        {
            printf("%d %d %d %d %d %d\n",an_buffer_idx-index_800*800,an_buffer_idx,index_800*800,an_buffer[index_800*800],output1,index_800);
        }
        //sendto(sock, (const void *)(&an_buffer[index_800*800]), 1600, 0, (sockaddr*)&client_addr, n);
        sendto(sock, (const void *)&haha, 4, 0, (sockaddr*)&client_addr, n);
        //if(send_flag_temp==0)
        {
            //printf("         %d %d %d 0x%x\n",an_buffer_idx_A-index_800*800,an_buffer_idx_A,index_800*800,an_buffer[index_800*800]);
            send_flag_temp=1;
        }
        index_800++;
        index_800%=(AN_BUFFER_880kLEN/800);
        //sleep(1);
        sem_post(&data_send_sem);
        sem_wait(&data_send_sem);
    }
    close(sock);
}
