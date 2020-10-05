#include "Connect.h"
#include<math.h>
//******************************
//Connect & Disconnect
//******************************
const void DBConnect::Connect()
{
	pcon = mysql_init(NULL);
	if(!mysql_real_connect(pcon, server, user, password, db, port, NULL, 0))
	{
		cout << "connection failed!" << endl;
	}
	else
		cout << "connection success!" << endl;
}

const void DBConnect::Disconnect()
{
	mysql_free_result(result);
	mysql_close(pcon);
	cout << "Disconnect from the database!" << endl;
}

//********************************
//Add Object & Geometry & Connect
//********************************
const void DBConnect::addObject(sigma::Object *Object, string GeometryID)
{
	char sql[200];
	int sql_len = sprintf(sql, "insert into server.object(OID,POID) values('%d','%d')", Object->ObjectID, Object->ParentNodeID);
	res = mysql_real_query(pcon, sql, sql_len);

	//TODO
}

const string DBConnect::addGeometry(sigma::Position *pos_est, sigma::Attitude *atti_est, sigma::Position *pos_phys, sigma::Attitude *atti_phys, sigma::Time *time)
{
	string ID = "";
	char sql[5000];
	int sql_len;
	if(pos_est == NULL || atti_est == NULL)
	{
		ID += to_string(pos_phys->Posx) + "_" + to_string(pos_phys->Posy) + "_" + to_string(pos_phys->Posz) +"_";
		ID += to_string(atti_phys->Attitudex) + "_" + to_string(atti_phys->Attitudey) + "_" + to_string(atti_phys->Attitudez) + "_" + to_string(atti_phys->Attitudew) + "_";
		ID += to_string(time->s) + "." + to_string(time->ms);
		ID = encodeTool.getMD5(ID);
		sql_len = sprintf(sql, "insert into server.geometry(ID,Posx_phys,Posy_phys,Posz_phys,Attix_phys,Attiy_phys,Attiz_phys,Attiw_phys,Time_s,Time_ms) values('%s','%.20f','%.20f','%.20f','%.20f','%.20f','%.20f','%.20f','%u','%u')", ID.c_str(), pos_phys->Posx, pos_phys->Posy, pos_phys->Posz, atti_phys->Attitudex,atti_phys->Attitudey,atti_phys->Attitudez,atti_phys->Attitudew,time->s,time->ms);
		res = mysql_real_query(pcon,sql,sql_len);
	}
	else
	{
		ID += to_string(pos_est->Posx) + "_" + to_string(pos_est->Posy) + "_" + to_string(pos_est->Posz) +"_";
		ID += to_string(atti_est->Attitudex) + "_" + to_string(atti_est->Attitudey) + "_" + to_string(atti_est->Attitudez) + "_" + to_string(atti_est->Attitudew) + "_";
		ID += to_string(time->s) + "." + to_string(time->ms);
		ID = encodeTool.getMD5(ID);
		sql_len = sprintf(sql, "insert into server.geometry(ID,Posx_est,Posy_est,Posz_est,Attix_est,Attiy_est,Attiz_est,Attiw_est,Posx_phys,Posy_phys,Posz_phys,Attix_phys,Attiy_phys,Attiz_phys,Attiw_phys,Time_s,Time_ms) values('%s','%.20f','%.20f','%.20f','%.20f','%.20f','%.20f','%.20f','%.20f','%.20f','%.20f','%.20f','%.20f','%.20f','%.20f','%u','%u')", ID.c_str(), pos_est->Posx, pos_est->Posy, pos_est->Posz, atti_est->Attitudex, atti_est->Attitudey, atti_est->Attitudez, atti_est->Attitudew, pos_phys->Posx, pos_phys->Posy, pos_phys->Posz, atti_phys->Attitudex,atti_phys->Attitudey,atti_phys->Attitudez,atti_phys->Attitudew,time->s,time->ms);
		res = mysql_real_query(pcon,sql,sql_len);
	}	
	return ID;
}

