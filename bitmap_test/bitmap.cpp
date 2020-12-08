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
	int size;		//比特位为1 的个数
}_Bit_Map;


//初始化
void Init_Bit_Map(_Bit_Map * bm, int totalbit)
{
	int capacity = 0;
	assert(bm);

	//一个int  可以表示32个元素， totalbit 个元素需要size / 32 + 1 个int 来存储
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

//bit 位置1
void Set_Bit_Map(_Bit_Map *bm, int which)
{
	int index = 0;
	int pos = 0;
	assert(bm);
	//所在的哪个整形空间(32bit 位)
	index = (which / 32);
	//所在该整形空间的哪个比特位
	pos = (which % 32);

	bm->_pBit[index] |= (1<<pos);

	cout << bm->_pBit[index] << endl;
}


//bit 位置0
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
int main(int argc, char *argv[])
{
	_Bit_Map bm;
	Init_Bit_Map(&bm, 10);
	Set_Bit_Map(&bm,4);
	Un_Set_Bit_Map(&bm,4);

	waitKey();
	cout << "I am ok" << endl;

	return 0;
}
