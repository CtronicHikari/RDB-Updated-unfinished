#pragma once

#include <opencv2/opencv.hpp>

#define VERSION CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)

#if _DEBUG
#define DEBUG "d"
#else
#define DEBUG ""
#endif

#pragma comment(lib, "opencv_core" VERSION DEBUG ".lib")
#pragma comment(lib, "opencv_imgcodecs" VERSION DEBUG ".lib")
#pragma comment(lib, "opencv_imgproc" VERSION DEBUG ".lib")

