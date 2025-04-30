/*
  ==============================================================================

    Globals.cpp
    Created: 10 Apr 2025 3:16:59pm
    Author:  where

  ==============================================================================
*/

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

BodyManager bodies_dict;


std::list<float> xpos_frets= {-1.9, -1.8, -1.4, -1.3, -.9, -.8, -.4,-.3, .1, .2, .6, .7};
std::list<float> zpos_frets= {-5.5, -6.18, -6.68, -7.54, -8.22};

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