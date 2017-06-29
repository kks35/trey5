from socket import *
import time

address = ( '192.168.1.177', 8888) #My machine IP# and port#
client_socket = socket(AF_INET, SOCK_DGRAM) #SETUP the socket
client_socket.settimeout(0.5) # wait for 0.5 second for a response

while(1):

    data = "X_out"
    client_socket.sendto( data, address) #send data request

    try:

        rec_data, addr = client_socket.recvfrom(2048) #read response form Arduino packet size 2048
        RSSI = int(rec_data) #convert string rec_data to int rssi
        print " RSSI is ", RSSI #print the result

    except:
        pass
    #time.sleep(1) # delay before send next demand
        
