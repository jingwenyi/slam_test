#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>



using namespace std;
using namespace cv;

typedef struct Bit_Map
{
	int *_pBit;
	int _capacity;
	int size;		//����λΪ1 �ĸ���
}_Bit_Map;


//��ʼ��
void Init_Bit_Map(_Bit_Map * bm, int totalbit)
{
	int capacity = 0;
	assert(bm);

	//һ��int  ���Ա�ʾ32��Ԫ�أ� totalbit ��Ԫ����Ҫsize / 32 + 1 ��int ���洢
	capacity = (totalbit / 32) + 1;
	bm->_pBit = (int*)malloc(capacity);
	if(NULL == bm->_pBit)
	{
		assert(0);
		return;
	}

	memset(bm->_pBit, 0, sizeof(int)*capacity);
	bm->_capacity = capacity;
	bm->size = 0;
}

//bit λ��1
void Set_Bit_Map(_Bit_Map *bm, int which)
{
	int index = 0;
	int pos = 0;
	assert(bm);
	//���ڵ��ĸ����οռ�(32bit λ)
	index = (which / 32);
	//���ڸ����οռ���ĸ�����λ
	pos = (which % 32);

	bm->_pBit[index] |= (1<<pos);

	cout << bm->_pBit[index] << endl;
}


//bit λ��0
void Un_Set_Bit_Map(_Bit_Map* bm, int which)
{
	int index = 0;
	int pos = 0;
	assert(bm);
	index = (which / 32);
	pos = (which % 32);
	bm->_pBit[index] &= ~(1<<pos);
	cout << bm->_pBit[index] << endl;
	
}
int main_test1(int argc, char *argv[])
{
	_Bit_Map bm;
	Init_Bit_Map(&bm, 10);
	Set_Bit_Map(&bm,4);
	Un_Set_Bit_Map(&bm,4);

	waitKey();
	cout << "I am ok" << endl;

	return 0;
}



int main(int argc, char *argv[])
{
	Mat map;
	map.create(1000, 1000, CV_8UC3);
	map.setTo(255);
	imwrite("map.bmp", map);

	map.release();

	//����bmp �ļ����ѷɻ���λ�ñ�ע�����ĵ�
	map = imread("map.bmp");

	for(int i=0; i<map.rows; i++)
	{
		if(i > (map.rows / 2  - 10)  && i < (map.rows / 2 + 10))
		{
			for(int j=0; j<map.cols; j++)
			{
				if(j > (map.cols / 2 -10) && j < (map.cols / 2 + 10))
				{
					Scalar color = map.at<Vec3b>(i, j);
					color(0) = 255;
					color(1) = 0;
					color(2) = 0;
					map.at<Vec3b>(i,j) = Vec3b(color(0), color(1), color(2));
				}
			}
		}
	}


	imwrite("map.bmp", map);

	waitKey();
	cout << "I am ok" << endl;

	return 0;
}

