#pragma once

#include <JuceHeader.h>
#include "SimpleString.h"
#include "Globals.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
                       public Timer // for graphics refresh
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    double limit (double val); // limiter for your ears
    
    void timerCallback() override;


    std::list<float> xmapped_xpos_frets;
    std::list<float> ymapped_zpos_frets;

private:
    //==============================================================================
    // Your private member variables go here...
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent);
};
