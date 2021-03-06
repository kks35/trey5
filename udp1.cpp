#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>

using namespace std;

#define port 8888

int main(){
    //create socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    char buffer[64];


    //send request to server
    struct sockaddr_in serv_addr;//began to binding the socket
    memset(&serv_addr, 0, sizeof(serv_addr));  //0zero for each byte
    serv_addr.sin_family = AF_INET;  //use IPv4
    serv_addr.sin_addr.s_addr = inet_addr("192.168.1.177");  //IP
    serv_addr.sin_port = htons(port);  //port
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))
    {
        cout<<"Connecting to server..." <<endl;
    }
    

	
    //read back data
    for (;;) {            
                
       unsigned char buff[]="X_out";
       write(sock,buff,sizeof(buff));


                int recvlen = recvfrom(sock, buffer, sizeof(buffer), 0, NULL, NULL);
		// printf("received %d bytes\n", recvlen);
                if (recvlen > 0) {
                        buffer[recvlen] = 0;
                        printf("RSSI: \"%s\"\n", buffer);
                }
    }


    close(sock);
    return 0;
}
