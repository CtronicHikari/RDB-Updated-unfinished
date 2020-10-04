#include "Common.h"

extern bool xmlFileGenerator(const std::vector<sigma::ResourceMeta> resourcemetas, const std::string path) {

	//保存するものがあるかどうか
	if (resourcemetas.empty()) {
		std::cout << "保存するファイルがありません" << std::endl;
		return false;
	}

	boost::system::error_code error;
	const bool result = boost::filesystem::exists(path, error);
	if (!result || error) {
		std::cout << "ファイルがないので作成します" << std::endl;

		const bool result = boost::filesystem::create_directory(path, error);
		if (!result || error) {
			std::cout << "ディレクトリの作成に失敗" << std::endl;

			return false;
		}
	}

	boost::property_tree::ptree pt;
	for (auto& v : resourcemetas) {

		boost::property_tree::ptree& child = pt.add("resourcemeta", "");
		child.add("ID", v.ID);
		child.add("ParentObjectID", v.ParentObjectID);
		child.add("Type", v.Type);
		child.add("Ext", v.Ext);
		child.add("Path", v.Path);
		child.add("Parameters", v.Parameters);

		boost::property_tree::ptree& child2 = child.add("transform", "");
		child2.add("Attiw", v.transform.Attiw);
		child2.add("Attix", v.transform.Attix);
		child2.add("Attiy", v.transform.Attiy);
		child2.add("Attiz", v.transform.Attiz);
		child2.add("Time_s", v.transform.Time_s);
		child2.add("Time_ms", v.transform.Time_ms);
		child2.add("Posx", v.transform.Posx);
		child2.add("Posy", v.transform.Posy);
		child2.add("Posz", v.transform.Posz);
	}

	const int indent = resourcemetas.size();
	boost::property_tree::write_xml(path + "/meta.xml", pt, std::locale(),
		boost::property_tree::xml_writer_make_settings<std::string>(' ', indent));

	return true;
};

void xmlFileParser(std::vector<sigma::ResourceMeta>& resourcemetametas, const std::string path) {

	boost::property_tree::ptree pt;

	try {
		read_xml(path + "/meta.xml", pt);

		for (auto tree : pt) {

			sigma::Transform t(
				tree.second.get<double>("transform.Posx"),
				tree.second.get<double>("transform.Posy"),
				tree.second.get<double>("transform.Posz"),
				tree.second.get<unsigned int>("transform.Time_s"),
				tree.second.get<unsigned int>("transform.Time_ms"),
				tree.second.get<double>("transform.Attix"),
				tree.second.get<double>("transform.Attiy"),
				tree.second.get<double>("transform.Attiz"),
				tree.second.get<double>("transform.Attiw"));

			sigma::ResourceMeta r(tree.second.get<std::string>("ID"),
				tree.second.get<int>("ParentObjectID"),
				tree.second.get<int>("Type"),
				tree.second.get<std::string>("Ext"),
				t,
				tree.second.get<std::string>("Path"),
				tree.second.get<std::string>("Parameters"));

			resourcemetametas.emplace_back(r);
		}
	}
	catch (std::exception const& ex) {
		std::cout << "xml parse error:" << ex.what() << std::endl;
	}
};

