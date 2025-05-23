#include "MainComponent.h"
#include "Globals.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (1000, 800);

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
    parameters.set ("T", 600);
    parameters.set ("E", 2e11);
    parameters.set ("I", r * r * r * r * double_Pi * 0.25);
    parameters.set ("sigma0", 2);
    parameters.set ("sigma1", 0.002);
    

    //// Initialise an instance of the SimpleString class ////
    f1 = 101.6; // hand-tuned for E string 4th fret, 'real' freq is 103.8
    string1 = std::make_unique<SimpleString> (parameters, 1.0 / sampleRate, f1);
    //parameters.set ("L", 3);

    f2 = 134.9; // hand-tuned for A string 4th fret, 'real' freq is 138.59
    string2 = std::make_unique<SimpleString>(parameters, 1.0 / sampleRate, f2);
    //parameters.set ("L", 2);

    f3 = 179.3; // hand-tuned for D string 4th fret, 'real' freq is 185.0
    string3 = std::make_unique<SimpleString>(parameters, 1.0 / sampleRate, f3);
    //parameters.set ("L", 1);

    f4 = 237.3; // hand-tuned for G string 4th fret, 'real' freq is 246.94
    string4 = std::make_unique<SimpleString>(parameters, 1.0 / sampleRate, f4);
    //parameters.set ("L", 0.66);

    f5 = 295.7; // hand-tuned for B string 4th fret, 'real' freq is 311.13
    string5 = std::make_unique<SimpleString>(parameters, 1.0 / sampleRate, f5);
    //parameters.set ("L", 0.33);

    f6 = 393.1; // hand-tuned for E string 4th fret, 'real' freq is 415.3
    string6 = std::make_unique<SimpleString>(parameters, 1.0 / sampleRate, f6);

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
        
        output = (string1->getOutput (0.73) 
                    + string2->getOutput (0.73)
                    + string3->getOutput (0.73)
                    + string4->getOutput (0.73)
                    + string5->getOutput (0.73)
                    + string6->getOutput (0.73)
                )/6; // get output at 0.73L of the string

        double outputR = (string1->getOutput(0.73)*1.6
            + string2->getOutput(0.73)*1.3
            + string3->getOutput(0.73)*1.0
            + string4->getOutput(0.73)*0.7
            + string5->getOutput(0.73)*0.4
            + string6->getOutput(0.73)*0.1
            ) / 6; // get output at 0.73L of the string

        double outputL = (string1->getOutput(0.73) * 0.1
            + string2->getOutput(0.73) * 0.4
            + string3->getOutput(0.73) * 0.7
            + string4->getOutput(0.73) * 1.0
            + string5->getOutput(0.73) * 1.3
            + string6->getOutput(0.73) * 1.6
            ) / 6; // get output at 0.73L of the string
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            // use outputL and R below for panned stereo sound, use regular output for mono
            if (channel == 0)
            {
                curChannel[channel][0][i] = limit(outputL);
                //curChannel[channel][0][i] = limit(output);
            }
            if (channel == 1)
            {
                curChannel[channel][0][i] = limit(outputR);
                //curChannel[channel][0][i] = limit(output);
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
    std::vector<float> ymapped_zpos_frets;
    for (float zpos : zpos_frets)
    {
        float mapped = juce::jmap(zpos, -7.5f, -3.2f, (float)getHeight(), 0.0f);
        ymapped_zpos_frets.push_back(mapped);
    }

    // Controlla che ci siano almeno due linee
    if (ymapped_zpos_frets.size() >= 2)
    {
        // Disegna la "nuova" prima linea (bianca)
        g.setColour (juce::Colours::white);
        float yPosWhite = ymapped_zpos_frets[1];
        juce::Line<float> firstLine (juce::Point<float> (0.0f, yPosWhite), juce::Point<float> ((float)getWidth(), yPosWhite));
        g.drawLine (firstLine, 14.0f);

        // Disegna le linee restanti (arancioni)
        g.setColour (juce::Colours::orange);
        for (size_t i = 2; i < ymapped_zpos_frets.size(); ++i)
        {
            float yPos = ymapped_zpos_frets[i];
            juce::Line<float> line (juce::Point<float> (0.0f, yPos), juce::Point<float> ((float)getWidth(), yPos));
            g.drawLine (line, 5.0f);
        }
    }

    g.setColour(juce::Colour(32,32,32));
    juce::Rectangle<int> nut(0, 0, (int)getWidth(), (float)getHeight()*0.25);
    g.drawRect(nut, 2);  // outline
    // or
    g.fillRect(nut);  
    
    g.setOpacity(0.5);
    float radius = 10.0f;

    for (int id:id_list) {
        g.setColour(bodies_dict.assign_colour(id,8));
        BodyData body_data = bodies_dict.getBody(id);
        float visual_mapped_y = juce::jmap(body_data.posz, -7.5f, -3.2f, (float)getHeight(),0.0f);
        float visual_mapped_x = juce::jmap(body_data.posx, -2.2f, 1.3f, 0.0f, (float)getWidth());
    
        g.drawEllipse(visual_mapped_x-radius, visual_mapped_y-radius, radius * 2, radius * 2, 0.3);
        g.fillEllipse(visual_mapped_x-radius, visual_mapped_y-radius, radius * 2, radius * 2);
        //DBG("ID: " << id << ", velx: " << data.velx << ", posx: " << data.posx);
    }
    
    //g.setColour(juce::Colours::white); // Cambia il colore qui (es. blu)
    //g.drawRect(0.0, 0.0, (float)getWidth(), ymapped_zpos_frets, 2); // The last parameter is the thickness
    //g.fillRect(0.0, 0.0, (float)getWidth(), ymapped_zpos_frets, 2); // The last parameter is the thickness
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
