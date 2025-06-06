
#pragma once
#include "SimpleString.h"
#include "BodyManager.h"
#include <chrono>


extern float posX;
extern float posZ;
extern float posY;
extern float pluckWidth;
extern double abs_velX;
extern double abs_velY;
extern std::list<int> id_list;

extern float rWristY;
extern float chestY;
extern bool pluckPrimer1;

extern BodyManager bodies_dict;
extern std::vector<float> string_limit;
extern std::vector<float> zpos_frets;

extern std::unique_ptr<SimpleString> string1;
extern std::unique_ptr<SimpleString> string2;
extern std::unique_ptr<SimpleString> string3;
extern std::unique_ptr<SimpleString> string4;
extern std::unique_ptr<SimpleString> string5;
extern std::unique_ptr<SimpleString> string6;

extern double tune1;
extern double tune2;
extern double tune3;
extern double tune4;
extern double tune5;
extern double tune6;

extern double f1;
extern double f2;
extern double f3;
extern double f4;
extern double f5;
extern double f6;