void parameterFileParser(std::vector<pairIE>& pairie, std::vector<intrinsicPara>& intrinsicpara,
	std::vector<extrinsicPara>& extrinsicpara, const std::string path) {

	boost::property_tree::ptree pt;

	try {
		read_xml(path, pt);

		intrinsicPara ip;
		extrinsicPara ep;
		pairIE ie;

		std::string folder;

		for (auto tree : pt) {

			for (auto& it : tree.second.get_child("")) {

				if (it.first == "root_path") folder = boost::lexical_cast<std::string>(it.second.data());

				//内部と外部のペア
				else if (it.first == "views") {
					for (auto& it2 : it.second.get_child("")) {
						for (auto& it3 : it2.second.get_child("")) {

							if (it3.first == "value") {

								for (auto& it4 : it3.second.get_child("")) {

									if (it4.first == "ptr_wrapper") {

										for (auto& it5 : it4.second.get_child("")) {

											if (it5.first == "data") {

												for (auto& it6 : it5.second.get_child("")) {

													if (it6.first == "filename") {
														ie.path = folder + "/" + boost::lexical_cast<std::string>(it6.second.data());
													}

													else if (it6.first == "id_intrinsic") {
														ie.i_id = boost::lexical_cast<int>(it6.second.data());
													}

													else if (it6.first == "id_pose") {
														ie.e_id = boost::lexical_cast<int>(it6.second.data());
													}
												}
												pairie.emplace_back(ie);
											}
										}

									}
								}
							}
						}
					}
				}//else if (it.first == "views") {
			}
			//この時点でview idは全て見ているので，
			intrinsicpara.resize(pairie.size());
			extrinsicpara.resize(pairie.size());
			int key = 0;

			for (auto& it : tree.second.get_child("")) {
				
				//内部パラメータの場合
				if (it.first == "intrinsics") {

					for (auto& it2 : it.second.get_child("")) {
						for (auto& it3 : it2.second.get_child("")) {

							if (it3.first == "key") {
								key = boost::lexical_cast<int>(it3.second.data());
							}
							
							else if (it3.first == "value") {

								for (auto& it4 : it3.second.get_child("")) {

									if (it4.first == "ptr_wrapper") {

										for (auto& it5 : it4.second.get_child("")) {

											if (it5.first == "data") {

												for (auto& it6 : it5.second.get_child("")) {

													if (it6.first == "width") {
														ip.width = boost::lexical_cast<int>(it6.second.data());
													}

													else if (it6.first == "height") {
														ip.height = boost::lexical_cast<int>(it6.second.data());
													}

													else if (it6.first == "focal_length") {
														ip.focal = boost::lexical_cast<double>(it6.second.data());
													}

													else if (it6.first == "principal_point") {

														for (auto& it7 : it6.second.get_child("")) {

															if (it7.first == "value0") {
																ip.ppx = boost::lexical_cast<double>(it7.second.data());
															}

															else if (it7.first == "value1") {
																ip.ppy = boost::lexical_cast<double>(it7.second.data());
															}
														}
													}

													else if (it6.first == "disto_k3") {

														for (auto& it7 : it6.second.get_child("")) {

															if (it7.first == "value0") {
																ip.k1 = boost::lexical_cast<double>(it7.second.data());
															}

															else if (it7.first == "value1") {
																ip.k2 = boost::lexical_cast<double>(it7.second.data());
															}

															else if (it7.first == "value2") {
																ip.k3 = boost::lexical_cast<double>(it7.second.data());
															}
														}
													}
												}

												intrinsicpara[key] = ip;
											}
										}
									}
								}
							}
						}
					}
				}//内部パラメータの場合

				//外部パラメータの場合
				else if (it.first == "extrinsics") {

					for (auto& it2 : it.second.get_child("")) {
						for (auto& it3 : it2.second.get_child("")) {

							if (it3.first == "key") {
								key = boost::lexical_cast<int>(it3.second.data());
							}

							else if (it3.first == "value") {

								for (auto& it4 : it3.second.get_child("")) {
									if (it4.first == "rotation") {

										for (auto& it5 : it4.second.get_child("")) {
											if (it5.first == "value0") {

												for (auto& it6 : it5.second.get_child("")) {
													if (it6.first == "value0") {
														ep.r[0] = boost::lexical_cast<double>(it6.second.data());
													}

													else if (it6.first == "value1") {
														ep.r[1] = boost::lexical_cast<double>(it6.second.data());
													}

													else if (it6.first == "value2") {
														ep.r[2] = boost::lexical_cast<double>(it6.second.data());
													}
												}
											}

											else if (it5.first == "value1") {
												for (auto& it6 : it5.second.get_child("")) {
													if (it6.first == "value0") {
														ep.r[3] = boost::lexical_cast<double>(it6.second.data());
													}

													else if (it6.first == "value1") {
														ep.r[4] = boost::lexical_cast<double>(it6.second.data());
													}

													else if (it6.first == "value2") {
														ep.r[5] = boost::lexical_cast<double>(it6.second.data());
													}
												}
											}

											else if (it5.first == "value2") {

												for (auto& it6 : it5.second.get_child("")) {

													if (it6.first == "value0") {
														ep.r[6] = boost::lexical_cast<double>(it6.second.data());
													}

													else if (it6.first == "value1") {
														ep.r[7] = boost::lexical_cast<double>(it6.second.data());
													}

													else if (it6.first == "value2") {
														ep.r[8] = boost::lexical_cast<double>(it6.second.data());
													}
												}
											}
										}
									}

									if (it4.first == "center") {

										for (auto& it5 : it4.second.get_child("")) {

											if (it5.first == "value0") {
												ep.t[0] = boost::lexical_cast<double>(it5.second.data());
											}

											else if (it5.first == "value1") {
												ep.t[1] = boost::lexical_cast<double>(it5.second.data());
											}

											else if (it5.first == "value2") {
												ep.t[2] = boost::lexical_cast<double>(it5.second.data());
											}
										}
									}
								}
								extrinsicpara[key] = ep;
							}
						}
					}
				}//外部パラメータの場合
			}
		}
	}

	catch (std::exception const& ex) {
		std::cout << "xml parse error:" << ex.what() << std::endl;
	}
}

