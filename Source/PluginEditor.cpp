#include "PluginEditor.h"
#include "Modulation.h"

//==============================================================================
// ControlPage
//==============================================================================
ControlPage::Section& ControlPage::section (const juce::String& title)
{
    sections.push_back (std::make_unique<Section>());
    sections.back()->title = title;
    return *sections.back();
}

void ControlPage::knob (Section& s, const juce::String& paramID, const juce::String& labelText)
{
    auto c = std::make_unique<Control>();
    c->width = 72;

    c->slider = std::make_unique<juce::Slider>();
    c->slider->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    c->slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 15);
    addAndMakeVisible (*c->slider);
    c->sAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, paramID, *c->slider);

    c->label.setText (labelText, juce::dontSendNotification);
    c->label.setJustificationType (juce::Justification::centred);
    c->label.setFont (juce::FontOptions (11.5f));
    addAndMakeVisible (c->label);

    s.controls.push_back (std::move (c));
}

void ControlPage::combo (Section& s, const juce::String& paramID, const juce::String& labelText)
{
    auto c = std::make_unique<Control>();
    c->width = 104;

    c->box = std::make_unique<juce::ComboBox>();
    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (paramID)))
        c->box->addItemList (choice->choices, 1);
    addAndMakeVisible (*c->box);
    c->cAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        apvts, paramID, *c->box);

    c->label.setText (labelText, juce::dontSendNotification);
    c->label.setJustificationType (juce::Justification::centred);
    c->label.setFont (juce::FontOptions (11.5f));
    addAndMakeVisible (c->label);

    s.controls.push_back (std::move (c));
}

void ControlPage::toggle (Section& s, const juce::String& paramID, const juce::String& labelText)
{
    auto c = std::make_unique<Control>();
    c->width = 56;

    c->toggle = std::make_unique<juce::ToggleButton>();
    addAndMakeVisible (*c->toggle);
    c->bAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        apvts, paramID, *c->toggle);

    c->label.setText (labelText, juce::dontSendNotification);
    c->label.setJustificationType (juce::Justification::centred);
    c->label.setFont (juce::FontOptions (11.5f));
    addAndMakeVisible (c->label);

    s.controls.push_back (std::move (c));
}

void ControlPage::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff14151c));

    for (auto& s : sections)
    {
        auto r = s->bounds.toFloat();
        g.setColour (juce::Colour (0xff1d1f2a));
        g.fillRoundedRectangle (r, 8.0f);
        g.setColour (juce::Colour (0xff2c3040));
        g.drawRoundedRectangle (r.reduced (0.5f), 8.0f, 1.0f);

        g.setColour (juce::Colour (0xff8a93b5));
        g.setFont (juce::FontOptions (12.5f, juce::Font::bold));
        auto titleArea = s->bounds;
        g.drawText (s->title, titleArea.removeFromTop (22).reduced (10, 3),
                    juce::Justification::left);
    }
}

void ControlPage::resized()
{
    const int gap = 8, rowH = 142, pad = 8;
    int x = pad, y = pad;

    for (auto& s : sections)
    {
        int w = 16;
        for (auto& c : s->controls)
            w += c->width + 4;

        if (x + w > getWidth() - pad && x > pad)
        {
            x = pad;
            y += rowH + gap;
        }

        s->bounds = { x, y, w, rowH };
        x += w + gap;

        auto inner = s->bounds.reduced (8).withTrimmedTop (20);
        int cx = inner.getX();

        for (auto& c : s->controls)
        {
            juce::Rectangle<int> cell (cx, inner.getY(), c->width, inner.getHeight());
            cx += c->width + 4;

            c->label.setBounds (cell.removeFromBottom (15));

            if (c->slider != nullptr)
                c->slider->setBounds (cell);
            else if (c->box != nullptr)
                c->box->setBounds (cell.withSizeKeepingCentre (c->width - 4, 24));
            else if (c->toggle != nullptr)
                c->toggle->setBounds (cell.withSizeKeepingCentre (24, 24));
        }
    }
}

//==============================================================================
// MatrixPage
//==============================================================================
MatrixPage::MatrixPage (juce::AudioProcessorValueTreeState& apvts)
{
    for (int s = 1; s <= Mod::numSlots; ++s)
    {
        auto row = std::make_unique<Row>();
        const auto n = juce::String (s);

        row->src.addItemList (Mod::sourceNames, 1);
        row->dst.addItemList (Mod::destNames, 1);
        addAndMakeVisible (row->src);
        addAndMakeVisible (row->dst);

        row->amt.setSliderStyle (juce::Slider::LinearHorizontal);
        row->amt.setTextBoxStyle (juce::Slider::TextBoxRight, false, 56, 18);
        addAndMakeVisible (row->amt);

        row->srcAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
            apvts, "mod" + n + "Src", row->src);
        row->dstAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
            apvts, "mod" + n + "Dst", row->dst);
        row->amtAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, "mod" + n + "Amt", row->amt);

        rows.push_back (std::move (row));
    }
}

