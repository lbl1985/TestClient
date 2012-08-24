//
//  val_streamprocessor.cpp
//  
//
//  Created by Tom Hebble on 8/8/12.
//  Copyright (c) 2012 Northeaster University. All rights reserved.
//

#include "val_streamprocessor.h"
#include <conio.h>

// Constructor. Set all parameters of IMediaAppCallback here
VAL_StreamProcessor::VAL_StreamProcessor( const char * szUrl ) : liveStreamer( NULL ), 
	callIt(true), delay(0), fnumber(0), stop(false), frameToStop(-1), everyStep(3)
{
	memset( &StreamParam, 0, sizeof( SStreamingProperty ) );
	StreamParam.bEnableDecoding = true;
	StreamParam.bRtpOverTcp = false;
	StreamParam.fFrameRate = 30;
	StreamParam.ePixelFormat = EPIX_FMT_RGB24;//EPIX_FMT_YUV420P;//EPIX_FMT_RGB565LE;
	StreamParam.nSmoothingBufLength = 0;
	StreamParam.sCamUrl = strdup( szUrl );
	//rawFile = fopen("rawvideo.yuv", "wb" );
}

// Destructor
VAL_StreamProcessor::~VAL_StreamProcessor()
{
	Close();
	/*if( m_rawFile )
	{
		fclose( m_rawFile );
		m_rawFile = NULL;
	}*/
	if( StreamParam.sCamUrl )
	{
		delete [] StreamParam.sCamUrl;
		StreamParam.sCamUrl = NULL;
	}
}

// Function to initialize library
bool VAL_StreamProcessor::Start()
{
	if( liveStreamer == NULL )
	{
		if( strstr(StreamParam.sCamUrl, "rtsp://") == StreamParam.sCamUrl )
			liveStreamer = new CCTLiveMediaReader( *this, StreamParam );
		else
			liveStreamer = new CCTStreamReader( *this, StreamParam );

		if( liveStreamer->Open(&CCTMMTime::GetInstance()) == 0 )
		{
			liveStreamer->Start();
		}
		else
		{
			delete liveStreamer;
			liveStreamer = NULL;
		}
	}
	return liveStreamer != NULL;
}

// A test function to test stream replay
bool VAL_StreamProcessor::Replay()
{
	if( liveStreamer ) {
		return liveStreamer->StartFor( 1000, -1 ) == TRUE ? true : false;
	}
	return false;
}

// Call back function that will be called every time an image frame becomes available
// The name onEvent is a special name and cannot be redefined for this call back.
void VAL_StreamProcessor::onEvent( CCTStreamReader * pClient, int nEvtType, const void * cpDataObj )
{
	if( nEvtType != ETP_PIC )
		return;

	static int frame_count = 0;
	static bool first_time = true;
	cv::Mat frame;
	cv::Mat output;

	thisClient = pClient;

	if (first_time)
	{
		if (!outputFile.empty())
		{
			double framerate = getFrameRate(); // same as input
			videoSize = getFrameSize();
			bool wrote = writer.open(outputFile, CV_FOURCC('M', 'P', 'E', 'G'),
				framerate, // frame rate of the video
				videoSize, // frame size
				true);    // colorVideo

			if (!wrote)
			{
				std::cerr << "ERROR: Output file cannot be written!!!" << std::endl; 
				outputFile.clear();
			}
		}
		first_time = false;
	}

	if(!readNextFrame(frame))
	{                
		stopIt();
		return;
	}
	

	/*if (frame_count % everyStep != 0)
	{
		return;
	}*/

	// display input frame
	if(windowNameInput.length() !=0)
		cv::imshow(windowNameInput, frame);
	
	// calling the process function
	if(callIt)
	{
		// process the frame
		if (process)
			process(frame, output);
		else if (frameProcessor)
			frameProcessor->process(frame, output);
		else if(frameProcessorWithFnumber)
			frameProcessorWithFnumber->process(frame, output, fnumber);
		//increment frame number
		fnumber++;
	}
	else
	{
		output = frame.clone();
	}
	// write output sequence
	if(outputFile.length() != 0)
		writeNextFrame(output);
	// display output frame
	if(windowNameOutput.length() != 0){
		//cv::imshow(windowNameOutput, output);
		//IplImage img=output;
		//IplImage* pimg = &img;		
		//cvShowImage(windowNameOutput.c_str(), pimg);
		
	}
		


	/*
	// introduce a delay, also the user input
	// control the user input
    unsigned char c;
	if(delay>=0){
		c = cv::waitKey(delay) & 0xff;
		switch (c){
		case 'q':
			stopIt();
			break;
		default:
			;
		}
	}*/

	if(frameToStop>=0 && getFrameNumber() == frameToStop)
		stopIt();
}

