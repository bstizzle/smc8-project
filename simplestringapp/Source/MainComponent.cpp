#include "MainComponent.h"
#include "Globals.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (1000, 1000);

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
    string1 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 103.8);
    //parameters.set ("L", 3);

    //string2 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 110.0);
    string2 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 138.59);
    //parameters.set ("L", 2);

    //string3 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 146.8);
    string3 = std::make_unique<SimpleString>(parameters, 1.0 / sampleRate, 185.0);
    //parameters.set ("L", 1);

    //string4 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 196.0);
    string4 = std::make_unique<SimpleString>(parameters, 1.0 / sampleRate, 246.94);
    //parameters.set ("L", 0.66);

    //string5 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 246.9);
    string5 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 311.13);
    //parameters.set ("L", 0.33);

    //string6 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, 329.6);
    string6 = std::make_unique<SimpleString>(parameters, 1.0 / sampleRate, 415.3);


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

        double outputR = (string1->getOutput(0.8)*1.4
            + string2->getOutput(0.8)*1.2
            + string3->getOutput(0.8)*1.0
            + string4->getOutput(0.8)*0.8
            + string5->getOutput(0.8)*0.6
            + string6->getOutput(0.8)*0.4
            ) / 6; // get output at 0.8L of the string

        double outputL = (string1->getOutput(0.8) * 0.4
            + string2->getOutput(0.8) * 0.6
            + string3->getOutput(0.8) * 0.8
            + string4->getOutput(0.8) * 1.0
            + string5->getOutput(0.8) * 1.2
            + string6->getOutput(0.8) * 1.4
            ) / 6; // get output at 0.8L of the string
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            if (channel == 0)
            {
                curChannel[channel][0][i] = limit(outputL);
            }
            if (channel == 1)
            {
                curChannel[channel][0][i] = limit(outputR);
            }
        }
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
    g.setColour (juce::Colours::orange);

    ymapped_zpos_frets={};
    for (float zpos : zpos_frets)
    {
        float mapped = juce::jmap(zpos, -7.5f, -3.2f,(float)getHeight(),0.0f);
        ymapped_zpos_frets.push_back(mapped);
    }

    for (float zpos:ymapped_zpos_frets) {
        juce::Line<float> line (juce::Point<float> (0.0f, zpos),juce::Point<float> ((float)getWidth(),zpos));
        g.drawLine (line, 2.0f);
    }
    
    //g.fillAll(juce::Colours::black); // or whatever your background is
    // Draw the live OSC points
    
    
    g.setOpacity(0.5);
    float radius = 10.0f;

    for (int id:id_list) {
        g.setColour(bodies_dict.assign_colour(id,8));
        BodyData body_data = bodies_dict.getBody(id);
        float visual_mapped_y = juce::jmap(body_data.posz, -7.5f, -3.2f, (float)getHeight(),0.0f);
        float visual_mapped_x = juce::jmap(body_data.posx, -2.2f, 1.0f, 0.0f, (float)getWidth());
    
        g.drawEllipse(visual_mapped_x-radius, visual_mapped_y-radius, radius * 2, radius * 2, 0.3);
        g.fillEllipse(visual_mapped_x-radius, visual_mapped_y-radius, radius * 2, radius * 2);
        //DBG("ID: " << id << ", velx: " << data.velx << ", posx: " << data.posx);
    }
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    int numStrings = 6;
    int stringZone = bounds.getWidth() / numStrings;

    if (string1 != nullptr)
        string1->setBounds(bounds.removeFromLeft(stringZone));
    if (string2 != nullptr)
        string2->setBounds(bounds.removeFromLeft(stringZone));
    if (string3 != nullptr)
        string3->setBounds(bounds.removeFromLeft(stringZone));
    if (string4 != nullptr)
        string4->setBounds(bounds.removeFromLeft(stringZone));
    if (string5 != nullptr)
        string5->setBounds(bounds.removeFromLeft(stringZone));
    if (string6 != nullptr)
        string6->setBounds(bounds.removeFromLeft(stringZone));


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
