
#include "BodyManager.h"

BodyManager::BodyManager() {}


juce::HashMap<int, BodyData>& BodyManager::getBodies()
{
    return bodies;
}

void BodyManager::addOrUpdateBody(int id, float velx, float vely, float posx, float posz, float posy)
{
    bodies.set(id, BodyData{ velx, vely, posx, posz, posy, juce::Colours::red});
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
