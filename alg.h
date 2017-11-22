#ifndef ALG_H
#define ALG_H

#include <map>
#include <string>
#include <sstream>
#include <exception>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <cstddef>
#include <cmath>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "pmd.h"
#include "hidapi.h"
#include "usb-3100.h"

enum Direction { C, X, Y };

using namespace std;

class alg {
public:
  // alg(float ox, float rx, float ry, float dx, float dy)
	// : R_o(ox), R_x(rx), R_y(ry), D_x(dx), D_y(dy) {
    // Loc(ox, rx, ry, dx, dy);
  // }

  std::pair<float, float> Loc(float R_o, float R_x, float R_y, float D_x,
                              float D_y) {
    float det = 4 * d * f - e * e;
    if (det != 0) {
      x = (((R_x - R_o) / D_x * nor_x - b) * 2 * f -
           e * ((R_y - R_o) / D_y * nor_y - c)) /
          det;
      y = (((R_y - R_o) / D_y * nor_y - c) * 2 * d -
           e * ((R_x - R_o) / D_x * nor_x - b)) /
          det;
    }
    return std::make_pair(x, y);
  }

	void initDaq();
  float readStep(Direction dir, float r);
  void setGMVoltage(float x_volt, float y_volt);
  void correctGM(float rssi_x, float rssi_y, float rssi_o, float step);

	std::pair<float, float>  getdelta(){
		return std::make_pair(delta_x_change, delta_y_change);
	}
protected:
  // variables
  hid_device *hid;
  uint8_t x_pin = 0;
  uint8_t y_pin = 1;
  float cur_x_volt = 0.71;// initial maximum value
	float cur_y_volt = 5.58;
	float delta_x_change = 0; // initial voltage change value
	float delta_y_change = 0;
private:
  float R_o = 0;
  float R_x = 0;
  float R_y = 0;
  float D_x = 0;
  float D_y = 0;

  const float nor_x = 0.004518;
  const float nor_y = 0.004549;
	const float D_xs = 1.23;
	const float D_ys = 3.21;

  float a = 8641;
  float b = 1248;
  float c = 711;
  float d = -1823;
  float e = -71.5;
  float f = -1554;

  float x;
  float y;
};

#endif 