void MatrixPage::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff14151c));

    g.setColour (juce::Colour (0xff8a93b5));
    g.setFont (juce::FontOptions (12.5f, juce::Font::bold));
    g.drawText ("SOURCE", 16, 8, 150, 20, juce::Justification::left);
    g.drawText ("DESTINATION", 186, 8, 150, 20, juce::Justification::left);
    g.drawText ("AMOUNT", 356, 8, 150, 20, juce::Justification::left);
}

void MatrixPage::resized()
{
    int y = 34;
    for (auto& row : rows)
    {
        row->src.setBounds (16, y, 150, 26);
        row->dst.setBounds (186, y, 150, 26);
        row->amt.setBounds (356, y, juce::jmax (200, getWidth() - 380), 26);
        y += 36;
    }
}

//==============================================================================
// Editor
//==============================================================================
ObsidianAudioProcessorEditor::ObsidianAudioProcessorEditor (ObsidianAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    lnf.setColourScheme (juce::LookAndFeel_V4::getMidnightColourScheme());
    lnf.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff34d2eb));
    lnf.setColour (juce::Slider::thumbColourId,            juce::Colour (0xff34d2eb));
    setLookAndFeel (&lnf);

    synthPage  = new ControlPage (processor.apvts);
    fxPage     = new ControlPage (processor.apvts);
    matrixPage = new MatrixPage (processor.apvts);

    buildSynthPage();
    buildFXPage();

    tabs.addTab ("SYNTH",  juce::Colour (0xff14151c), synthPage,  true);
    tabs.addTab ("FX",     juce::Colour (0xff14151c), fxPage,     true);
    tabs.addTab ("MATRIX", juce::Colour (0xff14151c), matrixPage, true);
    addAndMakeVisible (tabs);

    addAndMakeVisible (loadWtButton);
    addAndMakeVisible (savePresetBtn);
    addAndMakeVisible (loadPresetBtn);
    addAndMakeVisible (initBtn);

    loadWtButton.onClick  = [this] { loadWavetableClicked(); };
    savePresetBtn.onClick = [this] { savePresetClicked(); };
    loadPresetBtn.onClick = [this] { loadPresetClicked(); };
    initBtn.onClick       = [this] { initPatchClicked(); };

    setSize (1100, 660);
    setResizable (true, true);
    setResizeLimits (900, 560, 1800, 1100);
}

ObsidianAudioProcessorEditor::~ObsidianAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void ObsidianAudioProcessorEditor::buildSynthPage()
{
    auto& page = *synthPage;

    auto& a = page.section ("OSC A");
    page.combo (a, "oscATable", "Table");
    page.knob  (a, "oscAMorph", "Morph");
    page.combo (a, "oscAWarpMode", "Warp");
    page.knob  (a, "oscAWarpAmt", "Warp Amt");
    page.knob  (a, "oscASemi", "Semi");
    page.knob  (a, "oscAFine", "Fine");
    page.knob  (a, "oscALevel", "Level");

    auto& b = page.section ("OSC B");
    page.combo (b, "oscBTable", "Table");
    page.knob  (b, "oscBMorph", "Morph");
    page.combo (b, "oscBWarpMode", "Warp");
    page.knob  (b, "oscBWarpAmt", "Warp Amt");
    page.knob  (b, "oscBSemi", "Semi");
    page.knob  (b, "oscBFine", "Fine");
    page.knob  (b, "oscBLevel", "Level");

    auto& sn = page.section ("SUB / NOISE");
    page.knob  (sn, "subLevel", "Sub");
    page.combo (sn, "subOct", "Octave");
    page.knob  (sn, "noiseLevel", "Noise");

    auto& u = page.section ("UNISON / VOICE");
    page.knob (u, "uniCount", "Voices");
    page.knob (u, "uniDetune", "Detune");
    page.knob (u, "uniWidth", "Width");
    page.knob (u, "glideTime", "Glide");
    page.knob (u, "bendRange", "Bend");

    auto& f = page.section ("FILTER");
    page.combo (f, "fltModel", "Model");
    page.knob  (f, "cutoff", "Cutoff");
    page.knob  (f, "reso", "Reso");
    page.knob  (f, "fltDrive", "Drive");
    page.knob  (f, "fltEnvAmt", "Env2 Amt");

    auto& e1 = page.section ("ENV 1 (AMP)");
    page.knob (e1, "ampA", "A");
    page.knob (e1, "ampD", "D");
    page.knob (e1, "ampS", "S");
    page.knob (e1, "ampR", "R");

    auto& e2 = page.section ("ENV 2");
    page.knob (e2, "env2A", "A");
    page.knob (e2, "env2D", "D");
    page.knob (e2, "env2S", "S");
    page.knob (e2, "env2R", "R");

    auto& l1 = page.section ("LFO 1");
    page.combo  (l1, "lfo1Shape", "Shape");
    page.knob   (l1, "lfo1Rate", "Rate");
    page.toggle (l1, "lfo1Sync", "Sync");
    page.combo  (l1, "lfo1Div", "Div");
    page.knob   (l1, "lfo1Cut", "> Cutoff");

    auto& l2 = page.section ("LFO 2");
    page.combo  (l2, "lfo2Shape", "Shape");
    page.knob   (l2, "lfo2Rate", "Rate");
    page.toggle (l2, "lfo2Sync", "Sync");
    page.combo  (l2, "lfo2Div", "Div");

    auto& out = page.section ("OUT");
    page.knob (out, "master", "Master");
}

