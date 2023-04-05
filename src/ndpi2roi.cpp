#include <iostream>
#include "kb_openslide.h"
#include "kb_xml_io.h"
#include "kb_xml_ndpa.h"
#include "kb_searchfiles.h"
#include "kb_split_string.h"
#include <direct.h>
#include "kb_cv_resize.h"

//ルックアップテーブル変換  
int LUT_RGB(cv::Mat& mat1, int lut[256])
{
	cv::Size sz1 = mat1.size();

	for (int j = 0; j < sz1.height; j++) {
		for (int i = 0; i < sz1.width; i++) {
			cv::Vec3b a = mat1.at<cv::Vec3b>(j, i);
			cv::Vec3b b(lut[a[0]], lut[a[1]], lut[a[2]]);
			mat1.at<cv::Vec3b>(j, i) = b;
		}
	}
	return 0;
}

int main(int argc, char* argv[])
{
	std::string dirname_input_ndpi = argv[1];
	std::string dirname_input_ndpa = argv[2];
	std::string dirname_output = argv[3];
	double gamma1 = std::atof(argv[4]);
	double ratio1= std::atof(argv[5]);
	int width1= std::atoi(argv[6]);

	std::cout << dirname_input_ndpi << std::endl;
	std::cout << dirname_output << std::endl;


	std::vector<std::string>  vfnames;
	if (kb::search_files(dirname_input_ndpa, "*.ndpa", vfnames) < 0)
		return -1;

	int num_files = vfnames.size();

	std::string dname_1 = dirname_output + "HSIL/";
	std::string dname_2 = dirname_output + "LSIL/";
	std::string dname_3 = dirname_output + "SIL/";
	std::string dname_4 = dirname_output + "ASCH/";
	std::string dname_5 = dirname_output + "ASCUS/";
	std::string dname_6 = dirname_output + "else/";
	_mkdir(dname_1.c_str());
	_mkdir(dname_2.c_str());
	_mkdir(dname_3.c_str());
	_mkdir(dname_4.c_str());
	_mkdir(dname_5.c_str());
	_mkdir(dname_6.c_str());


	int lut[256];
	{
		//ガンマ補正テーブルの作成  
		for (int i = 0; i < 256; i++)
		{
			lut[i] = (int)(pow((double)i / 255.0, 1.0 / gamma1) * 255.0);
		}
	}

	for (int kk = 0; kk < num_files; kk++) {
		std::string path_ndpa = dirname_input_ndpa + vfnames[kk];
		std::cout << "---- " << kk << " / " << num_files << " ----" << std::endl;
		std::cout << path_ndpa << std::endl;

		int len_fname = vfnames[kk].length();
		std::string fname_ndpi = vfnames[kk].substr(0, len_fname - 5);
		std::string path_input = dirname_input_ndpi + fname_ndpi;
		std::string path_output = dirname_output + fname_ndpi + ".jpg";

		std::string ftitle1, fext1;
		kb::split_filename(fname_ndpi, ftitle1, fext1);


		kb::COpenslide slide1;
		if (slide1.create(path_input) < 0)
			return -1;
		slide1.m_info.print();



		std::vector<kb::slide_annotation> v_ndpa;
		if (kb::load_ndpa(path_ndpa, v_ndpa, slide1.m_info) < 0)
			return -1;

		int num_ndpa = v_ndpa.size();
		for (int k = 0; k < num_ndpa; k++) {
			//std::cout << v_ndpa[k].m_title << std::endl;
			int num = v_ndpa[k].m_pointlist_d.size();

			//if (v_ndpa[k].m_title == "show area")
			//	continue;


			//	doctor's annotation
			if (v_ndpa[k].m_annotation_type == "circle") {
				std::cout << k << ": " <<
					v_ndpa[k].m_annotation_type << " " <<
					v_ndpa[k].m_annotation_radius << " ";//

				cv::Mat mat1;
				if (num == 2) {
					cv::Point2d p1(v_ndpa[k].m_pointlist_d[0], v_ndpa[k].m_pointlist_d[1]);
					int w = width1;
					slide1.getImage(mat1, p1.x - w / 2, p1.y - w / 2, w, w, 0);
					LUT_RGB(mat1, lut);

					char buf1[256];
					snprintf(buf1, 256, "%s_%d_%d", ftitle1.c_str(), (int)p1.x, (int)p1.y);
					cv::Mat mat2;
					kb::resize(mat1, mat2, ratio1);

					//	red
					if (v_ndpa[k].m_color[0] == 0 && v_ndpa[k].m_color[1] == 0 && v_ndpa[k].m_color[2] == 255) {
						std::cout << "red" << std::endl;
						std::string path1 = dname_1 + buf1 + ".jpg";
						cv::imwrite(path1, mat2);
					}
					//	yellow
					else if (v_ndpa[k].m_color[0] == 0 && v_ndpa[k].m_color[1] == 255 && v_ndpa[k].m_color[2] == 255) {
						std::cout << "yellow" << std::endl;
						std::string path1 = dname_2 + buf1 + ".jpg";
						cv::imwrite(path1, mat2);
					}
					//	white
					else if (v_ndpa[k].m_color[0] == 255 && v_ndpa[k].m_color[1] == 255 && v_ndpa[k].m_color[2] == 255) {
						std::cout << "white" << std::endl;
						std::string path1 = dname_3 + buf1 + ".jpg";
						cv::imwrite(path1, mat2);
					}
					//	purple
					else if (v_ndpa[k].m_color[0] == 255 && v_ndpa[k].m_color[1] == 0 && v_ndpa[k].m_color[2] == 255) {
						std::cout << "purple" << std::endl;
						std::string path1 = dname_4 + buf1 + ".jpg";
						cv::imwrite(path1, mat2);
					}
					//	aqua blue
					else if (v_ndpa[k].m_color[0] == 255 && v_ndpa[k].m_color[1] == 255 && v_ndpa[k].m_color[2] == 0) {
						std::cout << "aqua blue" << std::endl;
						std::string path1 = dname_5 + buf1 + ".jpg";
						cv::imwrite(path1, mat2);
					}
					//
					else {
						std::cout << "else" << std::endl;
						std::string path1 = dname_6 + buf1 + ".jpg";
						cv::imwrite(path1, mat2);
					}
				}
			}
		}

	}



	return 0;
}