const string DBConnect::addResource(int ParentObjectID, int Type, string Parameters, string GeometryID, string load_path, string save_path) // /home/guan/new_image/MOV_0168_21.png
{
	int sql_len;
	char sql[500];
	string ID = "";

	std::vector<string> splittedName = encodeTool.ParametersSplitter(load_path,'/');
	std::vector<string> splittedID = encodeTool.ParametersSplitter(splittedName[splittedName.size()-1],'.');
	ID = encodeTool.getMD5(splittedID[0] + "_" + GeometryID) + "." + splittedID[1];
	
	sql_len = sprintf(sql, "insert into server.resources(ID,POID,Type,Ext,Path,Para) values('%s','%d','%d','%s','%s','%s')",ID.c_str(),ParentObjectID,Type,splittedID[1].c_str(),save_path.c_str(),Parameters.c_str());
	res = mysql_real_query(pcon, sql, sql_len);

	return ID;
}

const void DBConnect::link_Geo_and_Obj(int ObjectID, string GeometryID)
{
	char sql[100];
	int sql_len = sprintf(sql,"insert into server.link_geo_and_obj(OID,GID) values('%d','%s')", ObjectID, GeometryID.c_str());
	res = mysql_real_query(pcon, sql, sql_len);
}

const void DBConnect::link_Geo_and_Res(string ResourceID, string GeometryID)
{
	char sql[100];
	int sql_len = sprintf(sql,"insert into server.link_geo_and_res(RID,GID) values('%s','%s')", ResourceID.c_str(), GeometryID.c_str());
	res = mysql_real_query(pcon, sql, sql_len);
}

