#include "CarsDetection.h"

/************************
����:carsDetector
���ܣ�carsDetecot�๹�캯�������ڳ�ʼ��
���룺int filetype�������ļ����ͣ�=0��Ƶ����=1����ͷ��
	  char* filename��Ŀ���ļ�·��
      char* cascade_name��ʹ�÷�������ѵ���ļ�
��������Ĭ�ϵĹ��캯��
************************/
carsDetector::carsDetector(int filetype, char* filename, char* cascade_name)
{
	//��ʼ��
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
����: getCarPositions
���ܣ����ڽ�λ�õ�ԭʼ����ת������frame�е�λ��
���룺CvSeq* objects������λ�����ݣ��ɷ�����ֱ�ӷ��أ�
	  CvPoint* carsPositions:���ص�������һ֡�е�λ��
�����CvPoint* carsPositions:���ص�������һ֡�е�λ��
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
����: frameCapture
���ܣ���ȡ֡��ÿ��֡����һ��carsDetection������⵱ǰ֡�еĳ���
���룺��
�������
************************/
void carsDetector::frameCapture()
{
	 IplImage *image;

	 CvCapture* capture = 0;
	 capture = cvCaptureFromFile(file_name);
	 storage = cvCreateMemStorage(0);							//��Ƶ�����ʼ��

	 CvSeq* preObjects=cvCreateSeq(0,100,100,storage);			//���ǰһ֡������λ��CvSeq����
	 CvSeq* curObjects=cvCreateSeq(0,100,100,storage);			//��ŵ�ǰ֡������λ��CvSeq����
	 CvSeq* carsObjects=cvCreateSeq(0,100,100,storage);
	 CvPoint	preCarsPostitions[1000];						//���ǰһ֡������λ�ã���preObjects��Ӧ
	 CvPoint	curCarsPostitions[1000];						//��ŵ�ǰ֡������λ�ã���curObjects��Ӧ
	 for (int i=0;i<=999;i++) {
		 preCarsPostitions[i].x = preCarsPostitions[i].y = 0;
		 curCarsPostitions[i].x = curCarsPostitions[i].y = 0;
	 }															//λ�����ݳ�ʼ��

	 CvPoint    Position;
	 int ObjectsTotal;											//������׽��ʼ��

	 int l=-1;													//֡��������ÿ��֡���һ������λ��
	 if (capture)
    { 
		while(1)
		{
			l++;												
			if (l%2!=1)											//ż��֡�������ͼ���Ȧ������������ʶ��	
			{
				if( !cvGrabFrame( capture ))
					   break;
				image = cvRetrieveFrame( capture );				//��Ƶһ֡���

				if (l==1)										//�ڵ�һ֡ʱ���preObjects������
				{
					preObjects = carsDetection( image );			//����������ص�preObjects����Ϊǰһ֡ͼ��
					getCarsPositions(preObjects,preCarsPostitions); //��õ�ǰ�����м�⵽�ĳ���λ��
				}
				printSquaresandCircle(preObjects,image);		//Ȧ������
				if( cvWaitKey( 10 ) >= 0 )
						 break;
			}
			else                                                //����֡��ǰ��֡���ȶԣ�ȥ�������
			{
				if( !cvGrabFrame( capture ))
					   break;
				image = cvRetrieveFrame( capture );				//��Ƶһ֡���

				curObjects = carsDetection( image );			//����������ص�curObjects����Ϊ��һ֡ͼ��
				getCarsPositions(curObjects,curCarsPostitions); //��õ�ǰ�����м�⵽�ĳ���λ��

				for (int i=0;i<curObjects->total;i++)		
				{
					//�ȶ�ǰ����֡�л�ó���λ�ã�����ں�һ֡�г�����һ��ǰһ֡��û�г��ֵļ��㣬����ɾ��
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

				printSquaresandCircle(curObjects,image);		//Ȧ������

				preObjects = curObjects;						//��һ��ѭ����ǰ֡��Ϊǰһ֡
				for (int q=0;q<=999;q++)
					preCarsPostitions[q] = curCarsPostitions[q];//ͬʱ�ƶ�����λ����Ϣ
				if( cvWaitKey( 10 ) >= 0 )
						 break;
			}
		}
		
	 }
	 	cvReleaseCapture( &capture );
		cvDestroyWindow("result");
}
/************************
����: carsDetecion
���ܣ�����һ֡���ֱ����imagePreparation��cvHaarDetectObjects��⳵��
���룺IplImage* img: ��ǰ���֡
�����CvSeq* objects:����λ��CvSeq��ʽ������
************************/
CvSeq* carsDetector::carsDetection(IplImage* img )			//����׷�٣�����һ֡������һ���⵽�ĳ�������
{ 
    //Detect objects if any 
    IplImage* small_img = imagePreparation(img);			//��ȡ�õ�һ֡����ͼ��Ԥ����
    cvClearMemStorage(storage); 
    //double t = (double)cvGetTickCount();					//���ʱ��
    CvSeq* objects = cvHaarDetectObjects(small_img, cascade,storage,1.1,2, 0/*CV_HAAR_DO_CANNY_PRUNING*/, cvSize(20,20));
															//��Haar����������⣬����CvSeq* ��ʽ��ԭʼλ������
    //t = (double)cvGetTickCount() - t; 
    //printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
    cvReleaseImage(&small_img); 
	return objects;
}
/************************
����: imagePreparation
���ܣ�ͼ��Ԥ����ת�Ҷ�ͼ����ȡǰ����ֱ��ͼ���⻯������
���룺IplImage* img: ��Ҫ��Ԥ�����֡
�����IplImage* small_img�����������֡
************************/
IplImage* carsDetector::imagePreparation(IplImage* img)
{
	  //Image Preparation 
    IplImage* gray = cvCreateImage(cvSize(img->width,img->height),8,1); 
	IplImage* graytemp = cvCreateImage(cvSize(img->width,img->height),8,1);
    IplImage* small_img=cvCreateImage(cvSize(cvRound(img->width/scale),cvRound(img->height/scale)),8,1); 
	foreground = cvCreateImage(cvSize(img->width,img->height),8,1); 

    cvCvtColor(img,gray, CV_BGR2GRAY);						//ת�Ҷ�ͼ
	mog(Mat(gray),Mat(foreground),0.005);					//����˶�ǰ��
	//erode(Mat(foreground), Mat(foreground), cv::Mat());	//��ʴ����
	//GaussianBlur(Mat(foreground),Mat(foreground),Size(3,3),0,0);//��˹�˲���ȥ��ǰ��������
	
	cvShowImage("originalimage",gray);
	Mat graymat = Mat(gray);
	graymat = Mat(gray).mul(Mat(foreground));
	gray = &IplImage(graymat);								//ǰ����ԭͼ��ˣ���ȡ�˶�����
	//cvAddWeighted(gray,1,foreground,0.5,0,gray);			//�˶�ǰ����ԭͼ���
	//cvThreshold(gray,gray,100,255,CV_THRESH_TOZERO);		//��ֵ��
    cvResize(gray, small_img, CV_INTER_LINEAR);				//����ͼƬ��С
	cvEqualizeHist(small_img,small_img);					//ֱ��ͼ����

	cvShowImage("foreground",foreground);
	cvShowImage("afterpreparation",gray);

	 //cvReleaseImage(&gray); 
	
	return small_img;
}
/************************
����: printSquaresandCircle
���ܣ����������λ��ԭʼ������֡��Ȧ������
���룺CvSeq* objects�� ����λ��ԭʼ����
	  IplImage* img�����ڻ�ͼ��֡
�������
************************/
void carsDetector::printSquaresandCircle(CvSeq* objects,IplImage* img)
{
	int distancecolor = 0;									//���ݲ�ͬ������ʾ��ͬ��ɫ�Ŀ�

    for(int i=0;i<(objects? objects->total:0);++i) 
    { 
        CvRect* r= (CvRect*)cvGetSeqElem(objects,i); 
		if (r->width>50&&r->height>75) distancecolor = 0; else distancecolor = 1;
        cvRectangle(img, cvPoint(r->x*scale,r->y*scale), cvPoint((r->x+r->width)*scale,(r->y+r->height)*scale), colors[distancecolor]); 
    } 
															//������
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
															//��Բ
    cvShowImage( "result", img ); 
}
