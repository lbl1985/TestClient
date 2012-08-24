//
//  singlesource_videoprocessor.h
//  
//
//  Created by Tom Hebble on 8/8/12.
//  Copyright (c) 2012 Northeaster University. All rights reserved.
//

#ifndef singlesource_videoprocessor_h
#define singlesource_videoprocessor_h

#include "utility.h"
#include "frameprocessor.h"
#include "frameprocessorwithfnumber.h"
#include "CTStreamReader.h"
#include "CTLiveMediaReader.h"


class singlesource_videoprocessor : public IMediaAppCallback
{
public:
	//constructor
    singlesource_videoprocessor();
    // set the callback function that
    // will be called for each frame
    void setFrameProcessor(void(*frameProcessingCallback)(cv::Mat&, cv::Mat&));
    // set the instance of the class that
    // implements the FrameProcessor interface
    void setFrameProcessor(FrameProcessor* frameProcessorPtr);
    // set the FrameProcessorWithFnumber interface
    void setFrameProcessorWithFnumber(FrameProcessorWithFnumber* frameProcessorWithFnumberPtr);
    // set the name of the video file
    bool setInput(std::string filename);
    // set the vector of input images
    bool setInput(const std::vector<std::string>& imgs);
    // set the id of input camera
    bool setInput(const int cameraId); 
    // to display the processed frames
    void displayInput(std::string wn);
    // to display the processed frames
    void displayOutput(std::string wn);
    // moving the Output display window
    void moveDisplay(const unsigned char type);
    // moving the Output display window
    void moveDisplayOutput();
    // do not display the processed frames
    void dontDisplay();
    // to grab (and process) the frames of the sequence
    void run();
    // Stop the processing
    void stopIt();
    // Is the process stopped?
    bool isStopped();
    // Is a capture device opened?
    bool isOpened();
    // set a delay between each frame
    // 0 means wait at each frame
    // negative means no delay
    void setDelay(int d);
    // to get the next frame
    // could be: video file or camera
    bool readNextFrame(cv::Mat &frame);
    // process callback to be called
    void callProcess();
    // do not call process callback
    void dontCallProcess();

    void stopAtFrameNo(long frame);
    // return the frame number of the next frame
    long getFrameNumber();
    // return the frame rate of current video
    double getFrameRate();
    // get the codec of input video
    int getCodec(char codec[4]);
    // get the size of input video
    cv::Size getFrameSize();
    // set the output video file
    // by default the same parameters than input video will be used
    bool setOutput(const std::string &, int, double, bool);
    // set the output as a series of image files
    // extension must be ".jpg", ".bmp" ...
    bool setOutput(const std::string &filename, //prefix
                   const std::string &ext, // image file extension
                   int NumberOfDigits, // number of digits
                   int startIndex); // start index
    // to write the output frame
    // could be: video file or images
    void writeNextFrame(cv::Mat&);



	// Call back function that will be called every time an image frame becomes available
	// The name onEvent is a special name and cannot be redefined for this call back.
	void onEvent( CCTStreamReader * pClient, int nEvtType, const void * cpDataObj );



private:
	CCTStreamReader		* m_liveStreamer;
	SStreamingProperty	m_StreamParam;
	FILE				* m_rawFile;   
	
	
	// the callback function to be called
    // for the processing of each frame
    void(*process)(cv::Mat&, cv::Mat&);    
    // the callback object to be called
    // for the processing of each frame
    FrameProcessor* frameProcessor;
    // callback object with fnumber passing
    FrameProcessorWithFnumber* frameProcessorWithFnumber;
    // a bool to determin if the process callback will be called
};


#endif