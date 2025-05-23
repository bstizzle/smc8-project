
#include "Globals.h"
#include "SimpleString.h"
#include "BodyManager.h"
#include <chrono>


float posX;
float posZ;
float posY;
double abs_velX;
double abs_velY;
std::list<int> id_list;

float rWristY;
float chestY;
bool pluckPrimer1 = false;

BodyManager bodies_dict;


//std::vector<float> string_limit= {-1.9, -1.4, -.9, -.4, .1, .6, 1.1};
//std::vector<float> string_limit = { -2.2, -1.65, -1.1, -0.55, 0.0, 0.55, 1.1 };
std::vector<float> string_limit = { -2.2, -1.62, -1.03, -0.45, 0.13, 0.72, 1.3 };
//std::vector<float> zpos_frets= {-4.7, -5.2, -5.7, -6.2, -6.7};
std::vector<float> zpos_frets = { -3.5, -4.3, -5.1, -5.9, -6.7, -7.7};

float pluckWidth;

std::unique_ptr<SimpleString> string1;
std::unique_ptr<SimpleString> string2;
std::unique_ptr<SimpleString> string3;
std::unique_ptr<SimpleString> string4;
std::unique_ptr<SimpleString> string5;
std::unique_ptr<SimpleString> string6;

double tune1;
double tune2;
double tune3;
double tune4;
double tune5;
double tune6;

double f1;
double f2;
double f3;
double f4;
double f5;
double f6;