/*****************************************************************************
Copyright (c) 2012 Siemens AG. All rights reserved.

AVAST: ALERT Video Analytic Surveillance Transition Project

Software Provided to ALERT under contract to Northeastern University, 
PO# 1206274 by:

Siemens Corporate Research and Technology
755 College Road East,
Princeton, NJ 08540

The use of this software  is authorized for research purposes only, 
and only for the research of this specific project, as defined herein.   
******************************************************************************/

// TestClient.cpp : Defines the entry point for the console application and 
// describes sample code to access decoded video frames.

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "CTLiveMediaReader.h"
#include "libtools/tool_logger.h"
#include "libtools/tool_mmtime.h"


///////////////////////////////////////////////////////////////////////////////

/******************************************************************************
To use the Video Access Library, you have to define a class and derive it from 
IMediaAppCallback class. 
******************************************************************************/
class CTestClient : public IMediaAppCallback
{
public:
	// Constructor. Set all parameters of IMediaAppCallback here
	CTestClient( const char * szUrl ) : m_liveStreamer( NULL )
	{
		memset( &m_StreamParam, 0, sizeof( SStreamingProperty ) );
		m_StreamParam.bEnableDecoding = true;
		m_StreamParam.bRtpOverTcp = false;
		m_StreamParam.fFrameRate = 30;
		m_StreamParam.ePixelFormat = EPIX_FMT_RGB24;//EPIX_FMT_YUV420P;//EPIX_FMT_RGB565LE;
		m_StreamParam.nSmoothingBufLength = 0;
		m_StreamParam.sCamUrl = strdup( szUrl );
		m_rawFile = fopen("rawvideo.yuv", "wb" );
	}

	// Destructor
	~CTestClient() 
	{
		Close();
		if( m_rawFile )
		{
			fclose( m_rawFile );
			m_rawFile = NULL;
		}
		if( m_StreamParam.sCamUrl )
		{
			delete [] m_StreamParam.sCamUrl;
			m_StreamParam.sCamUrl = NULL;
		}
	}

	// Function to initialize library
	bool Start()
	{
		if( m_liveStreamer == NULL )
		{
			if( strstr(m_StreamParam.sCamUrl, "rtsp://") == m_StreamParam.sCamUrl )
				m_liveStreamer = new CCTLiveMediaReader( *this, m_StreamParam );
			else
				m_liveStreamer = new CCTStreamReader( *this, m_StreamParam );

			if( m_liveStreamer->Open(&CCTMMTime::GetInstance()) == 0 )
			{
				m_liveStreamer->Start();
			}
			else
			{
				delete m_liveStreamer;
				m_liveStreamer = NULL;
			}
		}
		return m_liveStreamer != NULL;
	}

	// A test function to test stream replay
	bool Replay() 
	{
		if( m_liveStreamer ) {
			return m_liveStreamer->StartFor( 1000, -1 ) == TRUE ? true : false;
		}
		return false;
	}

	// Call back function that will be called every time an image frame becomes available
	// The name onEvent is a special name and cannot be redefined for this call back.
	void onEvent( CCTStreamReader * pClient, int nEvtType, const void * cpDataObj )
	{
		if( nEvtType != ETP_PIC )
			return;

		CPicture * pPic = pClient->GetNextImage();
		if( pPic ) {
			fwrite( pPic->m_ui8DataBuf, 1, pPic->m_nBufSize, m_rawFile );
			pClient->Recycle( pPic );
		}
	}

	// Close the connection to the streamer.
	void Close()
	{
		if( m_liveStreamer )
		{
			m_liveStreamer->Close();
			delete m_liveStreamer;
			m_liveStreamer = NULL;
		}
	}

private:
	CCTStreamReader		* m_liveStreamer;
	SStreamingProperty	m_StreamParam;
	FILE				* m_rawFile;

};

/******************************************************************************
Sample main() function. Compiled sample application is to be invoked as:

> TestClient.exe rtsp://<ip-address-of-camera>/media/p0.live

This application dumps to disk the raw decoded frames from the cameras in a file
called "rawvideo.yuv"
******************************************************************************/

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

	// Tracing enabled
	INIT_SYSTRACE( LEVEL_TRACE, "trace.log" );

	// Create an instance of the class defined above with the URL
	CTestClient client( pszUrl );
	
	// If for any reason the client does not start, close client and return error.
	if( !client.Start() ) 
	{
		client.Close();
		return -1;
	}

	/*****************************************************************************
	 Once client has been started, the onEvent function of the CTestClient class
	 will be called everytime a frame becomes available.
	******************************************************************************/

	// Infinite loop to continuously capture frames
	while( 1 ) 
	{
		// map some keystrokes to provide control over console application
		if (kbhit())
		{
		  int keyVal = getch();
		  if (keyVal == 'q' || keyVal == 'Q' || keyVal == 'x' || keyVal == 'X' ||
			  keyVal == 27)
		  {
			break;
		  }
		  else if( keyVal == 'p' ) {
			client.Replay();
		  }
		}
			
		Sleep(500);
	}

	// Close client before exit
	client.Close();

	return 0;
}

