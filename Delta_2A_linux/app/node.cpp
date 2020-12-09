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




//水平分辨率  像素/  米
#define HORIZONTAL_RESOLUTION  0.05
//垂直分辨率
#define VERTICAL_RESOLUTION  0.05





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
                std::vector<RslidarDataComplete> send_lidar_scan_data;
                send_lidar_scan_data.resize(lidar_scan_size);
                RslidarDataComplete one_lidar_data;
                for(size_t i = 0; i < lidar_scan_size; i++)
                {
                    one_lidar_data.signal = lidar_scan.signal[i];
                    one_lidar_data.angle = lidar_scan.angle[i];
                    one_lidar_data.distance = lidar_scan.distance[i];
                    send_lidar_scan_data[i] = one_lidar_data;
					printf("angle:%5.5f, distance:%5.5fm\r\n", one_lidar_data.angle, one_lidar_data.distance);
                }

                printf("Lidar count %d!\n", lidar_scan_size);


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
        //usleep(50);
    }

    return 0;
}
