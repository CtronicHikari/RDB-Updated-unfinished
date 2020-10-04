#pragma once
#ifndef MYCLASS_H
#define MYCLASS_H

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <pcl/common/common_headers.h>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
#include <pcl/console/parse.h>
#include <pcl/io/io.h>

using namespace std;
using namespace cv;

#define PICTURE 0
#define POINTCLOUD 1
#define TXT 2
#define FV_MIN 0.0000001

//////////////////////////////////////////////////////////////////////////
//********************4.0(Discard things before 4.0)********************//
//////////////////////////////////////////////////////////////////////////
namespace sigma{
//Object
class Object{
public:
	int ObjectID;
	int ParentNodeID;

	Object()
	{
		ObjectID = 0;
		ParentNodeID = 0;
	}
	
	Object(int _ObjectID, int _ParentNodeID)
	{
		ObjectID = _ObjectID;
		ParentNodeID = _ParentNodeID;
	}
};

//Geometry
class Geometry{
public:
	string ID;
	double Posx_est;
	double Posy_est;
	double Posz_est;
	double Attix_est;
	double Attiy_est;
	double Attiz_est;
	double Attiw_est;
	double Posx_phys;
	double Posy_phys;
	double Posz_phys;
	double Attix_phys;
	double Attiy_phys;
	double Attiz_phys;
	double Attiw_phys;
	unsigned int Time_s;
	unsigned int Time_ms;
	
	Geometry()
	{
		ID = "";
		Posx_est = 0.0f;
		Posy_est = 0.0f;
		Posz_est = 0.0f;
		Attix_est = 0.0f;
		Attiy_est = 0.0f;
		Attiz_est = 0.0f;
		Attiw_est = 0.0f;
		Posx_phys = 0.0f;
		Posy_phys = 0.0f;
		Posz_phys = 0.0f;
		Attix_phys = 0.0f;
		Attiy_phys = 0.0f;
		Attiz_phys = 0.0f;
		Attiw_phys = 0.0f;
		Time_s = 0;
		Time_ms = 0;
	}
};

class Transform {
public:
	double Posx;
	double Posy;
	double Posz;
	double Attix;
	double Attiy;
	double Attiz;
	double Attiw;
	unsigned int Time_s;
	unsigned int Time_ms;

	Transform() {
		Posx = 0.0f;
		Posy = 0.0f;
		Posz = 0.0f;
		Attix = 0.0f;
		Attiy = 0.0f;
		Attiz = 0.0f;
		Attiw = 0.0f;	
		Time_s = 0;
		Time_ms = 0;
	};

    	Transform(const double Posx,  const double Posy,  const double Posz,
                  const double Attix, const double Attiy, const double Attiz, const double Attiw,
	          const unsigned int Time_s, const unsigned int Time_ms) :
                  Posx(Posx), Posy(Posy), Posz(Posz),
                  Attix(Attix), Attiy(Attiy), Attiz(Attiz), Attiw(Attiw),
		  Time_s(Time_s), Time_ms(Time_ms){};

	~Transform() {};
};

class ResourceMeta {

public:
	string ID;
    	int ParentObjectID;
    	int Type;
   	string Ext;
   	sigma::Transform transform;
   	string Path;
    	string Parameters;

   	ResourceMeta() {
		ID = "";
		ParentObjectID = 0;
		Type = 0;
		Ext = "";
		transform = {};
		Path = "";
		Parameters = ""; 
	};

  	ResourceMeta(const string &ID, const int ParentObjectID, const int Type,
        	     const string Ext, const sigma::Transform &transform, const string Path, const string Parameters) :
       		     ID(ID), ParentObjectID(ParentObjectID), Type(Type),
       		     Ext(Ext), transform(transform), Path(Path), Parameters(Parameters){};

   	 ~ResourceMeta() {}

	//virtual void loadResourceFile(string full) {}
};

//Origin Radius Range
class Vector4
{
public:
	double x;
	double y;
	double z;
	unsigned int t_s;
	unsigned int t_ms;