const std::vector<sigma::Geometry> DBConnect::loadGeometry(sigma::Vector4 *Origin, sigma::Vector4 *Range, int Mode)
{
	std::vector<sigma::Geometry> Geometries;
	sigma::Geometry src;
	string temp;
	int sql_len;
	char sql[1000];

	if(Mode == 1)  //Sphere: If no _est use _phys
	{
		if(Range->x > 0 && Range->y > 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posy_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),(POW(Posx_phys-'%f',2)/POW('%f',2))+(POW(Posy_phys-'%f',2)/POW('%f',2))+(POW(Posz_phys-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		
		else if(Range->x > 0 && Range->y > 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posy_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1,(POW(Posx_phys-'%f',2)/POW('%f',2))+(POW(Posy_phys-'%f',2)/POW('%f',2))+(POW(Posz_phys-'%f',2)/POW('%f',2))<1);", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2);
		}
		else if(Range->x > 0 && Range->y > 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posy_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),(POW(Posx_phys-'%f',2)/POW('%f',2))+(POW(Posy_phys-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x > 0 && Range->y > 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posy_est-'%f',2)/POW('%f',2))<1,(POW(Posx_phys-'%f',2)/POW('%f',2))+(POW(Posy_phys-'%f',2)/POW('%f',2))<1);", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->x, Range->x/2, Origin->y, Range->y/2);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),(POW(Posx_phys-'%f',2)/POW('%f',2))+(POW(Posz_phys-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->x, Range->x/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->x, Range->x/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1,(POW(Posx_phys-'%f',2)/POW('%f',2))+(POW(Posz_phys-'%f',2)/POW('%f',2))<1);", Origin->x, Range->x/2, Origin->z, Range->z/2, Origin->x, Range->x/2, Origin->z, Range->z/2);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posy_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),(POW(Posy_phys-'%f',2)/POW('%f',2))+(POW(Posz_phys-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posy_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1,(POW(Posy_phys-'%f',2)/POW('%f',2))+(POW(Posz_phys-'%f',2)/POW('%f',2))<1);", Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->y, Range->y/2, Origin->z, Range->z/2);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posx_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),(POW(Posx_phys-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->x, Range->x/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->x, Range->x/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posx_est-'%f',2)/POW('%f',2))<1,(POW(Posx_phys-'%f',2)/POW('%f',2))<1);", Origin->x, Range->x/2, Origin->x, Range->x/2);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posy_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),(POW(Posy_phys-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->y, Range->y/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->y, Range->y/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posy_est-'%f',2)/POW('%f',2))<1,(POW(Posy_phys-'%f',2)/POW('%f',2))<1);", Origin->y, Range->y/2,Origin->y, Range->y/2);
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posz_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),(POW(Posz_phys-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posz_est-'%f',2)/POW('%f',2))<1,(POW(Posz_phys-'%f',2)/POW('%f',2))<1);", Origin->z, Range->z/2, Origin->z, Range->z/2);
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else
		{
			sql_len = sprintf(sql, "select * from server.geometry;");
		}
	}
	else if(Mode == 2) //Cube: If no _est use _phys
	{
		if(Range->x > 0 && Range->y > 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posx_est-'%f')<='%f' AND ABS(Posy_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),ABS(Posx_phys-'%f')<='%f' AND ABS(Posy_phys-'%f')<='%f' AND ABS(Posz_phys-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		
		else if(Range->x > 0 && Range->y > 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posx_est-'%f')<='%f' AND ABS(Posy_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f',ABS(Posx_phys-'%f')<='%f' AND ABS(Posy_phys-'%f')<='%f' AND ABS(Posz_phys-'%f')<='%f');", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2);
		}
		else if(Range->x > 0 && Range->y > 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posx_est-'%f')<='%f' AND ABS(Posy_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),ABS(Posx_phys-'%f')<='%f' AND ABS(Posy_phys-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x > 0 && Range->y > 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posx_est-'%f')<='%f' AND ABS(Posy_est-'%f')<='%f',ABS(Posx_phys-'%f')<='%f' AND ABS(Posy_phys-'%f')<='%f');", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->x, Range->x/2, Origin->y, Range->y/2);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posx_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),ABS(Posx_phys-'%f')<='%f' AND ABS(Posz_phys-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->x, Range->x/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->x, Range->x/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posx_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f',ABS(Posx_phys-'%f')<='%f' AND ABS(Posz_phys-'%f')<='%f');", Origin->x, Range->x/2, Origin->z, Range->z/2, Origin->x, Range->x/2, Origin->z, Range->z/2);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posy_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),ABS(Posy_phys-'%f')<='%f' AND ABS(Posz_phys-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posy_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f',ABS(Posy_phys-'%f')<='%f' AND ABS(Posz_phys-'%f')<='%f');", Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->y, Range->y/2, Origin->z, Range->z/2);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posx_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),ABS(Posx_phys-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->x, Range->x/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->x, Range->x/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posx_est-'%f')<='%f',ABS(Posx_phys-'%f')<='%f');", Origin->x, Range->x/2, Origin->x, Range->x/2);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posy_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),ABS(Posy_phys-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->y, Range->y/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->y, Range->y/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posy_est-'%f')<='%f',ABS(Posy_phys-'%f')<='%f');", Origin->y, Range->y/2, Origin->y, Range->y/2);
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posz_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),ABS(Posz_phys-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posz_est-'%f')<='%f',ABS(Posz_phys-'%f')<='%f');", Origin->z, Range->z/2, Origin->z, Range->z/2);
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'));", Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else
		{
			sql_len = sprintf(sql, "select * from server.geometry;");
		}
	}
	else if(Mode == 3)  //Sphere: Only use _est
	{
		if(Range->x > 0 && Range->y > 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posy_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		
		else if(Range->x > 0 && Range->y > 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posy_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1;", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2);
		}
		else if(Range->x > 0 && Range->y > 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posy_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x > 0 && Range->y > 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posy_est-'%f',2)/POW('%f',2))<1;", Origin->x, Range->x/2, Origin->y, Range->y/2);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->x, Range->x/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1;", Origin->x, Range->x/2, Origin->z, Range->z/2);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posy_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posy_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1;", Origin->y, Range->y/2, Origin->z, Range->z/2);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posx_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->x, Range->x/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posx_est-'%f',2)/POW('%f',2))<1;", Origin->x, Range->x/2);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posy_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->y, Range->y/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posy_est-'%f',2)/POW('%f',2))<1;", Origin->y, Range->y/2);
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posz_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where (POW(Posz_est-'%f',2)/POW('%f',2))<1;", Origin->z, Range->z/2);
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else
		{
			sql_len = sprintf(sql, "select * from server.geometry;");
		}
	}	
	else if(Mode == 4) //Cube: Only use _est
	{
		if(Range->x > 0 && Range->y > 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posx_est-'%f')<='%f' AND ABS(Posy_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		
		else if(Range->x > 0 && Range->y > 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posx_est-'%f')<='%f' AND ABS(Posy_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f';", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2);
		}
		else if(Range->x > 0 && Range->y > 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posx_est-'%f')<='%f' AND ABS(Posy_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x > 0 && Range->y > 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posx_est-'%f')<='%f' AND ABS(Posy_est-'%f')<='%f');", Origin->x, Range->x/2, Origin->y, Range->y/2);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posx_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->x, Range->x/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posx_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f';", Origin->x, Range->x/2, Origin->z, Range->z/2);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posy_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posy_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f';", Origin->y, Range->y/2, Origin->z, Range->z/2);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posx_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->x, Range->x/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posx_est-'%f')<='%f';", Origin->x, Range->x/2);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posy_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->y, Range->y/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posy_est-'%f')<='%f';", Origin->y, Range->y/2);
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posz_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z > 0 && Range->t_s <= 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where ABS(Posz_est-'%f')<='%f';", Origin->z, Range->z/2);
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u');", Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		else
		{
			sql_len = sprintf(sql, "select * from server.geometry;");
		}
	}
	else
	{
		cout << "Mode Error" << endl;
		Geometries.clear();
		return Geometries;
	}
	
	res = mysql_real_query(pcon, sql, sql_len);

	result = mysql_store_result(pcon);

	while(sql_row = mysql_fetch_row(result))
	{
		src.ID = sql_row[0];
		src.Posx_est = sql_row[1]==NULL ? 0.0f : strtod(sql_row[1],NULL);
		src.Posy_est = sql_row[2]==NULL ? 0.0f : strtod(sql_row[2],NULL);
		src.Posz_est = sql_row[3]==NULL ? 0.0f : strtod(sql_row[3],NULL);
		src.Attix_est = sql_row[4]==NULL ? 0.0f : strtod(sql_row[4],NULL);
		src.Attiy_est = sql_row[5]==NULL ? 0.0f : strtod(sql_row[5],NULL);
		src.Attiz_est = sql_row[6]==NULL ? 0.0f : strtod(sql_row[6],NULL);
		src.Attiw_est = sql_row[7]==NULL ? 0.0f : strtod(sql_row[7],NULL);
		src.Posx_phys = strtod(sql_row[8],NULL);
		src.Posy_phys = strtod(sql_row[9],NULL);
		src.Posz_phys = strtod(sql_row[10],NULL);
		src.Attix_phys = strtod(sql_row[11],NULL);
		src.Attiy_phys = strtod(sql_row[12],NULL);
		src.Attiz_phys = strtod(sql_row[13],NULL);
		src.Attiw_phys = strtod(sql_row[14],NULL);
		src.Time_s = encodeTool.string_to_unsigned(sql_row[15]);
		src.Time_ms = encodeTool.string_to_unsigned(sql_row[16]);
		Geometries.push_back(src);
	}
	return Geometries;
}

