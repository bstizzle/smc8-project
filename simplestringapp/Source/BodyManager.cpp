
#include "BodyManager.h"
#include <vector>
BodyManager::BodyManager() {}



juce::HashMap<int, BodyData>& BodyManager::getBodies()
{
    return bodies;
}

void BodyManager::addOrUpdateBody(int id, float velx, float posx, float posz)
{
    bodies.set(id, BodyData{ velx, posx, posz});
}

BodyData BodyManager::getBody(int id) const
{
    if (bodies.contains(id))
        return bodies[id];

    // Return a default BodyData if not found
    return BodyData{ 0.0f, 0.0f };
}

bool BodyManager::hasBody(int id) const
{
    return bodies.contains(id);
}

void BodyManager::removeBody(int id)
{
    bodies.remove(id);
}

void BodyManager::clearAllBodies()
{
    bodies.clear();
}



std::vector<juce::Colour> colourList = {
    juce::Colours::chocolate,
    juce::Colours::cyan,
    juce::Colours::red,
    juce::Colours::pink,
    juce::Colours::yellowgreen,
    juce::Colours::violet,
    juce::Colours::lime,
    juce::Colours::whitesmoke,
};

juce::Colour BodyManager::assign_colour(int id, int nbr_colors = 8){
    return colourList[id%nbr_colors];
}

