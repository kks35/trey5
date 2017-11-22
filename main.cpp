#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
//#include <arpa/inet.h>
//#include <sys/socket.h>
#include <iostream>
#include <map>
#include <vector>

#include "pmd.h"
#include "usb-3100.h"
#include "alg.h"


# define RSSI_READ_DELAY_SAMPLE 5
using namespace std;

std::map<float, float> loadTable(const char *filename) {
  // read step table and read data into matrix
  std::map<float, float> table;
  std::ifstream train_file(filename); // open the training file
  if (!train_file) {
    cout << "cannot open the file.\n";
		return table;
  }

  std::string row;
  while (std::getline(train_file, row)) {
    std::stringstream rowStream(row);
    float rssi;
    float step;
    if (rowStream >> rssi >> step) {
      table[rssi] = step;
    } else {
      std::cerr << "line " << row << " incorrect. "
                << "Should have two doubles";
      abort();
    }
  }

  return table;
}

int main() {
  // iterate through to lines
	std::map<float, float> table = loadTable("step.txt");

	alg a;
  a.initDaq();
	float rssi_o = a.readStep(Direction::C, 0);
	float step = table[rssi_o];

	float rssi_x = a.readStep(Direction::X, step);
	float rssi_y = a.readStep(Direction::Y, step);
	a.correctGM(rssi_x, rssi_y, rssi_o, step);

	std::pair<float,float> S = a.getdelta();
	float corr_x = S.first;
	float corr_y = S.second;
	float rssi_next = a.readStep(Direction::C, 0);
	std::cerr << "Current RSSI value: " << rssi_next << "Corrected value: " <<
		corr_x << corr_y;
	return 0;
}
// objectb for all three rssis

