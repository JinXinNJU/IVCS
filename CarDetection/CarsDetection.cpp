#include "CarsDetection.h"

/************************
名称:carsDetector
功能：carsDetecot类构造函数，用于初始化
输入：int filetype：输入文件类型（=0视频流，=1摄像头）
	  char* filename：目标文件路径
      char* cascade_name：使用分类器的训练文件
其他：无默认的构造函数
************************/
carsDetector::carsDetector(int filetype, char* filename, char* cascade_name)
{
	//初始化
	CvMemStorage* storage = 0;
	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0 ); 
    if( !cascade ) 
    { 
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" ); 
        exit(0);
    } 
	scale = 0.7;
	file_name = filename;
	file_type = filetype;
	colors[0] = CV_RGB(255,0,0);colors[1] = CV_RGB(0,255,0);
}
/************************
名称: getCarPositions
功能：用于将位置的原始数据转化成在frame中的位置
输入：CvSeq* objects：汽车位置数据（由分类器直接返回）
	  CvPoint* carsPositions:返回的汽车在一帧中的位置
输出：CvPoint* carsPositions:返回的汽车在一帧中的位置
************************/
void carsDetector::getCarsPositions(CvSeq* objects, CvPoint* carsPositions)
{
		for (int i=0;i<objects->total;i++)
				{
					CvRect* r = (CvRect*)cvGetSeqElem( objects, i ); 
					carsPositions[i].x = cvRound((r->x + r->width*0.5)*scale); 
					carsPositions[i].y = cvRound((r->y + r->height*0.5)*scale); 
				}
}
double carsDetector::getDistance(CvPoint point1, CvPoint point2)
{
	double d = 0;
	d = ((point1.x-point2.x)*(point1.x-point2.x))+((point1.y-point2.y)*(point1.y-point2.y));
	return sqrt(d);
}
/************************
名称: frameCapture
功能：读取帧，每两帧调用一次carsDetection方法检测当前帧中的车辆
输入：无
输出：无
************************/
void carsDetector::frameCapture()
{
	 IplImage *image;

	 CvCapture* capture = 0;
	 capture = cvCaptureFromFile(file_name);
	 storage = cvCreateMemStorage(0);							//视频处理初始化

	 CvSeq* preObjects=cvCreateSeq(0,100,100,storage);			//存放前一帧的汽车位置CvSeq数据
	 CvSeq* curObjects=cvCreateSeq(0,100,100,storage);			//存放当前帧的汽车位置CvSeq数据
	 CvSeq* carsObjects=cvCreateSeq(0,100,100,storage);
	 CvPoint	preCarsPostitions[1000];						//存放前一帧汽车的位置，与preObjects对应
	 CvPoint	curCarsPostitions[1000];						//存放当前帧汽车的位置，与curObjects对应
	 for (int i=0;i<=999;i++) {
		 preCarsPostitions[i].x = preCarsPostitions[i].y = 0;
		 curCarsPostitions[i].x = curCarsPostitions[i].y = 0;
	 }															//位置数据初始化

	 CvPoint    Position;
	 int ObjectsTotal;											//汽车捕捉初始化

	 int l=-1;													//帧计数器，每两帧检测一次汽车位置
	 if (capture)
    { 
		while(1)
		{
			l++;												
			if (l%2!=1)											//偶数帧，仅输出图像和圈出车辆，不做识别	
			{
				if( !cvGrabFrame( capture ))
					   break;
				image = cvRetrieveFrame( capture );				//视频一帧输出

				if (l==1)										//在第一帧时获得preObjects的数据
				{
					preObjects = carsDetection( image );			//检测汽车返回到preObjects，作为前一帧图像
					getCarsPositions(preObjects,preCarsPostitions); //获得当前画面中检测到的车的位置
				}
				printSquaresandCircle(preObjects,image);		//圈出车辆
				if( cvWaitKey( 10 ) >= 0 )
						 break;
			}
			else                                                //奇数帧，前后帧做比对，去除误检测点
			{
				if( !cvGrabFrame( capture ))
					   break;
				image = cvRetrieveFrame( capture );				//视频一帧输出

				curObjects = carsDetection( image );			//检测汽车返回到curObjects，作为后一帧图像
				getCarsPositions(curObjects,curCarsPostitions); //获得当前画面中检测到的车的位置

				for (int i=0;i<curObjects->total;i++)		
				{
					//比对前后两帧中获得车的位置，如果在后一帧中出现了一个前一帧中没有出现的检测点，则将其删除
					bool flag = false;
					for (int j=0;j<preObjects->total;j++)
					{
						if (getDistance(preCarsPostitions[j],curCarsPostitions[i])<20)
							flag = true;
					}
					if (flag==false) 
						{
							cvSeqRemove(curObjects,i);
						 }
				}

				printSquaresandCircle(curObjects,image);		//圈出车辆

				preObjects = curObjects;						//下一次循环当前帧变为前一帧
				for (int q=0;q<=999;q++)
					preCarsPostitions[q] = curCarsPostitions[q];//同时移动车辆位置信息
				if( cvWaitKey( 10 ) >= 0 )
						 break;
			}
		}
		
	 }
	 	cvReleaseCapture( &capture );
		cvDestroyWindow("result");
}
/************************
名称: carsDetecion
功能：输入一帧，分别调用imagePreparation和cvHaarDetectObjects检测车辆
输入：IplImage* img: 当前检测帧
输出：CvSeq* objects:汽车位置CvSeq形式的数据
************************/
CvSeq* carsDetector::carsDetection(IplImage* img )			//汽车追踪，输入一帧，返回一组检测到的车的序列
{ 
    //Detect objects if any 
    IplImage* small_img = imagePreparation(img);			//将取得的一帧先做图像预处理
    cvClearMemStorage(storage); 
    //double t = (double)cvGetTickCount();					//检测时间
    CvSeq* objects = cvHaarDetectObjects(small_img, cascade,storage,1.1,2, 0/*CV_HAAR_DO_CANNY_PRUNING*/, cvSize(20,20));
															//用Haar分类器做检测，返回CvSeq* 形式的原始位置数据
    //t = (double)cvGetTickCount() - t; 
    //printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
    cvReleaseImage(&small_img); 
	return objects;
}
/************************
名称: imagePreparation
功能：图像预处理，转灰度图、提取前景，直方图均衡化和缩放
输入：IplImage* img: 需要做预处理的帧
输出：IplImage* small_img：经过处理的帧
************************/
IplImage* carsDetector::imagePreparation(IplImage* img)
{
	  //Image Preparation 
    IplImage* gray = cvCreateImage(cvSize(img->width,img->height),8,1); 
	IplImage* graytemp = cvCreateImage(cvSize(img->width,img->height),8,1);
    IplImage* small_img=cvCreateImage(cvSize(cvRound(img->width/scale),cvRound(img->height/scale)),8,1); 
	foreground = cvCreateImage(cvSize(img->width,img->height),8,1); 

    cvCvtColor(img,gray, CV_BGR2GRAY);						//转灰度图
	mog(Mat(gray),Mat(foreground),0.005);					//检测运动前景
	//erode(Mat(foreground), Mat(foreground), cv::Mat());	//腐蚀操作
	//GaussianBlur(Mat(foreground),Mat(foreground),Size(3,3),0,0);//高斯滤波，去除前景的噪声
	
	cvShowImage("originalimage",gray);
	Mat graymat = Mat(gray);
	graymat = Mat(gray).mul(Mat(foreground));
	gray = &IplImage(graymat);								//前景与原图相乘，提取运动物体
	//cvAddWeighted(gray,1,foreground,0.5,0,gray);			//运动前景与原图混合
	//cvThreshold(gray,gray,100,255,CV_THRESH_TOZERO);		//二值化
    cvResize(gray, small_img, CV_INTER_LINEAR);				//调整图片大小
	cvEqualizeHist(small_img,small_img);					//直方图均衡

	cvShowImage("foreground",foreground);
	cvShowImage("afterpreparation",gray);

	 //cvReleaseImage(&gray); 
	
	return small_img;
}
/************************
名称: printSquaresandCircle
功能：根据输入的位置原始数据在帧上圈出汽车
输入：CvSeq* objects： 汽车位置原始数据
	  IplImage* img：用于画图的帧
输出：无
************************/
void carsDetector::printSquaresandCircle(CvSeq* objects,IplImage* img)
{
	int distancecolor = 0;									//根据不同距离显示不同颜色的框

    for(int i=0;i<(objects? objects->total:0);++i) 
    { 
        CvRect* r= (CvRect*)cvGetSeqElem(objects,i); 
		if (r->width>50&&r->height>75) distancecolor = 0; else distancecolor = 1;
        cvRectangle(img, cvPoint(r->x*scale,r->y*scale), cvPoint((r->x+r->width)*scale,(r->y+r->height)*scale), colors[distancecolor]); 
    } 
															//画方框
    for( int i = 0; i < (objects? objects->total : 0); i++ ) 
    { 
        CvRect* r= (CvRect*)cvGetSeqElem(objects,i);
        CvPoint center; 
        int radius; 
        center.x = cvRound((r->x + r->width*0.5)*scale); 
        center.y = cvRound((r->y + r->height*0.5)*scale); 
        radius = cvRound((r->width + r->height)*0.25*scale); 
		if (radius>50) distancecolor = 0; else distancecolor = 1;
        cvCircle( img, center, radius, colors[distancecolor], 1, 1, 0 ); 
    }
															//画圆
    cvShowImage( "result", img ); 
}
