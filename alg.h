#ifndef ALG_H
#define ALG_H

#include<string>
#include<sstream>
#include<exception>
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cstring>
#include<utility>
#include<cstddef>
#include<cmath>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "pmd.h"
#include "usb-3100.h"
#include "hidapi.h"

using namespace std;

enum Direction { C,X,Y };

class alg
{
    public:

        alg(float ox , float rx , float ry , float dx , float dy)
        {
           Loc(ox, rx, ry, dx, dy);

        }


        void Loc(float R_o,float R_x,float R_y,float D_x,float D_y)
        {

            float det = 4*d*f - e*e;
            if (det != 0){
                 x = (((R_x - R_o)/D_x*nor_x-b)*2*f - e*((R_y - R_o)/D_y*nor_y-c))/det;
                 y = (((R_y - R_o)/D_y*nor_y-c)*2*d - e*((R_x - R_o)/D_x*nor_x-b))/det;
            }
        }
        std::pair <float,float> getV(){

             return std::make_pair(x,y);

            }
    protected:
        //variables
        hid_device* hid;
        uint8_t x_pin;
        uint8_t y_pin;
        float cur_x_volt;
        float cur_y_volt;
        float *xvolts;
        float *yvolts;

        void initDaq();
        void setGMVoltage(float x_volt, float y_volt);
        void coarseAlignGM();
        void train();
        void loadTrainingData();
        void readstep(Direction dir, float r);
        void correctGM(float D_x, float D_y);
        void findGMStep(float RSSI_c);

    private:
        float R_o = 0;
        float R_x = 0;
        float R_y = 0;
        float D_x = 0;
        float D_y = 0;


        float nor_x = 0.004518;
        float nor_y = 0.004549;

        float a = 8641;
        float b = 1248;
        float c = 711;
        float d = -1823;
        float e = -71.5;
        float f = -1554;

        float x;
        float y;





};

#endif // ALG_H
