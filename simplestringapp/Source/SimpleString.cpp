#include <JuceHeader.h>
#include "SimpleString.h"
#include "Globals.h"
#include <chrono>

//==============================================================================
SimpleString::SimpleString (NamedValueSet& parameters, double k, double freq) : k (k)
{
    //tuning parameter
    pitch = 1.0;
    f0 = freq;
    // Initialise member variables using the parameter set
    L = *parameters.getVarPointer ("L");
    rho = *parameters.getVarPointer ("rho");
    A = *parameters.getVarPointer ("A");
    T = *parameters.getVarPointer ("T");
    E = *parameters.getVarPointer ("E");
    I = *parameters.getVarPointer ("I");
    sigma0 = *parameters.getVarPointer ("sigma0");
    sigma1 = *parameters.getVarPointer ("sigma1");
    
    

    // Calculate wave speed (squared)
    cSq = T / (rho * A);
    
    L = sqrt(cSq)/(freq*2);
    // Calculate stiffness coefficient (squared)
    kappaSq = E * I / (rho * A);

    double stabilityTerm = cSq * k * k + 4.0 * sigma1 * k; // just easier to write down below
    
    h = sqrt (0.5 * (stabilityTerm + sqrt ((stabilityTerm * stabilityTerm) + 16.0 * kappaSq * k * k)));
    N = floor (L / h);
    h = L / N; // recalculate h
    
    lambdaSq = cSq * k * k / (h * h);
    muSq = kappaSq * k * k / (h * h * h * h);
    
    // Initialise vectors
    uStates = std::vector<std::vector<double>> (3,
                                        std::vector<double>(N+1, 0));
    
    /*  Make u pointers point to the first index of the state vectors.
        To use u (and obtain a vector from the state vectors) use indices like u[n][l] where,
             - n = 0 is u^{n+1},
             - n = 1 is u^n, and
             - n = 2 is u^{n-1}.
        Also see calculateScheme()
     */
    
    // Initialise pointer vector
    u.resize (3, nullptr);
    
    // Make set memory addresses to first index of the state vectors.
    for (int i = 0; i < 3; ++i)
        u[i] = &uStates[i][0];
    
    // Coefficients used for damping
    S0 = sigma0 * k;
    S1 = (2.0 * sigma1 * k) / (h * h);
    
    // Scheme coefficients
    B0 = 2.0 - 2.0 * lambdaSq - 6.0 * muSq - 2.0 * S1; // u_l^n
    Bss = 2.0 - 2.0 * lambdaSq - 5.0 * muSq - 2.0 * S1;
    B1 = lambdaSq + 4.0 * muSq + S1;                   // u_{l+-1}^n
    B2 = -muSq;                                        // u_{l+-2}^n
    C0 = -1.0 + S0 + 2.0 * S1;                         // u_l^{n-1}
    C1 = -S1;                                          // u_{l+-1}^{n-1}
    
    Adiv = 1.0 / (1.0 + S0);                           // u_l^{n+1}
    
    // Divide by u_l^{n+1} term
    B0 *= Adiv;
    Bss *= Adiv;
    B1 *= Adiv;
    B2 *= Adiv;
    C0 *= Adiv;
    C1 *= Adiv;
}

SimpleString::~SimpleString()
{
    
}

void SimpleString::paint (juce::Graphics& g)
{
    // clear the background
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // choose your favourite colour
    g.setColour(Colours::cyan);
    
    // draw the state
    g.strokePath(visualiseState (g, 100), PathStrokeType(5.0f));

}

Path SimpleString::visualiseState (Graphics& g, double visualScaling)
{
    // String-boundaries are in the vertical middle of the component
    //double stringBoundaries = getHeight() / 2.0;
    double stringBoundaries = getWidth() / 2.0;

    // initialise path
    Path stringPath;
    
    // start path
    //HORIZoNTAL
    //stringPath.startNewSubPath (0, -u[1][0] * visualScaling + stringBoundaries);
    stringPath.startNewSubPath (-u[1][0] * visualScaling + stringBoundaries, 0);

    //HORIZoNTAL
    //double spacing = getWidth() / static_cast<double>(N);
    //double x = spacing;

    double spacing = getHeight() / static_cast<double>(N);
    double y = spacing;

    
    for (int l = 1; l <= N; l++) // if you don't save the boundaries use l < N
    {
        /*HORIZoNTAL
        // Needs to be -u, because a positive u would visually go down
        float newY = -u[1][l] * visualScaling + stringBoundaries;
        
        // if we get NAN values, make sure that we don't get an exception
        if (std::isnan(newY))
            newY = 0;
        
        stringPath.lineTo (x, newY);
        x += spacing;
*/

        //VERTICAL
        // Needs to be -u, because a positive u would visually go down
        float newX = -u[1][l] * visualScaling + stringBoundaries;

        // if we get NAN values, make sure that we don't get an exception
        if (std::isnan(newX))
            newX = stringBoundaries;
        
        stringPath.lineTo (newX, y);
        y += spacing;


    }
    // if you don't save the boundaries, and add a stringPath.lineTo (x, getWidth()) here to end the statedrawing

    return stringPath;
}

