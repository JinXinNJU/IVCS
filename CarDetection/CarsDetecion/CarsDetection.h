#include "cv.h" 
#include "highgui.h"
#include <opencv2/opencv.hpp> 
#include <iostream>
using namespace std;
using namespace cv;

struct Cars															//�����ṹ�壬���ڴ洢ÿһ֡�б���⵽�ĳ�����Ϣ
{
	schar* carsObjects;												//�洢������Ϣ
	CvPoint carsPositions;											//��CvPoint��ʽ���������λ��
	int weight;														//Ȩ�أ�����׼ȷʶ������
	bool isCar;														//����Ƿ�������
};

class carsDetector
{
private:
	void getCarsPositions(CvSeq*, CvPoint*);						//��ȡ����λ�÷����������⵽λ�õ�ԭʼ���ݣ�CvSeq*�������ض�Ӧ��λ�ã�CvPoint*)
	double getDistance(CvPoint, CvPoint);							//��ȡ�������
	//����ʶ����Ҫ����
private:
	IplImage* imagePreparation(IplImage* img);						//����֡��Ԥ�������ش���õ�ͼƬ
	CvSeq* carsDetection( IplImage* image );                        //����׷�٣�����׷�ٵ���ǰ֡�е�����λ��ԭʼ����
	void printSquaresandCircle(CvSeq* objects,IplImage* img);		//������Ȧ
public:
	carsDetector(int filetype, char* filename, char* cascadename);	
	~carsDetector(){ cvClearMemStorage(storage);}
	void frameCapture();												//��ȡÿһ֡�������򵥷���
private:
	CvMemStorage* storage;											
	CvHaarClassifierCascade* cascade;								//������
	double scale;													//���ű��������ڵ����㷨�ٶȺ;���
	char* file_name;												//������Ƶ�ļ�·��
	int file_type;													//== 0 ������Ƶ��== 1 ��������ͷ
	//�����˶��������Ԥ����
	BackgroundSubtractorMOG2 mog;									//ǰ��������
	IplImage* foreground;											//��⵽��ǰ��

	CvScalar colors[];												//��Ȧ�ͷ�����ʹ�õ���ɫ

};
