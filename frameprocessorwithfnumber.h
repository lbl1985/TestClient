#ifndef FRAMEPROCESSORWITHFNUMBER_H
#define FRAMEPROCESSORWITHFNUMBER_H
#include "utility.h"
//#include "frameprocessor.h"

class FrameProcessorWithFnumber
{
protected:
    std::string windowNameInput;
    std::string windowNameOutput;
public:
    // processing method with fnumber
    virtual void process(cv::Mat &input, cv::Mat &output, const long& fnumber) = 0;
    FrameProcessorWithFnumber();
    // to display the processed frames
    void setDisplayInput(std::string wn);
    // to display the processed frames
    void setDisplayOutput(std::string wn);
    // do not display the processed frames
    void dontDisplay();
};

#endif // FRAMEPROCESSORWITHFNUMBER_H
