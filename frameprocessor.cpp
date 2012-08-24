#include "frameprocessor.h"

FrameProcessor::FrameProcessor()
{
}

// to display the processed frames
void FrameProcessor::setDisplayInput(std::string wn){
    windowNameInput = wn;
    cv::namedWindow(windowNameInput);
}
// to display the processed frames
void FrameProcessor::setDisplayOutput(std::string wn){
    windowNameOutput = wn;
    cv::namedWindow(windowNameOutput);
}

// do not display the processed frames
void FrameProcessor::dontDisplay(){
    cv::destroyWindow(windowNameInput);
    cv::destroyWindow(windowNameOutput);
    windowNameInput.clear();
    windowNameOutput.clear();
}
