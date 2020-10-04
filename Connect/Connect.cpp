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

	if(Mode == 1)  //Sphere
	{
		if(Range->x > 0 && Range->y > 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,(POW(Posx_est-'%f',2)/POW('%f',2))+(POW(Posy_est-'%f',2)/POW('%f',2))+(POW(Posz_est-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),(POW(Posx_phys-'%f',2)/POW('%f',2))+(POW(Posy_phys-'%f',2)/POW('%f',2))+(POW(Posz_phys-'%f',2)/POW('%f',2))<1 AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'))", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		
		else if(Range->x > 0 && Range->y > 0 && Range->z > 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x > 0 && Range->y > 0 && Range->z <= 0 && Range->t_s > 0)
		{
			
		}
		else if(Range->x > 0 && Range->y > 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z > 0 && Range->t_s > 0)
		{
			
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z > 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z > 0 && Range->t_s > 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z > 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s > 0)
		{
			
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z <= 0 && Range->t_s > 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z > 0 && Range->t_s > 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z > 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s > 0)
		{

		}
		else
		{
			
		}
	}
	else if(Mode == 2) //Cube
	{
		if(Range->x > 0 && Range->y > 0 && Range->z > 0 && Range->t_s > 0)
		{
			sql_len = sprintf(sql, "select * from server.geometry where IF(Posx_est IS NOT NULL,ABS(Posx_est-'%f')<='%f' AND ABS(Posy_est-'%f')<='%f' AND ABS(Posz_est-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'),ABS(Posx_phys-'%f')<='%f' AND ABS(Posy_phys-'%f')<='%f' AND ABS(Posz_phys-'%f')<='%f' AND IF(Time_s='%u',Time_ms>='%u',Time_s>='%u') AND IF(Time_s='%u',Time_ms<='%u',Time_s<='%u'))", Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s, Origin->x, Range->x/2, Origin->y, Range->y/2, Origin->z, Range->z/2, Origin->t_s, Origin->t_ms, Origin->t_s, Range->t_s, Range->t_ms, Range->t_s);
		}
		
		else if(Range->x > 0 && Range->y > 0 && Range->z > 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x > 0 && Range->y > 0 && Range->z <= 0 && Range->t_s > 0)
		{
			
		}
		else if(Range->x > 0 && Range->y > 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z > 0 && Range->t_s > 0)
		{
			
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z > 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z > 0 && Range->t_s > 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z > 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s > 0)
		{
			
		}
		else if(Range->x > 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z <= 0 && Range->t_s > 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y > 0 && Range->z <= 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z > 0 && Range->t_s > 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z > 0 && Range->t_s <= 0)
		{
			
		}
		else if(Range->x <= 0 && Range->y <= 0 && Range->z <= 0 && Range->t_s > 0)
		{

		}
		else
		{
			
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


