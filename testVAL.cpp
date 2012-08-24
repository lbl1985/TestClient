//
//  testVAL.cpp
//		test for video access library
//
//  Created by Tom Hebble on 8/24/12.
//  Copyright (c) 2012 Northeaster University. All rights reserved.
//

#include "val_streamprocessor.h"
#include "passthrough.h"

int main(int argc, char* argv[])
{
	// Default URL if no command-line URL is specified
	const char * pszUrl = "rtsp://localhost/media/p0.live";
	if( argc < 2 )
	{
		printf( "Start with default streaming %s\n", pszUrl );
	}
	else
		pszUrl = argv[1];


	PassThrough playback;
	VAL_StreamProcessor processor( pszUrl );

	processor.setOutput("test", ".jpg", 3, 0);
	//processor.setOutput("test",0,0.0,true);
	//processor.displayOutput("test_window");

	processor.setFrameProcessorWithFnumber(&playback);

	processor.run();

	return 0;
}