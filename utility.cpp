// canny edge operation on input image
#include "utility.h"
void canny(cv::Mat& img, cv::Mat& out){
    // Convert to gray
    if(img.channels() == 3)
        cv::cvtColor(img, out, CV_BGR2GRAY);
    // Compute Canny edges
    cv::Canny(out, out, 100, 200);
    // Invert the image
    cv::threshold(out, out, 128, 255, cv::THRESH_BINARY_INV);
}
// make the border of the the image as a special color
void makeSpecialColorBorder(cv::Mat& output, int borderWidth, cv::Scalar color){
    // top bar
    for (int i = 0; i < output.cols; i++){
        for(int j = 0; j < borderWidth; j++){
            assignValueOnImage(output, j, i, color);
        }
    }
    // left bar
    for(int i = 0; i < borderWidth; i++){
        for(int j = 0; j < output.rows; j++){
            assignValueOnImage(output, j, i, color);
        }
    }
    // bottom bar
    for(int i = 0; i < output.cols; i++){
        for(int j = output.rows - borderWidth; j < output.rows; j ++){
            assignValueOnImage(output, j, i, color);
        }
    }
    // right bar
    for(int i = output.cols - borderWidth; i < output.cols; i++){
        for(int j = 0; j < output.rows; j++){
            assignValueOnImage(output, j, i, color);
        }
    }

}
// assign color/255 to a specific location on color/gray image
void assignValueOnImage(cv::Mat& image, int row, int col, cv::Scalar color){
    if(image.channels() == 1){
        image.at<uchar>(row, col) = 255;
    }else if(image.channels() == 3){
        image.at<cv::Vec3b>(row, col)[0] = color[0];
        image.at<cv::Vec3b>(row, col)[1] = color[1];
        image.at<cv::Vec3b>(row, col)[2] = color[2];
    }
}
// only draw one Trajectory
void drawTrajectory(std::vector<cv::Point2f> points, cv::Mat& img, cv::Scalar color){	
	if(points.size() > 0){
		std::vector<cv::Point2f>::iterator it = points.begin();
		for(int i = 0; i < points.size() - 1; i++){
			cv::Point2f point0 = *it;
			cv::Point2f point1 = *(++it);
			cv::line(img, point0, point1, color);
		}
		cv::circle(img, *it, 3, color, -1);
	}
}
// read the directories files names
void readDirectory( const std::string& directoryName, std::vector<std::string>& filenames, bool addDirectoryName )
{
    filenames.clear();
    
#ifdef WIN32
    struct _finddata_t s_file;
    std::string str = directoryName + "\\*.*";
    
	intptr_t h_file = _findfirst( str.c_str(), &s_file );
	if( h_file != static_cast<intptr_t>(-1.0) )
    {
        do
        {
            if( addDirectoryName )
                filenames.push_back(directoryName + "\\" + s_file.name);
            else
                filenames.push_back((std::string)s_file.name);
        }
        while( _findnext( h_file, &s_file ) == 0 );
    }
    _findclose( h_file );
#else
    DIR* dir = opendir( directoryName.c_str() );
    if( dir != NULL )
    {
        struct dirent* dent;
        while( (dent = readdir(dir)) != NULL )
        {
            if( addDirectoryName )
                filenames.push_back( directoryName + "/" + string(dent->d_name) );
            else
                filenames.push_back( string(dent->d_name) );
        }
    }
#endif
    
    sort( filenames.begin(), filenames.end() );
}

// calculate foreground percent according to a bkgd binary images
double calForePercent(const cv::Mat& output){
    cv::Scalar forePixelSumPerChannel = cv::sum(output);
    double allPixels = output.cols * output.rows * output.channels();
    double foreGroundPixel = 0;
    for(int i = 0; i < output.channels(); i++){
        //        cout << "Channel " << i << "percent " << "percent " << forePixelSumPerChannel[i] << endl;
        foreGroundPixel += forePixelSumPerChannel[i];
    }
    return foreGroundPixel / allPixels;
}



//compare covariance matricies
float compareCov(cv::Mat a, cv::Mat b)
{
	cv::Mat eigen_values;
	cv::eigen(a.inv() * b, eigen_values);
	cv::Mat ln;
	cv::Mat ln2;
	cv::log(eigen_values, ln);
	cv::multiply(ln,ln,ln2);
	cv::Scalar temp = cv::sum(ln2);
	return temp[0];
}




