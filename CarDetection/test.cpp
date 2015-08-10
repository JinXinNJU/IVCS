
#include "CarsDetection.h"
int main()
{
	carsDetector detector(0,"test.avi","CarsModel.xml");
	detector.frameCapture();
	//system("pause");
}