// Close the connection to the streamer.
void VAL_StreamProcessor::Close()
{
	if( liveStreamer )
	{
		liveStreamer->Close();
		delete liveStreamer;
		liveStreamer = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////
//videoprocessor functions

// set the callback function that
// will be called for each frame
void VAL_StreamProcessor::setFrameProcessor(void (*frameProcessingCallback)(cv::Mat &, cv::Mat &)){
    frameProcessor = 0;
    frameProcessorWithFnumber = 0;
    process = frameProcessingCallback;
}

// set the instance of the class that
// implements the FrameProcessor interface
void VAL_StreamProcessor::setFrameProcessor(FrameProcessor* frameProcessorPtr){
    // invalidate callback function;
    process = 0;
    frameProcessorWithFnumber = 0;
    // this is the frame processor instance
    // that will be called
    frameProcessor = frameProcessorPtr;
//    callProcessor();
}

// set the FrameProcessorWithFnumber interface
void VAL_StreamProcessor::setFrameProcessorWithFnumber(FrameProcessorWithFnumber *frameProcessorWithFnumberPtr){
    process = 0;
    frameProcessor = 0;
    frameProcessorWithFnumber = frameProcessorWithFnumberPtr;
}

// set the name of the video file
bool VAL_StreamProcessor::setInput(std::string filename){
    fnumber = 0;

	StreamParam.sCamUrl = strdup(filename.c_str());

    return true;
}

// to display the processed frames
void VAL_StreamProcessor::displayInput(std::string wn){
    windowNameInput = wn;
    cv::namedWindow(windowNameInput);
    moveDisplay('i');
}

// moving the Output display window
void VAL_StreamProcessor::moveDisplay(const unsigned char type){
    //videoSize = getFrameSize();
    //std::string str_id = windowNameInput.substr(windowNameInput.size()-1, windowNameInput.size());
    //int id = atoi(str_id.c_str());
    //switch (type){
    //case 'i': // stands for input
    //    cvMoveWindow(windowNameInput.c_str(), id * videoSize.width, 0);
    //    break;
    //case 'o': // stands for output
    //    cvMoveWindow(windowNameOutput.c_str(), id * videoSize.width, videoSize.height+45);
    //    break;
    //default:
    //    ;
    //}
}

// to display the processed frames
void VAL_StreamProcessor::displayOutput(std::string wn){
    windowNameOutput = wn;
    cv::namedWindow(windowNameOutput);
    moveDisplay('o');
}

// do not display the processed frames
void VAL_StreamProcessor::dontDisplay(){
    cv::destroyWindow(windowNameInput);
    cv::destroyWindow(windowNameOutput);
    windowNameInput.clear();
    windowNameOutput.clear();
}

// to grab (and process) the frames of the sequence
void VAL_StreamProcessor::run()
{
	if(!Start())
	{
		Close();
		return;
	}

	while(!isStopped())
	{
		// map some keystrokes to provide control over console application
		if (kbhit())
		{
			int keyVal = getch();
			if (keyVal == 'q' || keyVal == 'Q' || keyVal == 'x' || keyVal == 'X' ||
				keyVal == 27)
			{
				stopIt();
			}
			else if( keyVal == 'p' ) {
				Replay();
			}
		}

		Sleep(500);
	}
}

// Stop the processing
void VAL_StreamProcessor::stopIt()
{
	Close();
    stop = true;
}
// Is the process stopped?
bool VAL_StreamProcessor::isStopped(){
    return stop;
}

//*****************************************************************
// Is a capture device opened?
bool VAL_StreamProcessor::isOpened()
{
	return (StreamParam.sCamUrl != NULL);
}

//*****************************************************************
// set a delay between each frame
// 0 means wait at each frame
// negative means no delay
/*void VAL_StreamProcessor::setDelay(int d){
    delay = d;
}*/


// to get the next frame
// could be: video file or camera; vector of images
bool VAL_StreamProcessor::readNextFrame(cv::Mat &frame)
{
	CPicture * pPic = thisClient->GetNextImage();
	if( !pPic ) 
	{
		return false;
	}

	cv::Mat temp(pPic->m_nHeight, pPic->m_nWidth, CV_8UC3, pPic->m_ui8DataBuf, cv::Mat::AUTO_STEP);
	frame = temp.clone();
	//frame.create(temp.rows, temp.cols, temp.type());
	//cv::cvtColor(temp, frame, CV_RGB2BGR);
	


	thisClient->Recycle( pPic );
}

// process callback to be called
void VAL_StreamProcessor::callProcess(){
    callIt = true;
}
// do not call process callback
void VAL_StreamProcessor::dontCallProcess(){
    callIt = false;
}
void VAL_StreamProcessor::stopAtFrameNo(long frame){
    frameToStop = frame;
}

//*****************************************************************
// return the frame number of the next frame
long VAL_StreamProcessor::getFrameNumber(){
    // get info of from the capture device
    //long fnumber = static_cast<long>(
    //            capture.get(CV_CAP_PROP_POS_FRAMES));
    return fnumber;
}

//*****************************************************************
// return the frame rate of current video
double VAL_StreamProcessor::getFrameRate(){
    //return capture.get(CV_CAP_PROP_FPS);
	return (double)StreamParam.fFrameRate;
}

//*****************************************************************
// get the codec of input video
/*int VAL_StreamProcessor::getCodec(char codec[4]){
    // undefined for vector of images
    if(images.size() != 0) return -1;
    union {// data structur for the 4-char code
        int value;
        char code[4];
    } returned;
    // get the code
    returned.value = static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
    // get the 4 characters
    codec[0] = returned.code[0];
    codec[1] = returned.code[1];
    codec[2] = returned.code[2];
    codec[3] = returned.code[3];
    // return the int value corresponding to the code
    return returned.value;
}*/

//*****************************************************************
// get the size of input video
cv::Size VAL_StreamProcessor::getFrameSize()
{
	if (thisClient)
		return cv::Size(thisClient->PeekNextImage()->m_nWidth, thisClient->PeekNextImage()->m_nHeight);
	else
		return cv::Size();
}

// set the output video file
// by default the same parameters than input video will be used
bool VAL_StreamProcessor::setOutput(const std::string &filename,
                               int codec= 0, double framerate= 0.0,
                               bool isColor= true){
    outputFile = filename;
    extension.clear();
	return true;
    /*if(framerate == 0.0)
        framerate = getFrameRate(); // same as input
    char c[4];
    // use same codec as input
    if (codec == 0){
        codec = getCodec(c);
    }
    videoSize = getFrameSize();
    // Open output video
//    codec = -1;
    framerate = 30;
    return writer.open(outputFile, // filename
//                       codec,
                       CV_FOURCC('M', 'P', 'E', 'G'),
                       //codec, // codec to be used
                       framerate, // frame rate of the video
                       videoSize, // frame size
                       isColor);    // colorVideo
					   */
}
// set the output as a series of image files
// extension must be ".jpg", ".bmp" ...
bool VAL_StreamProcessor::setOutput(const std::string &filename, //prefix
               const std::string &ext, // image file extension
               int numberOfDigits = 3, // number of digits
               int startIndex = 0){     // start index
    // number of digits must be positive
    if (numberOfDigits < 0)
        return false;
    // filenames and their common extension
    outputFile = filename;
    extension = ext;
    // number of digits in the file numbering scheme
    digits = numberOfDigits;
    // start numbering at this index
    currentIndex = startIndex;
    return true;
}
// to write the output frame
// could be: video file or images
void VAL_StreamProcessor::writeNextFrame(cv::Mat& frame){
    if (extension.length()){
        std::stringstream ss;
        // compose the output filename
        ss << outputFile << std::setfill('0')
           << std::setw(digits)
              << currentIndex++ << extension;
        cv::imwrite(ss.str(), frame);
    }else{
        writer.write(frame);        
    }
}