const std::vector<sigma::Object> DBConnect::loadObject(sigma::Vector4 *Origin, sigma::Vector4 *Range, int Mode)
{
	std::vector<sigma::Object> Objects;
	std::vector<sigma::Geometry> Geometries;
	std::vector<int> OIDs;
	sigma::Object src;
	string temp;
	int sql_len;
	char sql[300];
	int i;

	Geometries = loadGeometry(Origin,Range,Mode);
	for(i=0;i<Geometries.size();i++)
	{
		sql_len = sprintf(sql, "select OID from server.link_geo_and_obj where GID='%s';", Geometries[i].ID.c_str());
		res = mysql_real_query(pcon, sql, sql_len);
		result = mysql_store_result(pcon);
		while(sql_row = mysql_fetch_row(result))
		{
			temp = sql_row[0];
			OIDs.push_back(atoi(temp.c_str()));
		}
	}
	for(i=0;i<OIDs.size();i++)
	{
		sql_len = sprintf(sql, "select * from server.object where OID='%d';", OIDs[i]);
		res = mysql_real_query(pcon, sql, sql_len);
		result = mysql_store_result(pcon);
		while(sql_row = mysql_fetch_row(result))
		{
			temp = sql_row[0];
			src.ObjectID = atoi(temp.c_str());
			temp = sql_row[1];
			src.ParentNodeID = atoi(temp.c_str());
			Objects.push_back(src);
		}
	}
	return Objects;
}

