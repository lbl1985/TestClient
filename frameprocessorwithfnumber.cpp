#include "frameprocessorwithfnumber.h"

FrameProcessorWithFnumber::FrameProcessorWithFnumber()
{
}

// to display the processed frames
void FrameProcessorWithFnumber::setDisplayInput(std::string wn){
    windowNameInput = wn;
    cv::namedWindow(windowNameInput);
}
// to display the processed frames
void FrameProcessorWithFnumber::setDisplayOutput(std::string wn){
    windowNameOutput = wn;
    cv::namedWindow(windowNameOutput);
}

// do not display the processed frames
void FrameProcessorWithFnumber::dontDisplay(){
    cv::destroyWindow(windowNameInput);
    cv::destroyWindow(windowNameOutput);
    windowNameInput.clear();
    windowNameOutput.clear();
}
