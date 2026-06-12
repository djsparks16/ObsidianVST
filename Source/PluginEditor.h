#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
// ControlPage — a generic page of titled sections containing knobs, combo
// boxes and toggles, all attached to APVTS parameters and laid out in a
// flowing grid.
//==============================================================================
class ControlPage : public juce::Component
{
public:
    explicit ControlPage (juce::AudioProcessorValueTreeState& s) : apvts (s) {}

    struct Control
    {
        std::unique_ptr<juce::Slider>       slider;
        std::unique_ptr<juce::ComboBox>     box;
        std::unique_ptr<juce::ToggleButton> toggle;
        juce::Label label;
        int width = 70;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   sAtt;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> cAtt;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   bAtt;
    };

    struct Section
    {
        juce::String title;
        std::vector<std::unique_ptr<Control>> controls;
        juce::Rectangle<int> bounds;
    };

    Section& section (const juce::String& title);
    void knob   (Section&, const juce::String& paramID, const juce::String& label);
    void combo  (Section&, const juce::String& paramID, const juce::String& label);
    void toggle (Section&, const juce::String& paramID, const juce::String& label);

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& apvts;
    std::vector<std::unique_ptr<Section>> sections;
};

//==============================================================================
// MatrixPage — 8 rows of source -> destination -> amount
//==============================================================================
class MatrixPage : public juce::Component
{
public:
    explicit MatrixPage (juce::AudioProcessorValueTreeState& s);
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Row
    {
        juce::ComboBox src, dst;
        juce::Slider amt;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> srcAtt, dstAtt;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amtAtt;
    };
    std::vector<std::unique_ptr<Row>> rows;
};

//==============================================================================
class ObsidianAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit ObsidianAudioProcessorEditor (ObsidianAudioProcessor&);
    ~ObsidianAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void buildSynthPage();
    void buildFXPage();
    void loadWavetableClicked();
    void savePresetClicked();
    void loadPresetClicked();
    void initPatchClicked();

    ObsidianAudioProcessor& processor;
    juce::LookAndFeel_V4 lnf;

    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };
    ControlPage* synthPage = nullptr; // owned by tabs
    ControlPage* fxPage    = nullptr;
    MatrixPage*  matrixPage = nullptr;

    juce::TextButton loadWtButton  { "Load Wavetable" };
    juce::TextButton savePresetBtn { "Save Preset" };
    juce::TextButton loadPresetBtn { "Load Preset" };
    juce::TextButton initBtn       { "Init" };

    std::unique_ptr<juce::FileChooser> chooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ObsidianAudioProcessorEditor)
};
