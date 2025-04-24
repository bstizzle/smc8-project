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
double abs_velX;
std::list<int> id_list;

BodyManager bodies_dict;


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