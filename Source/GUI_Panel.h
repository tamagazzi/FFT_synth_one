/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  11 Dec 2012 3:31:29pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_GUIPANEL_GUIPANEL_1ECFE922__
#define __JUCER_HEADER_GUIPANEL_GUIPANEL_1ECFE922__

//[Headers]     -- You can add your own extra header files here --
// #include "juce.h"

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class GuiPanel  : public Component,
                   public SliderListener
{
public:
    //==============================================================================
    GuiPanel (Fft_synth_oneAudioProcessor* ptr );
    ~GuiPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	
	Fft_synth_oneAudioProcessor* processor;
	
    //[/UserVariables]

    //==============================================================================
    Slider* slider;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    GuiPanel (const GuiPanel&);
    const GuiPanel& operator= (const GuiPanel&);
};


#endif   // __JUCER_HEADER_GUIPANEL_GUIPANEL_1ECFE922__
