#include <time.h>
#include "alg.h"

#define port 8888
#define GM_LATENCY_IN_MILISEC 5

int getRssi() {
  // create socket
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  char buffer[64];

  // send request to server
  struct sockaddr_in serv_addr; // began to binding the socket
  memset(&serv_addr, 0, sizeof(serv_addr)); // 0zero for each byte
  serv_addr.sin_family = AF_INET; // use IPv4
  serv_addr.sin_addr.s_addr = inet_addr("192.168.1.177"); // IP
  serv_addr.sin_port = htons(port);
  // read back data
  unsigned char buff[] = "X_out";
  write(sock, buff, sizeof(buff));

  int recvlen = recvfrom(sock, buffer, sizeof(buffer), 0, NULL, NULL);
  // printf("received %d bytes\n", recvlen);
	float rssi_val;
  if (recvlen > 0) {
    buffer[recvlen] = 0;
    rssi_val = atof(buffer);
  }

  close(sock);
  return rssi_val;
}

// Code for setting up communication with the USB-3100
void alg::initDaq() {
  if (hid_init() < 0) {
    cout << "@hid_init(): Failed to Initialize the hid_init\n";
    throw exception();
  }

  if ((this->hid = hid_open(MCC_VID, USB3103_PID, NULL)) <= 0) {
    cout << "@hid_open(): DAQ USB-3103 Device Not Found! "
         << "check USB connection\n";
    throw exception(); // throw out warning
  }

  // configure pins 0 and 1
  usbAOutConfig_USB31XX(this->hid, 0, UP_10_00V);
  usbAOutConfig_USB31XX(this->hid, 1, UP_10_00V);
}

float alg::readStep(Direction dir, float r) {
  float new_x_volt;
  float new_y_volt;

  // move the GM
  if (dir == C) {
    new_x_volt = this->cur_x_volt;
    new_y_volt = this->cur_y_volt;
  } else if (dir == X) {
    new_x_volt = this->cur_x_volt + r;
    new_y_volt = this->cur_y_volt;
  } else if (dir == Y) {
    new_x_volt = this->cur_x_volt;
    new_y_volt = this->cur_y_volt + r;
  }

  this->setGMVoltage(new_x_volt, new_y_volt);
	float rssi_val = 0.0;
	float init_rssi_val = getRssi();
  struct timespec ts = {0};
  ts.tv_nsec = 6 * 1000 * 1000;
  while (true) {
    nanosleep(&ts, nullptr);
    rssi_val = getRssi();
		if (rssi_val != init_rssi_val) break;
	}
  this->setGMVoltage(this->cur_x_volt,
                     this->cur_y_volt); // move back to the position
  return rssi_val;
}

void alg::setGMVoltage(float x_volt, float y_volt) {
  uint16_t x_pin_bits = 0;
  uint16_t y_pin_bits = 0;

  x_pin_bits = volts_USB31XX(UP_10_00V, x_volt);
  y_pin_bits = volts_USB31XX(UP_10_00V, y_volt);

  usbAOut_USB31XX(this->hid, this->x_pin, x_pin_bits, 0);
  usbAOut_USB31XX(this->hid, this->y_pin, y_pin_bits, 0);
  unsigned int microseconds = 1000 * GM_LATENCY_IN_MILISEC;
  usleep(microseconds);
}

void alg::correctGM(float rssi_x, float rssi_y, float rssi_o, float step) {
  std::pair<float, float> V = Loc(
		rssi_o, rssi_x, rssi_y, step, step);

  float V_x = V.first; // locate position on curve
  float V_y = V.second;
  
  this->delta_x_change = this->cur_x_volt - V_x;
  this->delta_y_change = this->cur_y_volt - V_y;

  this->cur_x_volt -= delta_x_change;
  this->cur_y_volt -= delta_y_change;
  this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);


  /*
  if (((V_x > 0) && (V_y > 0)) && (V_y / V_x >= 1)) {
    this->cur_y_volt -= D_ys / 0.004549;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
  } else if (((V_x > 0) && (V_y > 0)) && (V_y / V_x < 1)) {
    this->cur_x_volt -= D_xs / 0.004518;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
  } else if (((V_x < 0) && (V_y > 0)) && (V_y / V_x <= -1)) {
    this->cur_y_volt -= D_ys / 0.004549;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
  } else if (((V_x < 0) && (V_y > 0)) && (V_y / V_x > -1)) {
    this->cur_x_volt += D_xs / 0.004518;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
  } else if (((V_x < 0) && (V_y < 0)) && (V_y / V_x <= 1)) {
    this->cur_x_volt += D_xs / 0.004518;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
  } else if (((V_x < 0) && (V_y < 0)) && (V_y / V_x > 1)) {
    this->cur_y_volt += D_ys / 0.004549;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
  } else if (((V_x > 0) && (V_y < 0)) && (V_y / V_x >= -1)) {
    this->cur_x_volt += D_xs / 0.004518;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
  } else if (((V_x > 0) && (V_y < 0)) && (V_y / V_x < -1)) {
    this->cur_y_volt += D_ys / 0.004549;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
  } else if ((V_x == 0) && (V_y < 0)) {
    this->cur_y_volt += D_ys / 0.004549;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
  } else if ((V_x == 0) && (V_y > 0)) {
    this->cur_y_volt -= D_ys / 0.004549;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
  } else if ((V_x > 0) && (V_y == 0)) {
    this->cur_x_volt -= D_xs / 0.004518;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
  } else if ((V_x < 0) && (V_y == 0)) {
    this->cur_x_volt += D_xs / 0.004518;
    this->setGMVoltage(this->cur_x_volt, this->cur_y_volt);
  } else {
  }*/
}