std::string strGenerator(intrinsicPara& _intrinsicpara) {

	//double focal;→8byte
	//double ppx;→8byte
	//double ppy;→8byte
	//double k1;→8byte
	//double k2;→8byte
	//double k3;→8byte
	//int width;→4byte
	//int height;→4byte  計56byte

	char m[56];
	std::memcpy(&m[0], &_intrinsicpara.focal, sizeof(double));
	std::memcpy(&m[8], &_intrinsicpara.ppx, sizeof(double));
	std::memcpy(&m[16], &_intrinsicpara.ppy, sizeof(double));
	std::memcpy(&m[24], &_intrinsicpara.k1, sizeof(double));
	std::memcpy(&m[32], &_intrinsicpara.k2, sizeof(double));
	std::memcpy(&m[40], &_intrinsicpara.k3, sizeof(double));
	std::memcpy(&m[44], &_intrinsicpara.width, sizeof(int));
	std::memcpy(&m[48], &_intrinsicpara.height, sizeof(int));

	std::string tmp(m);

	return tmp;

};

std::string strGeneratorCSV(intrinsicPara& _intrinsicpara) {

	std::string tmp = std::to_string(_intrinsicpara.focal);
	tmp += ("," + std::to_string(_intrinsicpara.ppx));
	tmp += ("," + std::to_string(_intrinsicpara.ppy));
	tmp += ("," + std::to_string(_intrinsicpara.k1));
	tmp += ("," + std::to_string(_intrinsicpara.k2));
	tmp += ("," + std::to_string(_intrinsicpara.k3));
	tmp += ("," + std::to_string(_intrinsicpara.width));
	tmp += ("," + std::to_string(_intrinsicpara.height));

	return tmp;

};


void strParser(intrinsicPara& _intrinsicpara, std::string str) {

	const char* m = str.c_str();
	double aa = 0.4;
	std::memcpy(&_intrinsicpara.focal, &m[0], sizeof(double));
	std::memcpy(&_intrinsicpara.ppx, &m[8], sizeof(double));
	std::memcpy(&_intrinsicpara.ppy, &m[16], sizeof(double));
	std::memcpy(&_intrinsicpara.k1, &m[24], sizeof(double));
	std::memcpy(&_intrinsicpara.k2, &m[32], sizeof(double));
	std::memcpy(&_intrinsicpara.k3, &m[40], sizeof(double));
	std::memcpy(&_intrinsicpara.width, &m[44], sizeof(int));
	std::memcpy(&_intrinsicpara.height, &m[48], sizeof(int));
};

void strParserCSV(intrinsicPara& _intrinsicpara, std::string str) {

	boost::tokenizer< boost::escaped_list_separator< char > > tokens(str);
	for (auto it = tokens.begin(); it != tokens.end(); ++it) {

		if (std::distance(tokens.begin(), it) == 0) _intrinsicpara.focal = std::stod(*it);
		else if (std::distance(tokens.begin(), it) == 1) _intrinsicpara.ppx = std::stod(*it);
		else if (std::distance(tokens.begin(), it) == 2) _intrinsicpara.ppy = std::stod(*it);
		else if (std::distance(tokens.begin(), it) == 3) _intrinsicpara.k1 = std::stod(*it);
		else if (std::distance(tokens.begin(), it) == 4) _intrinsicpara.k2 = std::stod(*it);
		else if (std::distance(tokens.begin(), it) == 5) _intrinsicpara.k3 = std::stod(*it);
		else if (std::distance(tokens.begin(), it) == 6) _intrinsicpara.width = std::stod(*it);
		else if (std::distance(tokens.begin(), it) == 7) _intrinsicpara.height = std::stod(*it);
	}
}

glm::quat convertM2Q(double* m) {

	float _m[16];
	_m[0] = (float)m[0];
	_m[1] = (float)m[1];
	_m[2] = (float)m[2];
	_m[3] = 0.f;
	_m[4] = (float)m[3];
	_m[5] = (float)m[4];
	_m[6] = (float)m[5];
	_m[7] = 0.f;
	_m[8] = (float)m[6];
	_m[9] = (float)m[7];
	_m[10] = (float)m[8];
	_m[11] = 0.f;
	_m[12] = 0.f;
	_m[13] = 0.f;
	_m[14] = 0.f;
	_m[15] = 1.f;

	glm::mat4 mat = glm::make_mat4(_m);
	return glm::toQuat(mat);
};

void convertQ2M(glm::quat q, double* m) {

	glm::mat4 mat = glm::toMat4(q);

	float _m[16];
	memcpy(_m, glm::value_ptr(mat), sizeof(float) * 16);

	m[0] = (double)_m[0];
	m[1] = (double)_m[1];
	m[2] = (double)_m[2];
	m[3] = (double)_m[4];
	m[4] = (double)_m[5];
	m[5] = (double)_m[6];
	m[6] = (double)_m[8];
	m[7] = (double)_m[9];
	m[8] = (double)_m[10];
}
