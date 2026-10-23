#pragma once
#include "Core/Suite.h"
#include <JuceHeader.h>
namespace da::ui {
inline const juce::Colour ink{0xff080a0b}, crimson{0xffed173b}, silver{0xffd0d2d2},
    muted{0xff92999b};
class HardwareLook final : public juce::LookAndFeel_V4 {
  public:
    HardwareLook();
    juce::Font font(float size, bool bold = false) const;
    juce::Typeface::Ptr getTypefaceForFont(const juce::Font &) override;
    void drawRotarySlider(juce::Graphics &, int, int, int, int, float, float, float,
                          juce::Slider &) override;
    void drawButtonBackground(juce::Graphics &, juce::Button &, const juce::Colour &, bool,
                              bool) override;
    void drawButtonText(juce::Graphics &, juce::TextButton &, bool, bool) override;
    void drawComboBox(juce::Graphics &, int, int, bool, int, int, int, int,
                      juce::ComboBox &) override;
    juce::Font getComboBoxFont(juce::ComboBox &) override;
    juce::Font getTextButtonFont(juce::TextButton &, int) override;
    juce::Font getLabelFont(juce::Label &) override;

  private:
    juce::Typeface::Ptr regular, semibold;
};
struct Layout {
    juce::var data;
    explicit Layout(const juce::String &name);
    juce::Rectangle<int> area(const char *key) const;
    juce::String title(const char *key) const { return data[key].toString(); }
};
juce::Rectangle<int> rectangle(const juce::var &);
void panel(juce::Graphics &, juce::Rectangle<float>, bool accented = false);
void label(juce::Graphics &, const HardwareLook &, juce::String, juce::Rectangle<float>,
           float size = 14, juce::Colour colour = muted,
           juce::Justification align = juce::Justification::centredLeft);
// Procedurally painted hardware; no screenshot is used as the runtime interface.
class Chassis final : public juce::Component {
  public:
    Chassis(const Layout &, const HardwareLook &);
    void paint(juce::Graphics &) override;
    std::function<void(juce::Graphics &)> overlay;

  private:
    const Layout &layout;
    const HardwareLook &look;
    juce::Image weave, logo;
};
class ParameterControl final : public juce::Component {
  public:
    ParameterControl(juce::AudioProcessorValueTreeState &, const da::Parameter &, const juce::var &,
                     HardwareLook &, std::function<void()> learn);
    void resized() override;
    void paint(juce::Graphics &) override;
    juce::Slider slider;

  private:
    da::Parameter metadata;
    juce::String caption;
    HardwareLook &look;
    juce::TextButton learnButton{"LEARN"};
    juce::ComboBox choiceMenu;
    bool isMenu=false;
    std::vector<std::unique_ptr<juce::TextButton>> choices;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};
} // namespace da::ui
