#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
//#include <arpa/inet.h>
//#include <sys/socket.h>
#include <iostream>
#include "alg.h"

# define RSSI_READ_DELAY_SAMPLE 5
# define GM_LATENCY_IN_MILISEC 5
using namespace std;
# define port 8888

int getRssi();

//Code for setting up communication with the USB-3100
void alg::initDaq() {
	if ( hid_init()< 0) {
		cout<<"@hid_init(): Failed to Initialize the hid_init\n";
		throw exception();

	}

	if ( (this->hid = hid_open(MCC_VID, USB3103_PID, NULL)  ) <= 0){
		cout<<"@hid_open(): DAQ USB-3103 Device Not Found! check USB connection\n";
		throw exception(); //throw out warning
    }
	//configure pins
	usbAOutConfig_USB31XX(this->hid, 0, UP_10_00V); // configure pin 0 and 1
    usbAOutConfig_USB31XX(this->hid, 1, UP_10_00V);
}
void alg::setGMVoltage(float x_volt, float y_volt) { // initialized the DAQ
	uint16_t x_pin_bits = 0;
	uint16_t y_pin_bits = 0;

	x_pin_bits = volts_USB31XX(UP_10_00V, x_volt);
	y_pin_bits = volts_USB31XX(UP_10_00V, y_volt);

	usbAOut_USB31XX(this->hid, this->x_pin, x_pin_bits, 0);
	usbAOut_USB31XX(this->hid, this->y_pin, y_pin_bits, 0);
	unsigned int microseconds = 1000*GM_LATENCY_IN_MILISEC;
	usleep(microseconds);
}


float alg::readstep(Direction dir, float r) {
	float rssi_val = -1;
	float new_x_volt;
	float new_y_volt;

	//move the GM
	if(dir == C){
		new_x_volt = this->cur_x_volt;
		new_y_volt = this->cur_y_volt;
	}
	else if(dir == X){
		new_x_volt = this->cur_x_volt + r;
		new_y_volt = this->cur_y_volt;
	}
	else if(dir == Y){
		new_x_volt = this->cur_x_volt;
		new_y_volt = this->cur_y_volt + r;
	}

	this->setGMVoltage(new_x_volt, new_y_volt);
	for(int i=0;i<RSSI_READ_DELAY_SAMPLE;++i)
		rssi_val = this->getRssi();
	this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);// move back to the position
	return rssi_val
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(){

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// read step table and read data into matrix
	std::ifstream train_file("step.txt"); // open the training file
	if (!train_file) {
		cout << "cannot open the file.\n";
		return 0;
	}

	std::vector<std::vector<double> > lines;
	std::string row;
	while (std::getline(train_file, row)) { // read the 2d (r X 6)matrix in training files into the 2D matrix - lines)
		std::vector<double> rowData;
		std::stringstream rowStream(row);

		double value;
		while (rowStream >> value) {
			rowData.push_back(value);
		}
		lines.push_back(rowData);
		//cout<< row <<endl;
	}
	//close the training file
	train_file.close();
	int r = lines.size();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//setup the table of vale through lines

std::map<double,double>in;
for (int i=0; i<r; i++){
    for (int j = 0; j<2; j++) {
        in[i][j] = lines[i][j];
    }
    }
// iterate through to fine value
float alg::getStep(float cen){
in::iterator srv_iter = this->in.lower_bound(cen);; // srv_iter point to the lowestvalue first(which no smaller than RSSI_C)
float step = sre_iter->second;
return RSSI_o step;
}


void alg::correctGM(float D_x, float D_y) { // need function to define D_x and D_y
	//move GM center, north, south, east, west and read the current RSSIs
    float RSSI_c = this->readstep(C,r);
    float r = getStep(RSSI_c);
    float RSSI_x = this->readstep(X,r);
    float RSSI_y = this->readstep(Y,r);

	alg M(RSSI_c,RSSI_x,RSSI_y,r,r);
    std::pair<float,float> V = M.getV();

    float V_x = V.first; // locate position on curve
    float V_y = V.second;


    this->cur_x_volt -= V_x;
	this->cur_y_volt -= V_y;

    if (((V_x > 0) && (V_y >0)) && (V_y/V_x >= 1))
	this->cur_y_volt -= D_y/0.004549;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
    else if (((V_x > 0) && (V_y >0)) && (V_y/V_x < 1))
    this->cur_x_volt -= D_x/0.004518;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
    else if (((V_x < 0) && (V_y >0)) && (V_y/V_x <= -1))
    this->cur_y_volt -= D_y/0.004549;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
    else if (((V_x < 0) && (V_y >0)) && (V_y/V_x > -1))
    this->cur_x_volt += D_x/0.004518;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
    else if (((V_x < 0) && (V_y <0)) && (V_y/V_x <= 1))
    this->cur_x_volt += D_x/0.004518;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
    else if (((V_x < 0) && (V_y <0)) && (V_y/V_x > 1))
    this->cur_y_volt += D_y/0.004549;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
    else if (((V_x > 0) && (V_y <0)) && (V_y/V_x >= -1))
    this->cur_x_volt += D_x/0.004518;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
    else if (((V_x > 0) && (V_y <0)) && (V_y/V_x < -1))
    this->cur_y_volt += D_y/0.004549;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
    else if ((V_x == 0) && (V_y <0))
    this->cur_y_volt += D_y/0.004549;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
    else if ((V_x == 0) && (V_y >0))
    this->cur_y_volt -= D_y/0.004549;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
    else if ((V_x > 0) && (V_y == 0))
    this->cur_x_volt -= D_x/0.004518;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
    else if ((V_x < 0) && (V_y == 0))
    this->cur_x_volt += D_x/0.004518;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
else
{}


}


}
// objectb for all three rssis

int getRssi (){
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
        //cout<<"Connecting to server..." <<endl;
    }



    //read back data
       unsigned char buff[]="X_out";
       write(sock,buff,sizeof(buff));


                int recvlen = recvfrom(sock, buffer, sizeof(buffer), 0, NULL, NULL);
		// printf("received %d bytes\n", recvlen);
                if (recvlen > 0) {
                        buffer[recvlen] = 0;
                        rssi_val = atof (buffer);

                }

    close(sock);
    return rssi_val;
}

