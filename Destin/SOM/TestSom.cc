
#include <stdlib.h>
#include <vector>
#include "SomPresentor.hpp"
#include "ClusterSom.hpp"

int main(int argc, char ** argv){
    ClusterSom som(50,50,3);

    std::vector<std::vector<float> > data;

    int n = 1000;
    for(int i = 0 ; i < n; i++){

        std::vector<float> d;
        for(int j = 0 ; j < 3 ; j++){
            d.push_back( (float)rand() / RAND_MAX );
        }
        data.push_back(d);
        som.addTrainData(d.data());
    }

    SomPresentor sp(som);

    som.train(5000);

    sp.showSimularityMap();

    // pause for inut to let user see the output window
    cv::waitKey();

    return 0;
}
