#include <iostream>
#include "Common.h"
#include "Connect.h"
#include "EncodeTool.h"
#include "MyClass.h"
using namespace std;

int main() {
	DBConnect dbConnect;  
	dbConnect.Connect(); 

	sigma::Object* obj = new sigma::Object(1,0);
	sigma::Position *pos_phys1 = new sigma::Position(1.1,2.2,3.3);
	sigma::Position *pos_phys2 = new sigma::Position(1.2,2.3,3.4);
	sigma::Position *pos_est1 = new sigma::Position(-1.1,-2.2,-3.3);
	sigma::Position *pos_est2 = new sigma::Position(-1.2,-2.3,-3.4);
	sigma::Attitude *atti_phys1 = new sigma::Attitude(1.1,2.2,3.3,4.4);
	sigma::Attitude *atti_phys2 = new sigma::Attitude(1.2,2.3,3.4,4.5);
	sigma::Attitude *atti_est1 = new sigma::Attitude(-1.1,-2.2,-3.3,-4.4);
	sigma::Attitude *atti_est2 = new sigma::Attitude(-1.2,-2.3,-3.4,-4.5);
	sigma::Time *time1 = new sigma::Time(1,2);
	string GeometryID = "dummy";
	string ResourceID = "dummy";
	std::vector<sigma::Geometry> Geometries;
	std::vector<sigma::Object> Objects;
	sigma::Vector4 *Origin = new sigma::Vector4(0.0f,0.0f,0.0f,0,0);
	sigma::Vector4 *Range = new sigma::Vector4(10.0f,10.0f,10.0f,2,2);

	//dbConnect.addObject(obj, GeometryID);
	//GeometryID = dbConnect.addGeometry(NULL,NULL,pos_phys2,atti_phys2,time);
	//GeometryID = dbConnect.addGeometry(pos_est1,atti_est1,pos_phys1,atti_phys1,time);
	//ResourceID = dbConnect.addResource(1,1,"dummy-Paraments","dummy-GeometryID","/load_path_A/load_path_A/abc.jpg","/save_path_A/save_path_B");
	//dbConnect.link_Geo_and_Obj(2,GeometryID);
	//dbConnect.link_Geo_and_Res(ResourceID,GeometryID);
	//Geometries = dbConnect.loadGeometry(Origin,Range,3);
	//cout << Geometries.size() << endl;
	//Objects = dbConnect.loadObject(Origin,Range,1);
	//cout << Objects.size() << endl;
	/*
	for(int i=0;i<Geometries.size();i++)
	{
		cout << Geometries[i].ID << " ";
		cout << Geometries[i].Posx_est << " ";
		cout << Geometries[i].Posy_est << " ";
		cout << Geometries[i].Posz_est << " ";
		cout << Geometries[i].Attix_est << " ";
		cout << Geometries[i].Attiy_est << " ";
		cout << Geometries[i].Attiz_est << " ";
		cout << Geometries[i].Attiw_est << " ";
		cout << Geometries[i].Posx_phys << " ";
		cout << Geometries[i].Posy_phys << " ";
		cout << Geometries[i].Posz_phys << " ";
		cout << Geometries[i].Attix_phys << " ";
		cout << Geometries[i].Attiy_phys << " ";
		cout << Geometries[i].Attiz_phys << " ";
		cout << Geometries[i].Attiw_phys << " ";
		cout << Geometries[i].Time_s << " ";
		cout << Geometries[i].Time_ms << endl;
		cout << endl;
	}
	*/
	

	dbConnect.Disconnect();
	return 0;
}
