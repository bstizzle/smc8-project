#include <lo/lo.h>
#include <iostream>

#include "detected_object.h"
#include "message_handling.h"


// Callback function for received messages
int get_coordinates(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data) {
    SharedState* state = static_cast<SharedState*>(user_data);
    state->x = argv[0]->f;
    state->y = argv[1]->f;
    state->z = argv[2]->f;

    //float x=argv[0]->f;
    //float y=argv[1]->f;
    //float z=argv[2]->f;
    std::cout << "Received message on " << path << ": " <<  state->x  <<  state->y <<  state->z<< std::endl;

    return 0;
}


int get_id(const char *path, const char *types, lo_arg ** argv, int argc, lo_message msg, void *user_data){
    float id = argv[0]->i;
    SharedState* state = static_cast<SharedState*>(user_data);
    state->obj_id = argv[0]->i;
    std::cout << "ID received: " << state->obj_id << std::endl;

    return 0;
}
