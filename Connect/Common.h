#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OpenCV.h"
#include "MyClass.h"
/*
class Transform {
public:

    Transform() {

    };

    Transform(const double WorldPositionX, const double WorldPositionY, const double WorldPositionZ,
        const double Time, const double AttitudeX, const double AttitudeY, const double AttitudeZ, const double AttitudeW) :
        WorldPositionX(WorldPositionX), WorldPositionY(WorldPositionY), WorldPositionZ(WorldPositionZ),
        Time(Time), AttitudeX(AttitudeX), AttitudeY(AttitudeY), AttitudeZ(AttitudeZ), AttitudeW(AttitudeW){

    };

    ~Transform() {

    };

    double WorldPositionX;
    double WorldPositionY;
    double WorldPositionZ;
    double Time;
    double AttitudeX;
    double AttitudeY;
    double AttitudeZ;
    double AttitudeW;
};
*/
/*
class ResourceMeta {

public:

    ResourceMeta() {

    };

    ResourceMeta(const std::string &ID, const int ParentObjectID, const int Type,
        const std::string Ext, const Transform &transform, const std::string Path, const std::string Parameters) :
        ID(ID), ParentObjectID(ParentObjectID), Type(Type),
        Ext(Ext), transform(transform), Path(Path), Parameters(Parameters){

    };

    ~ResourceMeta() {

    }

	std::string ID;
    int ParentObjectID;
    int Type;
    std::string Ext;
    Transform transform;
    std::string Path;
    std::string Parameters;
};
*/
class intrinsicPara {

public:
	intrinsicPara() :
        focal(0.f), ppx(0.f), ppy(0.f), k1(0.f), k2(0.f), k3(0.f), width(0), height(0) {
	};

	intrinsicPara(const double focal, const double ppx, const double ppy,
		const double k1, const double k2, const double k3, const int width, const int height) :
		focal(focal), ppx(ppx), ppy(ppy), k1(k1), k2(k2), k3(k3), width(width), height(height){
	};

	~intrinsicPara() {
	};

	//焦点距離
	double focal;

	//主点
	double ppx;
	double ppy;

	//歪み係数
	double k1;
	double k2;
	double k3;

    //サイズ
    int width;
    int height;
};

class extrinsicPara {

public:
    extrinsicPara() {

        r[0] = r[1] = r[2] = r[3] = r[4] = r[5] = r[6] = r[7] = r[8] = 0.f;
        t[0] = t[1] = t[2] = 0.f;
    };

    ~extrinsicPara() {
    };

    //回転
    double r[9];

    //並進
    double t[3];
};

class pairIE {

public:
    pairIE() : i_id(0), e_id(0){

    };

    ~pairIE() {
    };

    //内部ID
    double i_id;

    //外部ID
    double e_id;

    //ファイルパス
    std::string path;
};

/**
 * @fn
 * @brief RDBからダウンロードしたリソースのメタ情報をxmlファイルに保存
 * @param const std::vector<ResourceMeta> resourcemeta 保存するvector
 * @param const std::string path 保存するディレクトリパス
 */
extern bool xmlFileGenerator(const std::vector<sigma::ResourceMeta> resourcemetas, const std::string path);

/**
 * @fn
 * @brief RDBからダウンロードしたリソースのメタ情報をxmlファイルに保存
 * @std::vector<ResourceMeta> & resourcemetametas xmlファイルを読み，resourcemetametasのvectorに格納
 * @param const std::string path 保存してあるディレクトリパス
 */
extern void xmlFileParser(std::vector<sigma::ResourceMeta>& resourcemetas, const std::string path);

/**
 * @fn
 * @brief OpenMVGで出力したxmlファイルを解析
 * @std::vector<pairIE>& pairie xmlファイルを読み，pairIEのvectorに格納
 * @std::vector<intrinsicPara>& intrinsicpara xmlファイルを読み，intrinsicParaのvectorに格納
 * @std::vector<extrinsicPara>& extrinsicpara xmlファイルを読み，extrinsicParaのvectorに格納
 * @param const std::string path 保存してあるディレクトリパス
 */
extern void parameterFileParser(std::vector<pairIE>& pairie, std::vector<intrinsicPara>& intrinsicpara, std::vector<extrinsicPara>& extrinsicpara, const std::string path);

/**
 * @fn
 * @brief カメラの内部パラメータから文字列を作成
 * @intrinsicPara _intrinsicpara intrinsicPara
 * @return 文字列
 */
extern std::string strGenerator(intrinsicPara& _intrinsicpara);

//strGeneratorのcsv版
extern std::string strGeneratorCSV(intrinsicPara &_intrinsicpara);

/**
 * @fn
 * @brief strGeneratorで作成した文字列を解析
 * @intrinsicPara _intrinsicpara intrinsicPara
 * @std::string str 作成した文字列
 */
extern void strParser(intrinsicPara& _intrinsicpara, std::string str);

//strParserのcsv版
extern void strParserCSV(intrinsicPara& _intrinsicpara, std::string str);

/**
 * @fn
 * @brief 配列を入れるとglm形式のクォータニオンが返って来る
 * @double* m 配列(3行3列)
 */
extern glm::quat convertM2Q(double* m);

/**
 * @fn
 * @brief 配列を入れるとglm形式のクォータニオンが返って来る
 * @glm::quat q　glm形式のクォータニオン 
 * @double* m 配列(3行3列)
 */
extern void convertQ2M(glm::quat q, double* m);

