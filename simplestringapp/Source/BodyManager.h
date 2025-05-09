#pragma once

#include <JuceHeader.h> // Includes JUCE core and containers like juce::HashMap
//#include <juce_gui_basics/juce_gui_basics.h>


// Define a struct to hold velocity and position
struct BodyData
{
    float velx;
    float vely;
    float posx;
    float posz;
    float posy;
    float prev_posx;
    float prev_posz;
    juce::Colour color;
};
// Then declare the outer HashMap
//extern HashMap<String, BodyData> bodies;

class BodyManager
  {
    public:
        BodyManager();
        juce::HashMap<int, BodyData>& getBodies();// const;// { return bodies_dict; }
        void addOrUpdateBody(int id, float velx, float vely, float posx, float posz, float posy,float prev_posx, float prev_posz);
        BodyData getBody(int id) const;
        bool hasBody(int id) const;
        void removeBody(int id);
        void clearAllBodies();
        juce::Colour assign_colour(int id, int nbr_colors);


    private:
        juce::HashMap<int, BodyData> bodies;
  };