	Vector4()
	{
		x = 0.0; y = 0.0; z = 0.0; t_s = 0; t_ms =0;
	}
	Vector4(double coordinateX,double coordinateY,double coordinateZ,unsigned int _s,unsigned int _ms)
	{
		x = coordinateX; y = coordinateY; z = coordinateZ; t_s = _s; t_ms = _ms;
	}
	void setVector4(double coordinateX,double coordinateY,double coordinateZ,unsigned int _s,unsigned int _ms)
	{
		x = coordinateX; y = coordinateY; z = coordinateZ; t_s = _s; t_ms = _ms;
	}
};

//Frustum
class Frustum
{
public:
	float dir_x, dir_y, dir_z;
	float nearDis, farDis;
	float theta, alpha;
	double t;

	Frustum()
	{
		dir_x = 0.0; dir_y = 0.0; dir_z = 0.0; nearDis = 0.0; farDis = 0.0; theta = 0.0; alpha = 0.0; t = 0.0;
	}
	Frustum(float dirX, float dirY, float dirZ, float near, float far, float angle1, float angle2, double time)
	{
		dir_x = dirX; dir_y = dirY; dir_z = dirZ; nearDis = near; farDis = far; theta = angle1; alpha = angle2; t = time;
	}
	void setFrustum(float dirX, float dirY, float dirZ, float near, float far, float angle1, float angle2,double time)
	{
		dir_x = dirX; dir_y = dirY; dir_z = dirZ; nearDis = near; farDis = far; theta = angle1; alpha = angle2; t = time;
	}
	
};

class Position
{
public:
	double Posx, Posy, Posz;
	Position()
	{
		Posx = 0.0f; Posy = 0.0f; Posz = 0.0f;
	}
	Position(double x,double y,double z)
	{
		Posx = x; Posy = y; Posz = z;
	}
	void setPosition(double x,double y,double z)
	{
		Posx = x; Posy = y; Posz = z;
	}
};

class Attitude
{
public:
	double Attitudex, Attitudey, Attitudez, Attitudew;
	Attitude()
	{
		Attitudex = 0.0f; Attitudey = 0.0f; Attitudez = 0.0f; Attitudew = 0.0f;
	}
	Attitude(double x, double y, double z, double w)
	{
		Attitudex = x; Attitudey = y; Attitudez = z; Attitudew = w;
	}
	void setAttitude(double x, double y, double z, double w)
	{
		Attitudex = x; Attitudey = y; Attitudez = z; Attitudew = w;
	}
};

class Time
{
public:
	unsigned int s;
	unsigned int ms;
	Time()
	{
		s = 0;  ms = 0;
	}
	Time(unsigned int _s, unsigned int _ms)
	{
		s = _s;  ms = _ms;
	}
	void setTime(unsigned int _s, unsigned int _ms)
	{
		s = _s;  ms = _ms;
	}
};

class Target
{
public:
	double targetX, targetY, targetZ;
	Target()
	{
		targetX = 0.0f; targetY = 0.0f; targetZ = 0.0f; 
	}	
	Target(double x, double y, double z)
	{
		targetX = x; targetY = y; targetZ = z; 
	}
	void setTarget(double x, double y, double z)
	{
		targetX = x; targetY = y; targetZ = z; 
	}		
};
}
//Load




//Expandable
//Image
/*class ImageResource:public ResourceMeta
{
public:
	Mat ImageData;
	void loadResourceFile(string full)
	{
		this->ImageData = imread(full);
	}


};*/

//PointCloud
/*class PointCloudResource:public ResourceMeta
{
public:
	pcl::PointCloud<pcl::PointXYZ> PointCloud;
	void loadResourceFile(string full)
	{
		pcl::io::loadPCDFile<pcl::PointXYZ>(full, this->PointCloud);
	}
};*/

//SAVE
/*class Data_new
{
public:
	virtual void saveResourceFile(string full){}
};*/

//Expandable
//Image
/*class ImageData_new:public Data_new
{
public:
	Mat ImageData;
	void saveResourceFile(string full)
	{
		cv::imwrite(full,this->ImageData);
	}
};*/

//PointCloud
/*class PointCloudData_new:public Data_new
{
public:
	pcl::PointCloud<pcl::PointXYZ> PointCloud;
	void saveResourceFile(string full)
	{
		pcl::io::savePCDFile(full, this->PointCloud);
	}
};*/

#endif