sigma::ResourceMeta DBConnect::readResourceInformation(MYSQL_ROW src)
{
	sigma::ResourceMeta dst;
	int sql_len,i;
	char sql[300];
	string temp;
	string temp_GID;

	dst.ID = src[0];
	temp = src[1];  	dst.ParentObjectID = atoi(temp.c_str());
	temp = src[2];  	dst.Type = atoi(temp.c_str());
	dst.Ext = src[3];
	dst.Path = src[4];
	dst.Parameters = src[5];

	sql_len = sprintf(sql, "select GID from server.link_geo_and_res where RID='%s';", dst.ID.c_str());
	res = mysql_real_query(pcon, sql, sql_len);
	result = mysql_store_result(pcon);
	while(sql_row = mysql_fetch_row(result))
	{
		temp = sql_row[0];
		temp_GID = temp;
	}
	sql_len = sprintf(sql, "select * from server.geometry where ID='%s';", temp_GID.c_str());
	res = mysql_real_query(pcon, sql, sql_len);
	result = mysql_store_result(pcon);
	while(sql_row = mysql_fetch_row(result))
	{
		dst.transform.Posx = sql_row[1]==NULL ? strtod(sql_row[8],NULL) : strtod(sql_row[1],NULL);
		dst.transform.Posy = sql_row[2]==NULL ? strtod(sql_row[9],NULL) : strtod(sql_row[2],NULL);
		dst.transform.Posz = sql_row[3]==NULL ? strtod(sql_row[10],NULL) : strtod(sql_row[3],NULL);
		dst.transform.Attix = sql_row[4]==NULL ? strtod(sql_row[11],NULL) : strtod(sql_row[4],NULL);
		dst.transform.Attiy = sql_row[5]==NULL ? strtod(sql_row[12],NULL) : strtod(sql_row[5],NULL);
		dst.transform.Attiz = sql_row[6]==NULL ? strtod(sql_row[13],NULL) : strtod(sql_row[6],NULL);
		dst.transform.Attiw = sql_row[7]==NULL ? strtod(sql_row[14],NULL) : strtod(sql_row[7],NULL);
		dst.transform.Time_s = encodeTool.string_to_unsigned(sql_row[15]);
		dst.transform.Time_ms = encodeTool.string_to_unsigned(sql_row[16]);
	}

	return dst;
}

std::vector<sigma::ResourceMeta> DBConnect::loadResourceMeta(sigma::Vector4 *Origin, sigma::Vector4 *Range, int Mode, int Type)
{
	std::vector<sigma::ResourceMeta> ResourceMetas;
	std::vector<sigma::Geometry> Geometries;
	std::vector<string> RIDs;
	sigma::ResourceMeta dst;
	string temp;
	char sql[300];
	int sql_len,i;

	Geometries = loadGeometry(Origin,Range,Mode);
	
	for(i=0;i<Geometries.size();i++)
	{
		sql_len = sprintf(sql, "select RID from server.link_geo_and_res where GID='%s';", Geometries[i].ID.c_str());
		res = mysql_real_query(pcon, sql, sql_len);
		result = mysql_store_result(pcon);
		while(sql_row = mysql_fetch_row(result))
		{
			temp = sql_row[0];
			RIDs.push_back(temp);
		}
	}
	
	for(i=0;i<RIDs.size();i++)
	{
		if(Type != 0)
			sql_len = sprintf(sql, "select * from server.Resources where ID='%s' and Type='%d';", RIDs[i].c_str(), Type);
		else
			sql_len = sprintf(sql, "select * from server.Resources where ID='%s';", RIDs[i].c_str());
		res = mysql_real_query(pcon, sql, sql_len);
		result = mysql_store_result(pcon);
		while(sql_row = mysql_fetch_row(result))
		{
			dst = readResourceInformation(sql_row);
			ResourceMetas.push_back(dst);
		}
	}
	
	return ResourceMetas;
}

std::vector<sigma::ResourceMeta> DBConnect::loadResourceMeta(int ObjectID, int Type)
{
	std::vector<sigma::ResourceMeta> ResourceMetas;
	sigma::ResourceMeta dst;
	string temp;
	char sql[100];
	int sql_len;

	if(Type != 0)
		sql_len = sprintf(sql, "select * from server.Resources where POID='%d' and Type='%d';", ObjectID, Type);
	else
		sql_len = sprintf(sql, "select * from server.Resources where POID='%d';", ObjectID);
	res = mysql_real_query(pcon, sql, sql_len);
	result = mysql_store_result(pcon);
	while(sql_row = mysql_fetch_row(result))
	{
		dst = readResourceInformation(sql_row);
		ResourceMetas.push_back(dst);
	}
	return ResourceMetas;
}