void SimpleString::resized()
{

}

void SimpleString::calculateScheme()
{
    for (int l = 2; l < N-1; ++l) // clamped boundaries
        u[0][l] = B0 * u[1][l] + B1 * (u[1][l + 1] + u[1][l - 1]) + B2 * (u[1][l + 2] + u[1][l - 2])
                + C0 * u[2][l] + C1 * (u[2][l + 1] + u[2][l - 1]);
    /*
    u[0][1] = Bss * u[1][1] + B1 * (u[1][2] + u[1][0]) + B2 * u[1][3]
            + C0 * u[2][1] + C1 * (u[2][2] + u[2][0]);
    u[0][N-1] = Bss * u[1][N-1] + B1 * (u[1][N] + u[1][N-2]) + B2 * (u[1][N-3])
            + C0 * u[2][N-1] + C1 * (u[2][N] + u[2][N-2]);
    */
}

void SimpleString::updateStates()
{
    // Do a pointer-switch. MUCH quicker than copying two entire state vectors every time-step.
    double* uTmp = u[2];
    u[2] = u[1];
    u[1] = u[0];
    u[0] = uTmp;
}

void SimpleString::excite()
{
    //// Arbitrary excitation function (raised cosine) ////
    
    // width (in grid points) of the excitation
    //double width = 10;
    
    // make sure we're not going out of bounds at the left boundary
    int start = std::max (floor((N+1) * excitationLoc) - floor(pluckWidth * 0.5), 1.0);

    for (int l = 0; l < pluckWidth; ++l)
    {
        // make sure we're not going out of bounds at the right boundary (this does 'cut off' the raised cosine)
        if (l+start > (clamped ? N - 2 : N - 1))
            break;
        
        u[1][l+start] += 0.5 * (1 - cos(2.0 * double_Pi * l / (pluckWidth-1.0)));
        u[2][l+start] += 0.5 * (1 - cos(2.0 * double_Pi * l / (pluckWidth-1.0)));
    }
    // Disable the excitation flag to only excite once
    excitationFlag = false;
}

void SimpleString::mouseDown (const MouseEvent& e)
{
    // Get the excitation location as a ratio between the x-location of the mouse-click and the width of the app
    //excitationLoc = e.x / static_cast<double> (getWidth());
    pluckWidth = 15;
    
    double stringPos = e.y / static_cast<double>(getHeight());
    int tune = f0 / pow(1.05946, 4);
    if (stringPos > 0.25 && stringPos <= 0.45)
    {
        tune = f0 / pow(1.05946, 3);
    }
    else if (stringPos > 0.45 && stringPos <= 0.6)
    {
        tune = f0 / pow(1.05946, 2);
    }
    else if (stringPos > 0.6 && stringPos <= 0.8)
    {
        tune = f0 / 1.05946;
    }
    else if (stringPos > 0.8 && stringPos <= 1.0)
    {
        tune = f0;
    }

    SimpleString::tune(tune);
    
    // Activate the excitation flag to be used by the MainComponent to excite the string
    excitationFlag = true;
}

void SimpleString::strum()
{
    auto duration = std::chrono::system_clock::now() - startTime;
    //check if enough time has passed since the last strum was triggered
    if (std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() >= 1200)
    {
        excitationFlag = true;
        startTime = std::chrono::system_clock::now();
    }
}

void SimpleString::tune(double freq)
{
    L = sqrt(cSq) / (freq * 2);
    h = L / N; // recalculate h
    //recalculate lambda
    double tunedLambda = cSq * k * k / (h * h);

    B0 = 2.0 - 2.0 * tunedLambda - 6.0 * muSq - 2.0 * S1; // u_l^n
    Bss = 2.0 - 2.0 * tunedLambda - 5.0 * muSq - 2.0 * S1;
    B1 = tunedLambda + 4.0 * muSq + S1;
    B0 *= Adiv;
    Bss *= Adiv;
    B1 *= Adiv;
}
