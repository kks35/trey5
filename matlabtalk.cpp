#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <valarray>

using namespace std;

int main()

{


    std::ifstream train_file("train_file.txt");
    if (!train_file){
        cout<< "cannot open the file.\n";
        return 0;
    }

    std::vector<std::vector<double> > lines;
    std::string row;
    while(std::getline(train_file, row)){
        std::vector<double> rowData;
        std::stringstream rowStream(row);

        double value;
        while(rowStream >> value){
            rowData.push_back(value);
        }
        lines.push_back(rowData);
        //cout<< row <<endl;
    }
    int r = lines.size();
       //close the training file
        train_file.close();


    // talk to matlab
	Engine *eng;
	mxArray *V = NULL;
	mxArray *R = NULL;

	if (!(eng = engOpen(NULL))) {
		cout<<"Error in open matlab"<<endl;
		return EXIT_FAILURE;
	}
    // read training txt into array

    V = mxCreateDoubleMatrix(r,6,mxREAL); // create varibale to matlab
    R = mxCreateDoubleMatrix(r,1,mxREAL); // result

    //assign value for array
    //std::valarray <double> lines;

    double *pa = mxGetPr(V);
    for (int i=0; i<r;i++){
            for (int j=0; j<6;j++){
                pa[i*6+j] = lines[i][j];
            }
    }


    std::valarray <double> result(r); // doible check
    double *pb = mxGetPr(R);
    for (int i=0; i<r;i++){
        pb[i] = result[i];
        }

    engEvalString (eng, "clc;clear;");
    engPutVariable(eng,"V",V);
    engPutVariable(eng,"Step",R);

    engEvalString (eng, "Step = Err3(V);"); // matrix as an input

    // return the result
    R = engGetVariable(eng,"Step");
    pb = mxGetPr(R);
    for (int i=0; i<r;i++){
        result[i] = pb[i];
    }

    mxDestoryArray(V);
    mxDestoryArray(R);
    engClose(eng);

    for(int i=0; i<r;i++){
        cout<<result[i]<<endl;
    }
    return 0;

}
