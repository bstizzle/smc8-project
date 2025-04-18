#include "MainComponent.h"
#include "Globals.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }
}

MainComponent::~MainComponent()
{
    // Stop the graphics update
    stopTimer();

    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    //// Set the paramters ///
    NamedValueSet parameters;
    
    // parameters you'll use to initialise more than one other parameter should be defined here
    double r = 0.0005;
    
    parameters.set ("L", 1);
    parameters.set ("rho", 7850);
    parameters.set ("A", r * r * double_Pi);
    parameters.set ("T", 299.75);
    parameters.set ("E", 2e11);
    parameters.set ("I", r * r * r * r * double_Pi * 0.25);
    parameters.set ("sigma0", 2);
    parameters.set ("sigma1", 0.005);
    

    //// Initialise an instance of the SimpleString class ////
    //string1 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 82.4);
    string1 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 146.83);
    //parameters.set ("L", 3);

    //string2 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 110.0);
    string2 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 196.0);
    //parameters.set ("L", 2);

    //string3 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 146.8);
    string3 = std::make_unique<SimpleString>(parameters, 1.0 / sampleRate, 261.63);
    //parameters.set ("L", 1);

    //string4 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 196.0);
    string4 = std::make_unique<SimpleString>(parameters, 1.0 / sampleRate, 349.6);
    //parameters.set ("L", 0.66);

    //string5 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 246.9);
    string5 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 440.0);
    //parameters.set ("L", 0.33);

    //string6 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 329.6);
    string6 = std::make_unique<SimpleString>(parameters, 1.0 / sampleRate, 587.3);


    addAndMakeVisible (string1.get()); // add the string to the application
    addAndMakeVisible (string2.get()); // add the string to the application
    addAndMakeVisible (string3.get()); // add the string to the application
    addAndMakeVisible (string4.get()); // add the string to the application
    addAndMakeVisible (string5.get()); // add the string to the application
    addAndMakeVisible (string6.get()); // add the string to the application

    // Call resized again as our components need a sample rate before they can get initialised.
    
    resized();

    startTimerHz (15); // start the timer (15 Hz is a nice tradeoff between CPU usage and update speed)
    
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    int numChannels = bufferToFill.buffer->getNumChannels();
    
    // Get pointers to output locations
    float* const channelData1 = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
    float* const channelData2 = numChannels > 1 ? bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample) : nullptr;

    float output = 0.0;
    std::vector<float* const*> curChannel {&channelData1, &channelData2};

    // only do control stuff out of the buffer (at least work with flags so that control doesn't interfere with the scheme calculation)

        
    if (string1->shouldExcite())
        string1->excite();
    if (string2->shouldExcite())
        string2->excite();
    if (string3->shouldExcite())
        string3->excite();
    if (string4->shouldExcite())
        string4->excite();
    if (string5->shouldExcite())
        string5->excite();
    if (string6->shouldExcite())
        string6->excite();

        
    for (int i = 0; i < bufferToFill.numSamples; ++i)
    {
        string1->calculateScheme();
        string1->updateStates();
        string2->calculateScheme();
        string2->updateStates();
        string3->calculateScheme();
        string3->updateStates();
        string4->calculateScheme();
        string4->updateStates();
        string5->calculateScheme();
        string5->updateStates();
        string6->calculateScheme();
        string6->updateStates();
        
        
        output = (string1->getOutput (0.8) 
                    + string2->getOutput (0.8)
                    + string3->getOutput (0.8)
                    + string4->getOutput (0.8)
                    + string5->getOutput (0.8)
                    + string6->getOutput (0.8)
                )/6; // get output at 0.8L of the string
        
        for (int channel = 0; channel < numChannels; ++channel)
            curChannel[channel][0][i] = limit(output);
    }
    
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    //g.fillAll(juce::Colours::black); // or whatever your background is
    // Draw the live OSC point
    g.setColour(juce::Colours::red);
    float radius = 5.0f;

    float visual_mapped_x = juce::jmap(0.5f, 0.0f, 1.0f, 0.0f, (float)getWidth());
    float visual_mapped_y = juce::jmap(posX, -2.2f, 1.0f, 0.0f, (float)getHeight());

    //visual_mapped_x = 400;
    //visual_mapped_y = 300;

    g.drawEllipse(visual_mapped_x-radius, visual_mapped_y-radius, radius * 2, radius * 2, 0.3);
    g.fillEllipse(visual_mapped_x-radius, visual_mapped_y-radius, radius * 2, radius * 2);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    int numStrings = 6;
    int stringHeight = bounds.getHeight() / numStrings;

    if (string1 != nullptr)
        string1->setBounds(bounds.removeFromTop(stringHeight));
    if (string2 != nullptr)
        string2->setBounds(bounds.removeFromTop(stringHeight));
    if (string3 != nullptr)
        string3->setBounds(bounds.removeFromTop(stringHeight));
    if (string4 != nullptr)
        string4->setBounds(bounds.removeFromTop(stringHeight));
    if (string5 != nullptr)
        string5->setBounds(bounds.removeFromTop(stringHeight));
    if (string6 != nullptr)
        string6->setBounds(bounds.removeFromTop(stringHeight));


}



// limiter
double MainComponent::limit (double val)
{
    if (val < -1)
    {
        val = -1;
        return val;
    }
    else if (val > 1)
    {
        val = 1;
        return val;
    }
    return val;
}


void MainComponent::timerCallback()
{
    repaint(); // update the graphics X times a second
}
