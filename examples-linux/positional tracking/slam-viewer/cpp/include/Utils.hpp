#ifndef UTILS_H
#define UTILS_H

// ZED includes
#include <sl/Camera.hpp>

// OpenCV includes
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

// For Foxglove WS:
#include "MCAPWriter.h"

std::string parseArgs(int argc, char **argv, sl::InitParameters &param);
cv::Mat slMat2cvMat(sl::Mat& input);
void cvtMatrix4f(const sl::Matrix4f &in, Eigen::Matrix4f &out); // Temporary for converting Eigen::Matrix4f to sl::Transform
void print(std::string msg_prefix, sl::ERROR_CODE err_code, std::string msg_suffix = "[sample]");

#endif