#include <opencv2/opencv.hpp>
#include <raspicam_cv.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include  <wiringPi.h>

using namespace std;
using namespace cv;
using namespace raspicam;

Mat frame, Matrix, framePers, frameGray, frameThresh, frameEdge, frameFinal, frameFinalDuplicate;
Mat ROILane;
int LeftLanePos, RightLanePos, frameCenter, laneCenter, Result;

RaspiCam_Cv Camera;

stringstream ss;

vector<int> histrogramLane;

Point2f Source[] = {Point2f(53,194),Point2f(326,194),Point2f(33,228), Point2f(348,228)};
Point2f Destination[] = {Point2f(80,0),Point2f(280,0),Point2f(80,240), Point2f(280,240)};


 void Setup ( int argc,char **argv, RaspiCam_Cv &Camera )
  {
    Camera.set ( CAP_PROP_FRAME_WIDTH,  ( "-w",argc,argv,360 ) );
    Camera.set ( CAP_PROP_FRAME_HEIGHT,  ( "-h",argc,argv,240 ) );
    Camera.set ( CAP_PROP_BRIGHTNESS, ( "-br",argc,argv,50 ) );
    Camera.set ( CAP_PROP_CONTRAST ,( "-co",argc,argv,50 ) );
    Camera.set ( CAP_PROP_SATURATION,  ( "-sa",argc,argv,50 ) );
    Camera.set ( CAP_PROP_GAIN,  ( "-g",argc,argv ,50 ) );
    Camera.set ( CAP_PROP_FPS,  ( "-fps",argc,argv,100));

}

void Capture()
{
    Camera.grab();
    Camera.retrieve( frame);
    flip(frame,frame,-1);
    cvtColor(frame, frame, COLOR_BGR2RGB);
}

void Perspective()
{
    Matrix = getPerspectiveTransform(Source, Destination);
	warpPerspective(frame, framePers, Matrix, Size(360,240));
	line(frame,Source[0], Source[1], Scalar(0,0,255), 2);
	line(frame,Source[1], Source[3], Scalar(0,0,255), 2);
	line(frame,Source[3], Source[2], Scalar(0,0,255), 2);
	line(frame,Source[2], Source[0], Scalar(0,0,255), 2);
	
	
	
}

void Threshold()
{
	/*cvtColor(framePers, frameGray, COLOR_RGB2GRAY);
	inRange(frameGray, 25, 255, frameThresh);
	Canny(frameGray,frameEdge, 400, 700, 3, false);
	add(frameThresh, frameEdge, frameFinal);
	cvtColor(frameFinal, frameFinal, COLOR_GRAY2RGB);
	cvtColor(frameFinal, frameFinalDuplicate, COLOR_RGB2BGR);   //used in histrogram function only*/
	
	
	
	Mat smoothedImage, houghedImg,cannyEdged;
    
	cvtColor(perispctiviewFr, framePerspecGray, COLOR_RGB2GRAY);
	framePerspecGray = smoothImage(framePerspecGray);
	int maxThreshold = getThresholds(framePerspecGray).back();
	threshold(framePerspecGray, frameThresh, maxThreshold, 255, THRESH_BINARY);
	//inRange(framePerspecGray, maxThreshold ,255, frameThresh);
	Canny(frameThresh,frameEdge, 450, 700, 3, false);
	add(frameThresh, frameEdge, frameEdge);
	//frameEdge = smoothImage(frameEdge);
	//frameEdge = smoothImage(frameEdge);
	//houghedImg = getHoughLines(frameEdge);
	
	
	////cvtColor(houghedImg, frameThresh,COLOR_BGR2GRAY);
	
	frameFinalDuplicate = frameEdge.clone();          //houghed image is in bgr    //used in histrogram function only
	cvtColor(frameFinalDuplicate,frameFinalDuplicate,GRAY2BGR);
	//cvtColor(houghedImg, frameFinal, COLOR_BGR2RGB);
	cvtColor(frameEdge, frameFinal, COLOR_GRAY2RGB);
}
void Histrogram()
{
    histrogramLane.resize(360);
    histrogramLane.clear();
    
    for(int i=0; i<360; i++)       //frame.size().width = 400
    {
	ROILane = frameFinalDuplicate(Rect(i,120,1,120));   // double check
	divide(255, ROILane, ROILane);
	histrogramLane.push_back((int)(sum(ROILane)[0])); 
    }
}

void LaneFinder()
{
    vector<int>:: iterator LeftPtr;
    LeftPtr = max_element(histrogramLane.begin(), histrogramLane.begin() + 135);
    LeftLanePos = distance(histrogramLane.begin(), LeftPtr); 
    
    vector<int>:: iterator RightPtr;
    RightPtr = max_element(histrogramLane.begin() +225, histrogramLane.end());
    RightLanePos = distance(histrogramLane.begin(), RightPtr);
    
    line(frameFinal, Point2f(LeftLanePos, 0), Point2f(LeftLanePos, 240), Scalar(0, 255,0), 2);
    line(frameFinal, Point2f(RightLanePos, 0), Point2f(RightLanePos, 240), Scalar(0,255,0), 2); 
}

