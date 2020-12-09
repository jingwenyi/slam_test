/*
*  3iRoboticsLIDAR System II
*  Driver Interface
*
*  Copyright 2017 3iRobotics
*  All rights reserved.
*
*	Author: 3iRobotics, Data:2017-09-15
*
*/


#include "C3iroboticsLidar.h"
#include "CSerialConnection.h"

#define DEG2RAD(x) ((x)*M_PI/180.)



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

#include <pthread.h>




//水平分辨率  像素/  米
#define HORIZONTAL_RESOLUTION  0.005
//垂直分辨率
#define VERTICAL_RESOLUTION  0.005





typedef struct _rslidar_data
{
    _rslidar_data()
    {
        signal = 0;
        angle = 0.0;
        distance = 0.0;
    }
    uint8_t signal;
    float   angle;
    float   distance;
}RslidarDataComplete;

using namespace cv;
using namespace std;
using namespace everest::hwdrivers;


int flag;
std::vector<RslidarDataComplete> send_lidar_scan_data;

/*

				0
				|
				|
				|
	270-----------------90
				|
				|
				|
			     180

*/

void *thread(void *ptr)
{
	Mat map;
	map.create(2000, 2000, CV_8UC3);
	map.setTo(255);

	for(int i=0; i<map.rows; i++)
	{
		if(i > (map.rows / 2  - 10)  && i < (map.rows / 2 + 10))
		{
			for(int j=0; j<map.cols; j++)
			{
				if(j > (map.cols / 2 -10) && j < (map.cols / 2 + 10))
				{
					map.at<Vec3b>(i,j) = Vec3b(255, 0, 0);
				}
			}
		}
	}

	int origal_row = map.rows / 2;
	int origal_col = map.cols / 2;

	while(1)
	{
		if(flag == 1)
		{
			int size = send_lidar_scan_data.size();

			for(int i=0; i<size; i++)
			{
				//根据角度和距离求每个点的x , y
				float x_col, y_row;
				x_col = send_lidar_scan_data[i].distance * sin(send_lidar_scan_data[i].angle * M_PI / 180) / HORIZONTAL_RESOLUTION;
				y_row = send_lidar_scan_data[i].distance * cos(send_lidar_scan_data[i].angle * M_PI / 180) / HORIZONTAL_RESOLUTION;

				int x,y;
				x = abs(x_col);
				y = abs(y_row);

				//printf("x=%d, y=%d\r\n", x, y);

				if(send_lidar_scan_data[i].angle > 0 && send_lidar_scan_data[i].angle <= 90)
				{
					map.at<Vec3b>(origal_row - y, origal_col + x) = Vec3b(0, 0, 0);
				}
				else if(send_lidar_scan_data[i].angle > 90 && send_lidar_scan_data[i].angle <= 180)
				{
					map.at<Vec3b>(origal_row + y, origal_col + x) = Vec3b(0, 0, 0);
				}
				else if(send_lidar_scan_data[i].angle > 180 && send_lidar_scan_data[i].angle <= 270)
				{
					map.at<Vec3b>(origal_row + y, origal_col - x) = Vec3b(0, 0, 0);
				}
				else
				{
					map.at<Vec3b>(origal_row - y, origal_col - x) = Vec3b(0, 0, 0);
				}
			}

			imwrite("map.bmp", map);

		
			flag = 2;
		}
		usleep(50);
	}


	return 0;
}

int main(int argc, char * argv[])
{
	//串口波特率和设备端口
	int    opt_com_baudrate = 230400;
    string opt_com_path = "/dev/ttyUSB0";

    CSerialConnection serial_connect;
    C3iroboticsLidar robotics_lidar;

	//串口设置
    serial_connect.setBaud(opt_com_baudrate);
    serial_connect.setPort(opt_com_path.c_str());

	//打开串口是否成功
    if(serial_connect.openSimple())
    {
        printf("[AuxCtrl] Open serail port sucessful!\n");
        printf("baud rate:%d\n",serial_connect.getBaud());
    }
    else
    {
        printf("[AuxCtrl] Open serail port %s failed! \n", opt_com_path.c_str());
		//打开串口失败直接退出
        return -1;
    }

    printf("C3iroboticslidar connected\n");

    robotics_lidar.initilize(&serial_connect);


	flag = 0;

	pthread_t id;
	int ret = pthread_create(&id, NULL, thread, NULL);

	if(ret)
	{
		printf("create pthread error!\r\n");
		return -1;
	}


    while (1)
    {
    	//获取雷达扫描的数据
		TLidarGrabResult result = robotics_lidar.getScanData();
        switch(result)
        {
            case LIDAR_GRAB_ING:
            {
                break;
            }
			//扫描数据成功
            case LIDAR_GRAB_SUCESS:
            {
                TLidarScan lidar_scan = robotics_lidar.getLidarScan();
                size_t lidar_scan_size = lidar_scan.getSize();
				if(flag == 0)
				{
                	send_lidar_scan_data.resize(lidar_scan_size);
				}
                RslidarDataComplete one_lidar_data;
                for(size_t i = 0; i < lidar_scan_size; i++)
                {
                    one_lidar_data.signal = lidar_scan.signal[i];
                    one_lidar_data.angle = lidar_scan.angle[i];
                    one_lidar_data.distance = lidar_scan.distance[i];
					if(flag == 0)
					{
                    	send_lidar_scan_data[i] = one_lidar_data;
					}
                }


				if(flag == 0)
					flag = 1;

                //printf("Lidar count %d!\n", lidar_scan_size);


                break;
            }
            case LIDAR_GRAB_ERRO:
            {
                break;
            }
            case LIDAR_GRAB_ELSE:
            {
                printf("[Main] LIDAR_GRAB_ELSE!\n");
                break;
            }
        }
        usleep(50);
    }

    return 0;
}