//calculate hog features
//based on code from http://www.mathworks.com/matlabcentral/fileexchange/33863
cv::Mat HOGfeatures(cv::Mat & pixels, int nb_bins, double cheight, double cwidth, int block_rows, int block_cols, bool orient, double clip_val)
{
    //const double PI = 3.1415926536;
	bool grayscale;

	//img must be uchar
	if (pixels.type() == CV_8UC1)
	{
		grayscale = 1;
	}
	else if (pixels.type() == CV_8UC3)
	{
		grayscale = 0;
	}
	else
	{
		std::cerr << "Image is not uchar, cannot calculate HOG!!!" << std::endl;
		return cv::Mat();
	}

	/*if (!pixels.isContinuous())
	{
		cv::Mat temp = pixels.clone();
		pixels = temp;
		if (!pixels.isContinuous())
		{
			std::cerr << "Memory error, cannot allocate a continuous array for HOG calc!" << std::endl;
			return cv::Mat();
		}
	}
	unsigned char * pixels = img.data;*/
	
	int img_width  = pixels.cols;
	int img_height = pixels.rows;

    int hist1= 2+ceil(-0.5 + img_height/cheight);
    int hist2= 2+ceil(-0.5 + img_width/cwidth);

	double bin_size = (1+(orient==1))*PI/nb_bins;

	cv::Mat features((hist1-2-(block_rows-1))*(hist2-2-(block_cols-1))*nb_bins*block_rows*block_cols, 1, CV_32FC1);

    float dx[3], dy[3], grad_or, grad_mag, temp_mag;
    float Xc, Yc, Oc, block_norm;
    int x1, x2, y1, y2, bin1, bin2;
    int des_indx = 0;
    
    std::vector<std::vector<std::vector<double> > > h(hist1, std::vector<std::vector<double> > (hist2, std::vector<double> (nb_bins, 0.0) ) );    
    std::vector<std::vector<std::vector<double> > > block(block_rows, std::vector<std::vector<double> > (block_cols, std::vector<double> (nb_bins, 0.0) ) );

	std::vector<cv::Mat> pixel_vec;

	if (!grayscale)
	{
		cv::split(pixels, pixel_vec);
	}

	//Calculate gradients (zero padding)
    

    for(unsigned int y=0; y<img_height; y++) 
	{
        for(unsigned int x=0; x<img_width; x++) 
		{
            if (grayscale == 1)
			{
                if (x==0) 
				{
					dx[0] = pixels.at<unsigned char>(y , x+1);
				}
                else
				{
                    if (x==img_width-1) 
					{
						dx[0] = -pixels.at<unsigned char>(y , x-1);
					}
					else
					{
						dx[0] = pixels.at<unsigned char>(y, x+1) - pixels.at<unsigned char>(y , x-1);
					}
                }
                if (y==0)
				{
					dy[0] = -pixels.at<unsigned char>(y+1, x);
				}
				else{
                    if (y==img_height-1) 
					{
						dy[0] = pixels.at<unsigned char>(y-1, x);
					}
					else
					{
						dy[0] = -pixels.at<unsigned char>(y+1, x) + pixels.at<unsigned char>(y-1, x);
					}
                }
            }
            else
			{
                if(x==0)
				{
                    dx[0] = pixel_vec[0].at<unsigned char>(y , x+1);
                    dx[1] = pixel_vec[1].at<unsigned char>(y , x+1);
                    dx[2] = pixel_vec[2].at<unsigned char>(y , x+1);                    
                }
                else
				{
                    if (x==img_width-1)
					{
                        dx[0] = -pixel_vec[0].at<unsigned char>(y , x-1);                        
                        dx[1] = -pixel_vec[1].at<unsigned char>(y , x-1);
                        dx[2] = -pixel_vec[2].at<unsigned char>(y , x-1);
                    }
                    else
					{
                        dx[0] = pixel_vec[0].at<unsigned char>(y, x+1) - pixel_vec[0].at<unsigned char>(y , x-1);
                        dx[1] = pixel_vec[1].at<unsigned char>(y, x+1) - pixel_vec[1].at<unsigned char>(y , x-1);
                        dx[2] = pixel_vec[2].at<unsigned char>(y, x+1) - pixel_vec[2].at<unsigned char>(y , x-1);
                        
                    }
                }
                if(y==0)
				{
                    dy[0] = -pixel_vec[0].at<unsigned char>(y+1, x);
                    dy[1] = -pixel_vec[1].at<unsigned char>(y+1, x);
                    dy[2] = -pixel_vec[2].at<unsigned char>(y+1, x);
                }
                else
				{
                    if (y==img_height-1)
					{
                        dy[0] = pixel_vec[0].at<unsigned char>(y-1, x);
                        dy[1] = pixel_vec[1].at<unsigned char>(y-1, x);
                        dy[2] = pixel_vec[2].at<unsigned char>(y-1, x);
                    }
                    else
					{
                        dy[0] = -pixel_vec[0].at<unsigned char>(y+1, x) + pixel_vec[0].at<unsigned char>(y-1, x);
                        dy[1] = -pixel_vec[1].at<unsigned char>(y+1, x) + pixel_vec[1].at<unsigned char>(y-1, x);
                        dy[2] = -pixel_vec[2].at<unsigned char>(y+1, x) + pixel_vec[2].at<unsigned char>(y-1, x);
                    }
                }
            }
            
            grad_mag = sqrt(dx[0]*dx[0] + dy[0]*dy[0]);
            grad_or= atan2(dy[0], dx[0]);
            
            if (grayscale == 0)
			{
                temp_mag = grad_mag;
                for (unsigned int cli=1;cli<3;++cli)
				{
                    temp_mag= sqrt(dx[cli]*dx[cli] + dy[cli]*dy[cli]);
                    if (temp_mag>grad_mag)
					{
                        grad_mag=temp_mag;
                        grad_or= atan2(dy[cli], dx[cli]);
                    }
                }
            }
            
            if (grad_or<0) grad_or+=PI + (orient==1) * PI;

            // trilinear interpolation
            
            bin1 = (int)floor(0.5 + grad_or/bin_size) - 1;
            bin2 = bin1 + 1;
            x1   = (int)floor(0.5+ x/cwidth);
            x2   = x1+1;
            y1   = (int)floor(0.5+ y/cheight);
            y2   = y1 + 1;
            
            Xc = (x1+1-1.5)*cwidth + 0.5;
            Yc = (y1+1-1.5)*cheight + 0.5;
            
            Oc = (bin1+1+1-1.5)*bin_size;
            
            if (bin2==nb_bins){
                bin2=0;
            }
            if (bin1<0){
                bin1=nb_bins-1;
            }            
           
            h[y1][x1][bin1]= h[y1][x1][bin1] + grad_mag*(1-((x+1-Xc)/cwidth))*(1-((y+1-Yc)/cheight))*(1-((grad_or-Oc)/bin_size));
            h[y1][x1][bin2]= h[y1][x1][bin2] + grad_mag*(1-((x+1-Xc)/cwidth))*(1-((y+1-Yc)/cheight))*(((grad_or-Oc)/bin_size));
            h[y2][x1][bin1]= h[y2][x1][bin1] + grad_mag*(1-((x+1-Xc)/cwidth))*(((y+1-Yc)/cheight))*(1-((grad_or-Oc)/bin_size));
            h[y2][x1][bin2]= h[y2][x1][bin2] + grad_mag*(1-((x+1-Xc)/cwidth))*(((y+1-Yc)/cheight))*(((grad_or-Oc)/bin_size));
            h[y1][x2][bin1]= h[y1][x2][bin1] + grad_mag*(((x+1-Xc)/cwidth))*(1-((y+1-Yc)/cheight))*(1-((grad_or-Oc)/bin_size));
            h[y1][x2][bin2]= h[y1][x2][bin2] + grad_mag*(((x+1-Xc)/cwidth))*(1-((y+1-Yc)/cheight))*(((grad_or-Oc)/bin_size));
            h[y2][x2][bin1]= h[y2][x2][bin1] + grad_mag*(((x+1-Xc)/cwidth))*(((y+1-Yc)/cheight))*(1-((grad_or-Oc)/bin_size));
            h[y2][x2][bin2]= h[y2][x2][bin2] + grad_mag*(((x+1-Xc)/cwidth))*(((y+1-Yc)/cheight))*(((grad_or-Oc)/bin_size));
        }
    }

	//Block normalization

    for(unsigned int x=1; x<hist2-block_cols; x++)
	{
        for (unsigned int y=1; y<hist1-block_rows; y++)
		{
            
            block_norm=0;
            for (unsigned int i=0; i<block_rows; i++)
			{
                for(unsigned int j=0; j<block_cols; j++)
				{
                    for(unsigned int k=0; k<nb_bins; k++)
					{
                        block_norm+=h[y+i][x+j][k]*h[y+i][x+j][k];
                    }
                }
            }
            
            block_norm=sqrt(block_norm);
            for (unsigned int i=0; i<block_rows; i++)
			{
                for(unsigned int j=0; j<block_cols; j++)
				{
                    for(unsigned int k=0; k<nb_bins; k++)
					{
                        if (block_norm>0)
						{
                            block[i][j][k]=h[y+i][x+j][k]/block_norm;
                            if (block[i][j][k]>clip_val) block[i][j][k]=clip_val;
                        }
                    }
                }
            }
            
            block_norm=0;
            for (unsigned int i=0; i<block_rows; i++)
			{
                for(unsigned int j=0; j<block_cols; j++)
				{
                    for(unsigned int k=0; k<nb_bins; k++)
					{
                        block_norm+=block[i][j][k]*block[i][j][k];
                    }
                }
            }
            
            block_norm=sqrt(block_norm);
            for (unsigned int i=0; i<block_rows; i++)
			{
                for(unsigned int j=0; j<block_cols; j++)
				{
                    for(unsigned int k=0; k<nb_bins; k++)
					{
                        if (block_norm>0) 
						{
							features.at<float>(des_indx,0) = block[i][j][k]/block_norm;
						}
                        else 
						{
							features.at<float>(des_indx,0) = 0.0;
						}
						des_indx++;
                    }
                }
            }
        }
    }

	return features;
}