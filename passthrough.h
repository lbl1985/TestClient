//
//  passthrough.h
//  
//
//  Created by Tom Hebble on 8/8/12.
//  Copyright (c) 2012 Northeaster University. All rights reserved.
//

#ifndef passthrough_h
#define passthrough_h

#include "frameprocessorwithfnumber.h"

class PassThrough : public FrameProcessorWithFnumber
{
public:
	void process(cv::Mat &input, cv::Mat &output, const long& fnumber)
	{
		output = input.clone();

		/*std::stringstream ss;
		ss << fnumber;
		cv::Point center;
		center.x = cvRound(0.5*output.cols);
		center.y = cvRound(0.5*output.rows);
		cv::putText(output, ss.str(),center,cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(), 3);*/
	}

};

#endif