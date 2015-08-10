#include "cv.h" 
#include "highgui.h"
#include <opencv2/opencv.hpp> 
#include <iostream>
using namespace std;
using namespace cv;

struct Cars															//汽车结构体，用于存储每一帧中被检测到的车辆信息
{
	schar* carsObjects;												//存储汽车信息
	CvPoint carsPositions;											//以CvPoint形式保存的汽车位置
	int weight;														//权重，用于准确识别汽车
	bool isCar;														//检测是否是汽车
};

class carsDetector
{
private:
	void getCarsPositions(CvSeq*, CvPoint*);						//获取汽车位置方法，输入检测到位置的原始数据（CvSeq*），返回对应的位置（CvPoint*)
	double getDistance(CvPoint, CvPoint);							//获取两点距离
	//汽车识别主要过程
private:
	IplImage* imagePreparation(IplImage* img);						//读入帧的预处理，返回处理好的图片
	CvSeq* carsDetection( IplImage* image );                        //汽车追踪，返回追踪到当前帧中的汽车位置原始数据
	void printSquaresandCircle(CvSeq* objects,IplImage* img);		//输出框和圈
public:
	carsDetector(int filetype, char* filename, char* cascadename);	
	~carsDetector(){ cvClearMemStorage(storage);}
	void frameCapture();												//读取每一帧，并做简单分析
private:
	CvMemStorage* storage;											
	CvHaarClassifierCascade* cascade;								//分类器
	double scale;													//缩放比例，用于调整算法速度和精度
	char* file_name;												//读入视频文件路径
	int file_type;													//== 0 处理视频，== 1 处理摄像头
	//用于运动物体分离预处理
	BackgroundSubtractorMOG2 mog;									//前景分离器
	IplImage* foreground;											//检测到的前景

	CvScalar colors[];												//画圈和方框所使用的颜色

};
