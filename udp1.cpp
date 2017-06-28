#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define port 8888
int main(){
    //create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    char buffer[2048];
    //send request to server
    struct sockaddr_in serv_addr;//began to binding the socket
    memset(&serv_addr, 0, sizeof(serv_addr));  //0zero for each byte
    serv_addr.sin_family = AF_INET;  //use IPv4
    serv_addr.sin_addr.s_addr = inet_addr("192.168.1.177");  //IP
    serv_addr.sin_port = htons(port);  //port
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    //read back data
    for (;;) {
                printf("waiting on port %d\n", port);
                recvlen = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&remaddr, &addrlen);
                printf("received %d bytes\n", recvlen);
                if (recvlen > 0) {
                        buf[recvlen] = 0;
                        printf("RSSI: \"%d\"\n", buf)


    close(sock);
    return 0;
}
