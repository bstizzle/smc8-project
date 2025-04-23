/*
  ==============================================================================

    Globals.h
    Created: 10 Apr 2025 3:16:53pm
    Author:  where

  ==============================================================================
*/

#pragma once
#include "SimpleString.h"
#include "BodyManager.h"
#include <chrono>


extern float posX;
extern float posZ;
extern double abs_velX;
//extern int nbrBodies;
//extern int lowest_id;

extern BodyManager bodies_dict;

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