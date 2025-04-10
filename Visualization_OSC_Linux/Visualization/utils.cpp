#include <iostream>

#include "utils.h"


float map_x_coord( float x, float x_min, float x_max, float range_min, float range_max){
    return ((x - x_min)*(range_max-range_min)/(x_max-x_min)) + range_min;
}

float map_y_coord( float y, float y_min, float y_max, float range_min, float range_max){
    return (((y - y_min)*(range_max-range_min)/(y_max-y_min)) + range_min);
}

float map_z_coord( float z, float z_min, float z_max, float range_min, float range_max){
    return (((z - z_min)*(range_max-range_min)/(z_max-z_min)) + range_min);
}