void LaneCenter()
{
    laneCenter = (RightLanePos-LeftLanePos)/2 +LeftLanePos;
    frameCenter = 178;
    
    line(frameFinal, Point2f(laneCenter,0), Point2f(laneCenter,240), Scalar(0,255,0), 3);
    line(frameFinal, Point2f(frameCenter,0), Point2f(frameCenter,240), Scalar(255,0,0), 3);
     Result = laneCenter-frameCenter;
}

void TrafficLight()  
{
    
 int iLowH = 170;
 int iHighH = 179;
 int iLowS = 150; 
 int iHighS = 255;
 int iLowV = 60;
 int iHighV = 255;
    
    // Applying the threshold values for RED
    inRange(traffic_HSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), traffic_threshold);
    
     //Removing small objects (Morphological Opening)
  erode(traffic_threshold, traffic_threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
  dilate( traffic_threshold, traffic_threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

  //Removing small holes from the foreground  (Morphological Closing)
  dilate( traffic_threshold, traffic_threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
  erode(traffic_threshold, traffic_threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

  //Calculate the moments of the thresholded image. 
  Moments oMoments = moments(traffic_threshold);
  
  // Finding the center of the red color in case we want to find the position of the red light
  double dM01 = oMoments.m01;
  double dM10 = oMoments.m10;
  
        Red = oMoments.m00; // Finding the number of Red pixels.
	
    
}
vector<int> getThresholds(Mat srcImgGray)
{
	int histogram[256] = { 0 };
	int pixelsCount = srcImgGray.cols * srcImgGray.rows;

	for (int y = 0; y < srcImgGray.rows; y++)
	{
		for (int x = 0; x < srcImgGray.cols; x++)
		{
			uchar value = srcImgGray.at<uchar>(y, x);
			histogram[value]++;
		}
	}

	double c = 0;
	double Mt = 0;

	double p[256] = { 0 };
	for (int i = 0; i < 256; i++)
	{
		p[i] = (double)histogram[i] / (double)pixelsCount;
		Mt += i * p[i];
	}

	int optimalTreshold1 = 0;
	int optimalTreshold2 = 0;
	int optimalTreshold3 = 0;

	double maxBetweenVar = 0;

	double w0 = 0;
	double m0 = 0;
	double c0 = 0;
	double p0 = 0;

	double w1 = 0;
	double m1 = 0;
	double c1 = 0;
	double p1 = 0;

	double w2 = 0;
	double m2 = 0;
	double c2 = 0;
	double p2 = 0;
	for (int tr1 = 0; tr1 < 256; tr1++)
	{
		p0 += p[tr1];
		w0 += (tr1 * p[tr1]);
		if (p0 != 0)
		{
			m0 = w0 / p0;
		}

		c0 = p0 * (m0 - Mt) * (m0 - Mt);

		c1 = 0;
		w1 = 0;
		m1 = 0;
		p1 = 0;
		for (int tr2 = tr1 + 1; tr2 < 256; tr2++)
		{

			p1 += p[tr2];
			w1 += (tr2 * p[tr2]);
			if (p1 != 0)
			{
				m1 = w1 / p1;
			}

			c1 = p1 * (m1 - Mt) * (m1 - Mt);


			c2 = 0;
			w2 = 0;
			m2 = 0;
			p2 = 0;
			for (int tr3 = tr2 + 1; tr3 < 256; tr3++)
			{

				p2 += p[tr3];
				w2 += (tr3 * p[tr3]);
				if (p2 != 0)
				{
					m2 = w2 / p2;
				}

				c2 = p2 * (m2 - Mt) * (m2 - Mt);

				double p3 = 1 - (p0 + p1 + p2);
				double w3 = Mt - (w0 + w1 + w2);
				double m3 = w3 / p3;
				double c3 = p3 * (m3 - Mt) * (m3 - Mt);

				double c = c0 + c1 + c2 + c3;

				if (maxBetweenVar < c)
				{
					maxBetweenVar = c;
					optimalTreshold1 = tr1;
					optimalTreshold2 = tr2;
					optimalTreshold3 = tr3;
				}
			}
		}
	}
	cout << "Threshold values" << endl << "Threshold 1: " << optimalTreshold1 << endl << "Threshold 2: " << optimalTreshold2 << endl << "Threshold 3: " << optimalTreshold3 << endl;

	vector<int> thresholds;
	thresholds.push_back(optimalTreshold1);
	thresholds.push_back(optimalTreshold2);
	thresholds.push_back(optimalTreshold3);

	return thresholds;


}
Mat smoothImage(Mat srcImage)
{
	Mat dstImage;
	const Size kernelSize = Size(9, 9);
	GaussianBlur(srcImage, dstImage, kernelSize, 0);
	//Try to fill the gaps 
	Mat kernel = Mat::ones(15, 15, CV_8U);
	dilate(dstImage, dstImage, kernel);
	erode(dstImage, dstImage, kernel);
	morphologyEx(dstImage, dstImage, MORPH_CLOSE, kernel);

	return dstImage;
}
Mat getHoughLines(Mat image)
{
	Mat houghImg;
	cvtColor(image, houghImg, COLOR_GRAY2BGR);
	// Probabilistic Line Transform
	vector<Vec4i> linesP; // will hold the results of the detection
	HoughLinesP(image, linesP, 1, CV_PI / 180, 40, 50, 100); // runs the actual detection
	// Draw the lines
	for (size_t i = 0; i < linesP.size(); i++)
	{
		Vec4i l = linesP[i];
		line(houghImg, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 10), 2, LINE_AA);
	}
	return houghImg;
}


int main(int argc,char **argv)
{
    wiringPiSetup();
    pinMode(21, OUTPUT);
    pinMode(22, OUTPUT);
    pinMode(23, OUTPUT);
    pinMode(24, OUTPUT);
	
	Setup(argc, argv, Camera);
	cout<<"Connecting to camera"<<endl;
	if (!Camera.open())
	{
		
	cout<<"Failed to Connect"<<endl;
     }
     
     cout<<"Camera Id = "<<Camera.getId()<<endl;
     
     
     
    
    while(1)
    {
	auto start = std::chrono::system_clock::now();

    Capture();
    Perspective();
    Threshold();
    Histrogram();
    LaneFinder();
    LaneCenter();
    
    
    if (Result == 0)
    {
	digitalWrite(21, 0);
	digitalWrite(22, 0);    //decimal = 0
	digitalWrite(23, 0);
	digitalWrite(24, 0);
	cout<<"Forward"<<endl;
    }
    
        
    else if (Result >0 && Result <10)
    {
	digitalWrite(21, 1);
	digitalWrite(22, 0);    //decimal = 1
	digitalWrite(23, 0);
	digitalWrite(24, 0);
	cout<<"Right1"<<endl;
    }
    
        else if (Result >=10 && Result <20)
    {
	digitalWrite(21, 0);
	digitalWrite(22, 1);    //decimal = 2
	digitalWrite(23, 0);
	digitalWrite(24, 0);
	cout<<"Right2"<<endl;
    }
    
        else if (Result >20)
    {
	digitalWrite(21, 1);
	digitalWrite(22, 1);    //decimal = 3
	digitalWrite(23, 0);
	digitalWrite(24, 0);
	cout<<"Right3"<<endl;
    }
    
        else if (Result <0 && Result >-10)
    {
	digitalWrite(21, 0);
	digitalWrite(22, 0);    //decimal = 4
	digitalWrite(23, 1);
	digitalWrite(24, 0);
	cout<<"Left1"<<endl;
    }
    
        else if (Result <=-10 && Result >-20)
    {
	digitalWrite(21, 1);
	digitalWrite(22, 0);    //decimal = 5
	digitalWrite(23, 1);
	digitalWrite(24, 0);
	cout<<"Left2"<<endl;
    }
    
        else if (Result <-20)
    {
	digitalWrite(21, 0);
	digitalWrite(22, 1);    //decimal = 6
	digitalWrite(23, 1);
	digitalWrite(24, 0);
	cout<<"Left3"<<endl;
    }
    
    
    ss.str(" ");
    ss.clear();
    ss<<"Result = "<<Result;
    putText(frame, ss.str(), Point2f(1,50), 0,1, Scalar(0,0,255), 2);
    
    
    namedWindow("orignal", WINDOW_KEEPRATIO);
    moveWindow("orignal", 0, 100);
    resizeWindow("orignal", 460, 345);
    imshow("orignal", frame);
    
    namedWindow("Perspective", WINDOW_KEEPRATIO);
    moveWindow("Perspective", 460, 100);
    resizeWindow("Perspective", 460, 345);
   imshow("Perspective", framePers);
    
    namedWindow("Final", WINDOW_KEEPRATIO);
    moveWindow("Final", 920, 100);
    resizeWindow("Final", 460, 345);
    imshow("Final", frameFinal);
    
    
    
    waitKey(1);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    
    float t = elapsed_seconds.count();
    int FPS = 1/t;
   // cout<<"FPS = "<<FPS<<endl;
    
    }

    
    return 0;
     
}
