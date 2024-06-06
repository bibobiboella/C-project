#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Helper.h"
#include <vector>
#include <cstring>
#include <iostream>
using namespace std;
/*class plot {
    //typedef vector<double> Vector;
    private:
        
        Vector yData1;
        Vector yData2;
        Vector yData3;
        
        char title[30];
        char yLabel[20]; 
    
    public: 
        plot(Vector& yData1_, Vector& yData2_, Vector& yData3_, const char* type) 
        {   
            yData1 = yData1_;
            yData2 = yData2_;
            yData3 = yData3_;
            strcpy(title, type);
            strcat(title, " for 3 groups");
            strcpy(yLabel, type);
            strcat(yLabel, "(%)");
        }
        
        void plotResult() const;
    
        ~plot(){}
};*/


#include <vector> // Ensure you include the vector header

class plot {
    private:
        std::vector<double> yData1;
        std::vector<double> yData2;
        std::vector<double> yData3;
        char title[30];
        char yLabel[20]; 
    
    public:
        // Constructor using std::vector
        plot(std::vector<double>& yData1_, std::vector<double>& yData2_, std::vector<double>& yData3_, const char* type) {
            yData1 = yData1_;
            yData2 = yData2_;
            yData3 = yData3_;
            strcpy(title, type);
            strcat(title, " for 3 groups");
            strcpy(yLabel, type);
            strcat(yLabel, "(%)");
        }
        
        void plotResult() const;
        ~plot() {}
};