std::vector<sigma::ResourceMeta>* DBConnect::loadResourceMeta(std::vector<sigma::Object> Objects, int Type)
{
	std::vector<sigma::ResourceMeta> ResourceMetas;
	std::vector<sigma::ResourceMeta>* collection = new std::vector<sigma::ResourceMeta>[Objects.size()];
	int i;

	for(i=0;i<Objects.size();i++)
	{
		ResourceMetas = loadResourceMeta(Objects[i].ObjectID, Type);
		collection[i] = ResourceMetas;
	}

	return collection;
}

std::vector<sigma::ResourceMeta> DBConnect::loadResourceMeta(string GeometryID, int Type)
{
	std::vector<sigma::ResourceMeta> ResourceMetas;
	std::vector<string> RIDs;
	sigma::ResourceMeta dst;
	string temp;
	char sql[300];
	int sql_len,i;


	sql_len = sprintf(sql, "select RID from server.link_geo_and_res where GID='%s';", GeometryID.c_str());
	res = mysql_real_query(pcon, sql, sql_len);
	result = mysql_store_result(pcon);
	while(sql_row = mysql_fetch_row(result))
	{
		temp = sql_row[0];
		RIDs.push_back(temp);
	}
	
	for(i=0;i<RIDs.size();i++)
	{
		if(Type != 0)
			sql_len = sprintf(sql, "select * from server.Resources where ID='%s' and Type='%d';", RIDs[i].c_str(), Type);
		else
			sql_len = sprintf(sql, "select * from server.Resources where ID='%s';", RIDs[i].c_str());
		res = mysql_real_query(pcon, sql, sql_len);
		result = mysql_store_result(pcon);
		while(sql_row = mysql_fetch_row(result))
		{
			dst = readResourceInformation(sql_row);
			ResourceMetas.push_back(dst);
		}
	}
	return ResourceMetas;
}

