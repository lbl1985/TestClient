#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H
#include "utility.h"
//#include "videoprocessor.h"

// The frame processor interface
class FrameProcessor
{
protected:
    std::string windowNameInput;
    std::string windowNameOutput;
public:
    // processing method
    virtual void process(cv::Mat &input, cv::Mat &output) = 0;
    FrameProcessor();
    // to display the processed frames
    void setDisplayInput(std::string wn);
    // to display the processed frames
    void setDisplayOutput(std::string wn);
    // do not display the processed frames
    void dontDisplay();
};
#endif