void ObsidianAudioProcessorEditor::buildFXPage()
{
    auto& page = *fxPage;

    auto& d = page.section ("DISTORTION");
    page.toggle (d, "fxDistOn", "On");
    page.knob   (d, "fxDistDrive", "Drive");
    page.knob   (d, "fxDistMix", "Mix");

    auto& c = page.section ("CHORUS");
    page.toggle (c, "fxChorusOn", "On");
    page.knob   (c, "fxChorusRate", "Rate");
    page.knob   (c, "fxChorusDepth", "Depth");
    page.knob   (c, "fxChorusMix", "Mix");

    auto& ph = page.section ("PHASER");
    page.toggle (ph, "fxPhaserOn", "On");
    page.knob   (ph, "fxPhaserRate", "Rate");
    page.knob   (ph, "fxPhaserDepth", "Depth");
    page.knob   (ph, "fxPhaserMix", "Mix");

    auto& dl = page.section ("DELAY");
    page.toggle (dl, "fxDelayOn", "On");
    page.knob   (dl, "fxDelayTime", "Time ms");
    page.knob   (dl, "fxDelayFb", "Feedback");
    page.knob   (dl, "fxDelayMix", "Mix");

    auto& rv = page.section ("REVERB");
    page.toggle (rv, "fxRevOn", "On");
    page.knob   (rv, "fxRevSize", "Size");
    page.knob   (rv, "fxRevDamp", "Damp");
    page.knob   (rv, "fxRevWidth", "Width");
    page.knob   (rv, "fxRevMix", "Mix");

    auto& cp = page.section ("COMPRESSOR");
    page.toggle (cp, "fxCompOn", "On");
    page.knob   (cp, "fxCompThresh", "Thresh");
    page.knob   (cp, "fxCompRatio", "Ratio");
}

//==============================================================================
void ObsidianAudioProcessorEditor::loadWavetableClicked()
{
    chooser = std::make_unique<juce::FileChooser> ("Load wavetable (WAV, 2048-sample frames)",
                                                   juce::File(), "*.wav");
    chooser->launchAsync (juce::FileBrowserComponent::openMode
                            | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& fc)
        {
            const auto file = fc.getResult();
            if (file.existsAsFile() && ! processor.loadUserWavetable (file))
                juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::WarningIcon,
                                                        "Obsidian", "Couldn't read that file.");
        });
}

void ObsidianAudioProcessorEditor::savePresetClicked()
{
    chooser = std::make_unique<juce::FileChooser> ("Save preset", juce::File(), "*.obsn");
    chooser->launchAsync (juce::FileBrowserComponent::saveMode
                            | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file == juce::File())
                return;
            if (auto xml = processor.apvts.copyState().createXml())
                xml->writeTo (file.withFileExtension ("obsn"));
        });
}

void ObsidianAudioProcessorEditor::loadPresetClicked()
{
    chooser = std::make_unique<juce::FileChooser> ("Load preset", juce::File(), "*.obsn");
    chooser->launchAsync (juce::FileBrowserComponent::openMode
                            | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& fc)
        {
            const auto file = fc.getResult();
            if (! file.existsAsFile())
                return;
            if (auto xml = juce::parseXML (file))
                processor.apvts.replaceState (juce::ValueTree::fromXml (*xml));
        });
}

void ObsidianAudioProcessorEditor::initPatchClicked()
{
    for (auto* param : processor.getParameters())
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (param))
        {
            ranged->beginChangeGesture();
            ranged->setValueNotifyingHost (ranged->getDefaultValue());
            ranged->endChangeGesture();
        }
}

//==============================================================================
void ObsidianAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0f1016));

    g.setColour (juce::Colour (0xff34d2eb));
    g.setFont (juce::FontOptions (24.0f, juce::Font::bold));
    g.drawText ("OBSIDIAN", 16, 8, 260, 30, juce::Justification::left);

    g.setColour (juce::Colours::grey);
    g.setFont (juce::FontOptions (12.0f));
    g.drawText ("wavetable synthesizer", 158, 15, 200, 20, juce::Justification::left);
}

void ObsidianAudioProcessorEditor::resized()
{
    auto top = getLocalBounds().removeFromTop (44).reduced (8);
    top.removeFromLeft (330); // title space

    initBtn.setBounds       (top.removeFromRight (60).reduced (2));
    loadPresetBtn.setBounds (top.removeFromRight (100).reduced (2));
    savePresetBtn.setBounds (top.removeFromRight (100).reduced (2));
    loadWtButton.setBounds  (top.removeFromRight (130).reduced (2));

    tabs.setBounds (getLocalBounds().withTrimmedTop (44).reduced (4));
}
