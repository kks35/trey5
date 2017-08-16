#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <fstream>

using namespace std;

#define port 8888
#define BUF 10240

int main(){
    //create socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

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

    unsigned char buff[]="X_out";
    bool xout_sent = false;

    //read back data
    for (;;) {            
        if (!xout_sent) {
            write(sock,buff,sizeof(buff));
            xout_sent = true;
        }
//////////////////////////
	int len=0;
	char buffer[BUF];
	while (len < BUF){
        	int recvlen = recvfrom(sock, buffer+len, BUF-len, 0, NULL, NULL);
        	len += recvlen;
	}
	FILE *f=fopen("latency.txt","w");
	int written = 0;
	while(written<BUF){
		int writelen = fwrite(buffer+written,1,BUF-written,f);
		written += writelen;
	}
        fclose(f);
    }


    close(sock);
    return 0;
}
