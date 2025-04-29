#pragma once

#include <JuceHeader.h> // Includes JUCE core and containers like juce::HashMap


// Define a struct to hold velocity and position
struct BodyData
{
    float velx;
    float vely;
    float posx;
    float posz;
    float posy;
    juce::Colour color;

};
// Then declare the outer HashMap
//extern HashMap<String, BodyData> bodies;

class BodyManager
  {
    public:
        BodyManager();
        juce::HashMap<int, BodyData>& getBodies();// const;// { return bodies_dict; }
        void addOrUpdateBody(int id, float velx, float vely, float posx, float posz, float posy);
        BodyData getBody(int id) const;
        bool hasBody(int id) const;
        void removeBody(int id);
        void clearAllBodies();

    private:
        juce::HashMap<int, BodyData> bodies;
  };
