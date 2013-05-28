
#include <time.h>

#include "VideoSource.h"
#include "DestinNetworkAlt.h"
#include "Transporter.h"
#include "stdio.h"
#include "unit_test.h"

using namespace cv;
void testNan(float * array, int len){
    for(int i = 0 ; i < len ; i++){
        if(isnan(array[i])){
            printf("input had nan\n");
            exit(1);
        }
    }
}

//@eth : shows centroid images in a window
void displayCentroidImages(DestinNetworkAlt * network){
    string titles[8]={"0","1","2","3","4","5","6","7"};

    int zoom=2;
    int window_width=256;
    for(int i=0;i<8;i++){
        cvMoveWindow((char *)titles[i].data(),512+(i%4)*window_width,(i<4?0:1)*window_width);
        network->eth_imageWinningCentroidGrid(i,zoom,titles[i]);
        zoom*=2;
    }

}

/** meaures time between calls and prints the fps.
 *  print - prints the fps if true, otherwise just keeps the timer consistent
 */
void printFPS(bool print){
    // start = initial frame time
    // end = final frame time
    // sec = time count in seconds
    // set all to 0
    static double end, sec, start = 0;

    // set final time count to current tick count
    end = (double)cv::getTickCount();

    //
    if(start != 0){
        sec = (end - start) / getTickFrequency();
        if(print==true){
            printf("fps: %f\n", 1 / sec);
        }
    }
    start = (double)cv::getTickCount();
}

int main(int argc, char ** argv){

    VideoSource vs(false, "./sample.avi");
    //VideoSource vs(true, "");

    vs.enableDisplayWindow();
    vs.increaseFrame(3);

    SupportedImageWidths siw = W512;

    // Left to Right is bottom layer to top
    uint centroid_counts[]  = {4,6,6,4,4,4,4,4};
    bool isUniform = true;

    DestinNetworkAlt * network = new DestinNetworkAlt(siw, 8, centroid_counts, isUniform);

    Transporter t;
    vs.eth_grab();//throw away first frame in case its garbage
    int frameCount = 0;

    while(vs.eth_grab()){

        frameCount++;

        t.setSource(vs.getOutput());
        t.transport(); //move video from host to card
        testNan(t.getDest(), 512*512);

        //network->doDestin(t.getDest());
        network->doDestin_c1(t.getDest());
        if(frameCount % 2 != 0 ){ //only print every 2rd so display is not so jumpy
            printFPS(false);
            continue;
        }

        // Old clear screen method
        //printf("\033[2J");

        // New clear screen method (might give less flickering...?)
        printf("\033[2J\033[1;1H");

        printf("Frame: %i\n", frameCount);
        printFPS(true);
        int layer = 1;
        Node & n = *network->getNode(layer,0,0);
        printf("Node %i,0,0 winner: %i\n",layer, n.winner);
        printf("Node centroids: %i\n", n.nb);

        printf("Node starv:");
        printFloatArray(n.starv, n.nb);
        printf("Starv coef: %f \n", n.starvCoeff);
        printf("\n");

        printf("layer %i node 0 centroid locations:\n", layer);
        network->printNodeCentroidPositions(layer, 0, 0);
        for(int l = 0 ; l < 8 ; l++){
            printf("belief graph layer: %i\n",l);
            network->printBeliefGraph(l,0,0);
        }


        //@eth : displays centroid image showing windows
        displayCentroidImages(network);
        //network->displayLayerCentroidImages(6);
    }

    delete network;
    return 0;
}