std::vector<sigma::ResourceMeta> DBConnect::loadResourceMeta(sigma::Vector4 *Origin, sigma::Vector4 *Range, sigma::Target *target, double theta, int Type)
{
	
	std::vector<sigma::ResourceMeta> ResourceMetas;
	std::vector<sigma::ResourceMeta> temp_ResourceMetas;
	std::vector<sigma::Geometry> Geometries;
	EncodeTool encodeTool;
	int i,j;

	double dir_x,dir_y,dir_z;
	double temp_x,temp_t,temp_z;
	double cos_temp_theta;
	double roll,pitch,yaw;
	
	Geometries = loadGeometry(Origin,Range,4);

	for(i = 0;i < Geometries.size();i++)
	{
		double tmp = (Geometries[i].Posx_est==0) ? Geometries[i].Posx_phys : Geometries[i].Posx_est;
		dir_x = target->targetX - tmp;
		tmp = (Geometries[i].Posy_est==0) ? Geometries[i].Posy_phys : Geometries[i].Posy_est;
		dir_y = target->targetY - tmp;
		tmp = (Geometries[i].Posz_est==0) ? Geometries[i].Posz_phys : Geometries[i].Posz_est;
		dir_z = target->targetZ - tmp;
		temp_ResourceMetas = loadResourceMeta(Geometries[i].ID, Type);
		
		for(j=0;j<temp_ResourceMetas.size();j++)
		{
			double Rx = temp_ResourceMetas[j].transform.Attix;
			double Ry = temp_ResourceMetas[j].transform.Attiy;
			double Rz = temp_ResourceMetas[j].transform.Attiz;
			double Rw = temp_ResourceMetas[j].transform.Attiw;
	
			encodeTool.R = encodeTool.Quaternion2RotationMatrix(Rx,Ry,Rz,Rw);
			encodeTool.init_vector << 0,0,1;
			encodeTool.temp_vector = encodeTool.R * encodeTool.init_vector;
			encodeTool.temp_dir_vector << dir_x,dir_y,dir_z;
			cos_temp_theta = encodeTool.temp_dir_vector.normalized().dot(encodeTool.temp_vector.normalized());
			if(cos_temp_theta > cos(theta))
			{
				ResourceMetas.push_back(temp_ResourceMetas[j]);				
			}
		}
	}
	return ResourceMetas;
}
//********************************
std::vector<sigma::ResourceMeta> DBConnect::loadResourceMeta(sigma::Vector4 *Origin, sigma::Vector4 *Range, sigma::Target *target, double alpha, double beta, int Type)
{
	
	std::vector<sigma::ResourceMeta> ResourceMetas;
	std::vector<sigma::ResourceMeta> temp_ResourceMetas;
	std::vector<sigma::Geometry> Geometries;
	EncodeTool encodeTool;
	int i,j;

	double dir_x,dir_y,dir_z;
	double temp_x,temp_t,temp_z;
	double cos_temp_theta,cos_temp_theta_A,cos_temp_theta_B;
	double resultA,resultB;
	double roll,pitch,yaw;
	
	Geometries = loadGeometry(Origin,Range,4);

	for(i = 0;i < Geometries.size();i++)
	{
		double tmp = (Geometries[i].Posx_est==0) ? Geometries[i].Posx_phys : Geometries[i].Posx_est;
		dir_x = target->targetX - tmp;
		tmp = (Geometries[i].Posy_est==0) ? Geometries[i].Posy_phys : Geometries[i].Posy_est;
		dir_y = target->targetY - tmp;
		tmp = (Geometries[i].Posz_est==0) ? Geometries[i].Posz_phys : Geometries[i].Posz_est;
		dir_z = target->targetZ - tmp;
		temp_ResourceMetas = loadResourceMeta(Geometries[i].ID, Type);
		
		for(j=0;j<temp_ResourceMetas.size();j++)
		{
			Eigen::Vector3d temp_z(0,0,1),temp_y(0,1,0),temp_x(1,0,0),temp_dir_vector_y(0,0,0),temp_dir_vector_x(0,0,0),temp_dir_vector_z(0,0,0);
			Eigen::Vector3d temp_vector(0,0,0),init_vector(0,0,0);
			Eigen::Vector3d vector_weight_with_z(0,0,0),vector_weight_notwith_z(0,0,0);
			Eigen::Matrix3d rotMatrix,R;
			
			double Rx = temp_ResourceMetas[j].transform.Attix;
			double Ry = temp_ResourceMetas[j].transform.Attiy;
			double Rz = temp_ResourceMetas[j].transform.Attiz;
			double Rw = temp_ResourceMetas[j].transform.Attiw;
	
			R = encodeTool.Quaternion2RotationMatrix(Rx,Ry,Rz,Rw);
			init_vector << 0,0,1;
			temp_vector = (R * init_vector).normalized();  //姿势向量
			temp_dir_vector_z << dir_x,dir_y,dir_z;  //摄像机Z轴
			temp_dir_vector_z = temp_dir_vector_z.normalized();

			rotMatrix = Eigen::Quaterniond::FromTwoVectors(temp_z.normalized(),temp_dir_vector_z.normalized());
			temp_dir_vector_y = (rotMatrix * temp_y).normalized(); //摄像机y轴
			temp_dir_vector_x = (rotMatrix * temp_x).normalized(); //摄像机x轴

			cos_temp_theta = temp_dir_vector_z.dot(temp_vector);
			vector_weight_with_z = temp_vector * cos_temp_theta;
			vector_weight_notwith_z = temp_vector - vector_weight_with_z;

			cos_temp_theta_A = vector_weight_notwith_z.dot(temp_dir_vector_x);
			cos_temp_theta_B = vector_weight_notwith_z.dot(temp_dir_vector_y);

			resultA = (vector_weight_notwith_z * cos_temp_theta_A).norm();
			resultB = (vector_weight_notwith_z * cos_temp_theta_B).norm();

			if(resultA <= sin(alpha) && resultB <= sin(beta) && resultA >=0 && resultB >= 0)
			{
				ResourceMetas.push_back(temp_ResourceMetas[j]);				
			}
		}
	}
	return ResourceMetas;
}
