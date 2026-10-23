#include "HardwareLook.h"
#include <DAAssets.h>
namespace da::ui {
namespace {
juce::Path bevel(juce::Rectangle<float> r, float cut = 12.f) {
    juce::Path p;
    auto x = r.getX(), y = r.getY(), w = r.getWidth(), h = r.getHeight();
    p.startNewSubPath(x + cut, y);
    p.lineTo(x + w - cut, y);
    p.lineTo(x + w, y + cut);
    p.lineTo(x + w, y + h - cut);
    p.lineTo(x + w - cut, y + h);
    p.lineTo(x + cut, y + h);
    p.lineTo(x, y + h - cut);
    p.lineTo(x, y + cut);
    p.closeSubPath();
    return p;
}
} // namespace
HardwareLook::HardwareLook() {
    regular = juce::Typeface::createSystemTypefaceFor(DAAssets::RajdhaniMedium_ttf,
                                                      DAAssets::RajdhaniMedium_ttfSize);
    semibold = juce::Typeface::createSystemTypefaceFor(DAAssets::RajdhaniSemiBold_ttf,
                                                       DAAssets::RajdhaniSemiBold_ttfSize);
    setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff0a0c0e));
    setColour(juce::TextEditor::textColourId, silver);
    setColour(juce::TextEditor::outlineColourId, juce::Colour(0xff303638));
    setColour(juce::TextEditor::focusedOutlineColourId, crimson.withAlpha(.7f));
    setColour(juce::TextEditor::highlightColourId, crimson.withAlpha(.35f));
    setColour(juce::ComboBox::textColourId, silver);
    setColour(juce::ComboBox::backgroundColourId, ink);
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff303638));
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff101315));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff6e1021));
    setColour(juce::Slider::textBoxTextColourId, silver);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
juce::Font HardwareLook::font(float size, bool bold) const {
    return juce::Font(juce::FontOptions(bold ? semibold : regular).withHeight(size));
}
juce::Typeface::Ptr HardwareLook::getTypefaceForFont(const juce::Font &f) {
    return f.isBold() ? semibold : regular;
}
juce::Font HardwareLook::getComboBoxFont(juce::ComboBox &) { return font(14); }
juce::Font HardwareLook::getTextButtonFont(juce::TextButton &, int) { return font(13, true); }
juce::Font HardwareLook::getLabelFont(juce::Label &) { return font(17); }
void label(juce::Graphics &g, const HardwareLook &look, juce::String text,
           juce::Rectangle<float> box, float size, juce::Colour colour, juce::Justification align) {
    g.setFont(look.font(size));
    g.setColour(colour);
    g.drawText(text, box, align, true);
}
void HardwareLook::drawRotarySlider(juce::Graphics &g, int x, int y, int w, int h, float position,
                                    float start, float end, juce::Slider &slider) {
    float diameter = std::max(16.f, float(std::min(w, h)) - (std::min(w, h) < 90 ? 4.f : 16.f)),
          radius = diameter * .5f;
    juce::Point<float> c{float(x) + w * .5f, float(y) + h * .5f};
    auto circle = juce::Rectangle<float>(diameter, diameter).withCentre(c);
    juce::DropShadow(juce::Colours::black.withAlpha(.95f), 9, {1, 5})
        .drawForPath(g, [](juce::Rectangle<float> r) {
            juce::Path p;
            p.addEllipse(r);
            return p;
        }(circle));
    // Machined concentric housing, luminous recessed ring and a conical face.
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff555b5d), c.x, c.y - radius,
                                           juce::Colour(0xff040506), c.x, c.y + radius, false));
    g.fillEllipse(circle);
    g.setColour(juce::Colour(0xff020304));
    g.fillEllipse(circle.reduced(1.5f));
    g.setColour(juce::Colour(0xff242a2c));
    g.drawEllipse(circle.reduced(4.f), 1.f);
    float ring = radius - 7, angle = start + position * (end - start);
    juce::Path base;
    base.addCentredArc(c.x, c.y, ring, ring, 0, start, end, true);
    g.setColour(juce::Colour(0xff414749));
    g.strokePath(base, juce::PathStrokeType(1.f));
    juce::Path active;
    active.addCentredArc(c.x, c.y, ring, ring, 0, start, angle, true);
    for (int layer = 3; layer > 0; --layer) {
        g.setColour(crimson.withAlpha(.045f * float(4 - layer)));
        g.strokePath(active, juce::PathStrokeType(float(2 + layer * 3)));
    }
    g.setColour(slider.isEnabled() ? crimson : muted);
    g.strokePath(active, juce::PathStrokeType(2.6f, juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));
    auto face = circle.reduced(12.f);
    float faceRadius = face.getWidth() * .5f;
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff3b4143), c.x - faceRadius * .6f,
                                           c.y - faceRadius, juce::Colour(0xff050708),
                                           c.x + faceRadius * .6f, c.y + faceRadius, false));
    g.fillEllipse(face);
    // Fine fan-shaped brushed highlights, clipped by triangular faces within the dial.
    for (int j = 0; j < 56; ++j) {
        float a = float(j) * juce::MathConstants<float>::twoPi / 56.f;
        float light = .5f + .5f * std::sin(a * 3.f + .7f);
        g.setColour(juce::Colours::white.withAlpha(.008f + light * .018f));
        g.drawLine(c.x, c.y, c.x + std::sin(a) * (faceRadius - 1),
                   c.y - std::cos(a) * (faceRadius - 1), .6f);
    }
    g.setColour(juce::Colour(0xff3a4042));
    g.drawEllipse(face, .7f);
    g.setColour(juce::Colours::black.withAlpha(.75f));
    g.drawEllipse(face.reduced(1.5f), 1.f);
    for (int tick = 0; tick <= 10; ++tick) {
        float a = start + (end - start) * float(tick) / 10.f;
        auto p = c.getPointOnCircumference(radius - 1, a);
        auto q = c.getPointOnCircumference(radius - 3, a);
        g.setColour(tick % 5 == 0 ? muted : juce::Colour(0xff454b4d));
        g.drawLine({p, q}, .7f);
    }
    auto p = c.getPointOnCircumference(faceRadius - 5, angle),
         q = c.getPointOnCircumference(faceRadius - 14, angle);
    g.setColour(silver);
    g.drawLine({p, q}, 1.4f);
}
void HardwareLook::drawButtonBackground(juce::Graphics &g, juce::Button &b, const juce::Colour &,
                                        bool hover, bool down) {
    if (bool(b.getProperties()["round"])) {
        auto r = b.getLocalBounds().toFloat().reduced(4).withSizeKeepingCentre(98, 98);
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xff383e40), r.getX(), r.getY(), ink,
                                               r.getRight(), r.getBottom(), false));
        g.fillEllipse(r);
        g.setColour(crimson.withAlpha(.10f));
        g.drawEllipse(r.reduced(5), 12);
        g.setColour(crimson);
        g.drawEllipse(r.reduced(9), 2);
        g.setColour(ink);
        g.fillEllipse(r.reduced(15));
        return;
    }
    auto r = b.getLocalBounds().toFloat().reduced(1);
    bool accent = b.getToggleState() || bool(b.getProperties()["accent"]);
    auto path = bevel(r, 5);
    g.setGradientFill(
        juce::ColourGradient(accent ? juce::Colour(0xff361018) : juce::Colour(0xff1c2123), 0, 0,
                             juce::Colour(0xff080a0b), 0, r.getBottom(), false));
    g.fillPath(path);
    g.setColour(accent ? crimson.withAlpha(hover || down ? 1.f : .8f)
                       : juce::Colour(hover ? 0xff757c7e : 0xff3c4345));
    g.strokePath(path, juce::PathStrokeType(accent ? 1.2f : .8f));
    g.setColour(juce::Colours::white.withAlpha(.07f));
    g.drawLine(r.getX() + 7, r.getY() + 1, r.getRight() - 7, r.getY() + 1, .7f);
}
void HardwareLook::drawButtonText(juce::Graphics &g, juce::TextButton &b, bool, bool) {
    float size = b.getHeight() < 23 ? 11.f : 13.f;
    label(g, *this, b.getButtonText(), b.getLocalBounds().toFloat().reduced(3, 0), size,
          b.isEnabled()
              ? (b.getToggleState() || bool(b.getProperties()["accent"]) ? crimson.brighter(.25f)
                                                                         : silver)
              : muted.withAlpha(.4f),
          juce::Justification::centred);
}
void HardwareLook::drawComboBox(juce::Graphics &g, int w, int h, bool, int, int, int, int,
                                juce::ComboBox &) {
    panel(g, {0, 0, float(w), float(h)});
    g.setColour(muted);
    juce::Path p;
    p.addTriangle(float(w - 18), h * .43f, float(w - 10), h * .43f, float(w - 14), h * .62f);
    g.fillPath(p);
}
juce::Rectangle<int> rectangle(const juce::var &v) {
    auto *a = v.getArray();
    if (!a || a->size() != 4)
        return {};
    return {int((*a)[0]), int((*a)[1]), int((*a)[2]), int((*a)[3])};
}
Layout::Layout(const juce::String &name) {
    auto parsed = juce::JSON::parse(
        juce::String::fromUTF8(DAAssets::Layouts_json, DAAssets::Layouts_jsonSize));
    if (auto *items = parsed.getArray())
        for (auto &item : *items)
            if (item["name"].toString() == name) {
                data = item;
                break;
            }
    jassert(!data.isVoid());
}
juce::Rectangle<int> Layout::area(const char *key) const { return rectangle(data[key]); }
void panel(juce::Graphics &g, juce::Rectangle<float> r, bool accent) {
    auto path = bevel(r, 10);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff181c1e), r.getX(), r.getY(),
                                           juce::Colour(0xff060809), r.getRight(), r.getBottom(),
                                           false));
    g.fillPath(path);
    g.setColour(juce::Colour(0xff353c3e));
    g.strokePath(path, juce::PathStrokeType(1));
    g.setColour(juce::Colours::black);
    g.strokePath(bevel(r.reduced(3), 8), juce::PathStrokeType(2));
    g.setColour(juce::Colours::white.withAlpha(.055f));
    g.drawLine(r.getX() + 12, r.getY() + 2, r.getRight() - 12, r.getY() + 2);
    if (accent) {
        g.setColour(crimson.withAlpha(.65f));
        g.drawLine(r.getX() + 1, r.getY() + 18, r.getX() + 1, r.getBottom() - 18, 1.5f);
    }
}
Chassis::Chassis(const Layout &l, const HardwareLook &f) : layout(l), look(f) {
    setInterceptsMouseClicks(false, false);
    weave = juce::Image(juce::Image::RGB, 64, 64, true);
    juce::Graphics g(weave);
    g.fillAll(juce::Colour(0xff0b0d0e));
    juce::Random random(90127);
    for (int y = 0; y < 64; ++y)
        for (int x = 0; x < 64; ++x) {
            int diagonal = (x + y) % 8;
            float shade = diagonal < 4 ? .022f : .008f;
            shade += random.nextFloat() * .006f;
            g.setColour(juce::Colours::white.withAlpha(shade));
            g.fillRect(x, y, 1, 1);
        }
    logo = juce::ImageFileFormat::loadFrom(DAAssets::DAmark_png, DAAssets::DAmark_pngSize);
}
void Chassis::paint(juce::Graphics &g) {
    g.fillAll(juce::Colour(0xff040506));
    auto outer = bevel({20, 20, 1160, 1080}, 28);
    g.setTiledImageFill(weave, 0, 0, 1);
    g.fillPath(outer);
    g.setColour(juce::Colour(0xff50575a));
    g.strokePath(outer, juce::PathStrokeType(1));
    g.setColour(juce::Colour(0xff020303));
    g.strokePath(bevel({26, 26, 1148, 1068}, 24), juce::PathStrokeType(4));
    g.setColour(crimson.withAlpha(.65f));
    g.strokePath(bevel({24, 24, 1152, 1072}, 26), juce::PathStrokeType(.8f));
    // The angular top rail and footer follow the reference's machined enclosure.
    juce::Path rail;
    rail.startNewSubPath(190, 26);
    rail.lineTo(280, 126);
    rail.lineTo(920, 126);
    rail.lineTo(1010, 26);
    g.setColour(juce::Colour(0xff010203));
    g.strokePath(rail, juce::PathStrokeType(9));
    g.setColour(juce::Colour(0xff4b5254));
    g.strokePath(rail, juce::PathStrokeType(1));
    g.setColour(crimson.withAlpha(.55f));
    g.drawLine(207, 28, 291, 120, .8f);
    g.drawLine(993, 28, 909, 120, .8f);
    g.setColour(juce::Colour(0xff484f51));
    g.drawLine(51, 134, 1149, 134);
    g.setColour(juce::Colours::black);
    g.drawLine(51, 137, 1149, 137, 3);
    g.setOpacity(.76f);
    g.drawImageWithin(logo, 74, 55, 89, 40, juce::RectanglePlacement::centred, false);
    g.setOpacity(1);
    label(g, look, "P L U G I N S", {71, 96, 100, 15}, 11, silver, juce::Justification::centred);
    auto name = layout.title("name").toUpperCase();
    float nameX = 600 - float(name.length()) * 15;
    for (int i = 0; i < name.length(); ++i)
        label(g, look, juce::String::charToString(name[i]), {nameX + i * 30, 43, 30, 43}, 36,
              i == 0 ? crimson : silver, juce::Justification::centred);
    label(g, look, layout.title("subtitle"), {340, 89, 520, 20}, 13, muted,
          juce::Justification::centred);
    label(g, look, "D E V  /  0.3", {1020, 67, 113, 24}, 12, muted,
          juce::Justification::centredRight);
    panel(g, {45, 152, 1110, 236});
    if (auto *sections = layout.data["sections"].getArray())
        for (auto &section : *sections) {
            auto r = rectangle(section["rect"]).toFloat();
            panel(g, r, true);
            label(g, look, section["title"], r.reduced(15).withHeight(24), 14, silver);
        }
    auto learning = layout.area("learningRect").toFloat();
    panel(g, learning, true);
    label(g, look, "L E A R N   M Y   S O U N D",
          {learning.getX() + 20, learning.getY() + 12, 430, 22}, 16, silver);
    auto advanced = layout.area("advancedRect").toFloat();
    panel(g, advanced);
    label(g, look, layout.title("advancedTitle"),
          {advanced.getX() + 20, advanced.getY() + 12, 450, 22}, 16, silver);
    panel(g, {1038, learning.getY(), 116, learning.getHeight()});
    g.setColour(juce::Colour(0xff343b3d));
    g.drawLine(48, 1041, 1152, 1041);
    g.setOpacity(.42f);
    g.drawImageWithin(logo, 71, 1062, 48, 21, juce::RectanglePlacement::centred, false);
    g.setOpacity(1);
    label(g, look, layout.title("name").toUpperCase(), {146, 1060, 145, 23}, 15, muted);
    label(g, look, "LEARN THE SOUND. RECREATE THE FEEL.", {650, 1060, 475, 23}, 12, muted,
          juce::Justification::centredRight);
    if (overlay)
        overlay(g);
}
ParameterControl::ParameterControl(juce::AudioProcessorValueTreeState &state,
                                   const da::Parameter &m, const juce::var &spec, HardwareLook &f,
                                   std::function<void()> action)
    : metadata(m), caption(spec["label"].toString()), look(f) {
    setComponentID(m.id);
    setTitle(m.module + " / " + m.name);
    setDescription("Parameter with independent audio learning");
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setRotaryParameters(juce::MathConstants<float>::pi * 1.22f,
                               juce::MathConstants<float>::pi * 2.78f, true);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 105, 22);
    bool percent =
        (m.unit == "%" || m.id == "mix" || m.id == "width" || m.id == "density" ||
         m.id == "diffusion" || m.id == "modulation" || m.id == "sensitivity" || m.id == "depth" ||
         m.id == "focus" || m.id == "delayMix" || m.id == "reverbMix");
    slider.textFromValueFunction = [m, percent](double v) {
        if (percent)
            return juce::String(int(std::round(v * 100))) + "%";
        if (m.unit == "Hz")
            return v >= 1000 ? juce::String(v / 1000, 1) + " kHz" : juce::String(int(v)) + " Hz";
        if (m.unit == "ms")
            return juce::String(v, v < 10 ? 1 : 0) + " ms";
        if (m.unit == ":1")
            return juce::String(v, 1) + ":1";
        return juce::String(v, m.unit == "s" || m.unit == "dB" ? 1 : 2) + " " +
               juce::String(m.unit);
    };
    slider.valueFromTextFunction = [m, percent](const juce::String &s) {
        double v = s.getDoubleValue();
        if (percent)
            return v / 100.;
        if (m.unit == "Hz" && s.containsIgnoreCase("k"))
            return v * 1000;
        return v;
    };
    slider.setTitle(m.name);
    slider.setTooltip(m.module + " / " + m.name + " — double-click to reset");
    slider.setDoubleClickReturnValue(true, m.initial);
    learnButton.onClick = std::move(action);
    learnButton.setTitle("Learn " + juce::String(m.module + " / " + m.name));
    addAndMakeVisible(learnButton);
    if (auto *options = spec["options"].getArray())
        for (int i = 0; i < options->size(); ++i) {
            auto button = std::make_unique<juce::TextButton>((*options)[i].toString());
            button->onClick = [this, i] { slider.setValue(i, juce::sendNotificationSync); };
            addAndMakeVisible(*button);
            choices.push_back(std::move(button));
        }
    slider.onValueChange = [this] {
        for (size_t i = 0; i < choices.size(); ++i)
            choices[i]->setToggleState(int(slider.getValue()) == int(i),
                                       juce::dontSendNotification);
    };
    if (choices.empty())
        addAndMakeVisible(slider);
    // JUCE's attachment installs its own text callbacks. Preserve the UI's units/parsers
    // while retaining the host's original parameter range and notifications.
    auto format = slider.textFromValueFunction;
    auto parse = slider.valueFromTextFunction;
    attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, m.id, slider);
    slider.textFromValueFunction = std::move(format);
    slider.valueFromTextFunction = std::move(parse);
    slider.updateText();
    slider.onValueChange();
}
void ParameterControl::paint(juce::Graphics &g) {
    if (caption.isNotEmpty())
        label(g, look, caption, {0, 0, float(getWidth()), 20}, getWidth() < 100 ? 12.f : 14.f,
              silver, juce::Justification::centred);
}
void ParameterControl::resized() {
    int top = caption.isEmpty() ? 0 : 22;
    if (!choices.empty() && getHeight() > 180)
        top = 80;
    if (choices.empty()) {
        slider.setBounds(0, top, getWidth(), std::max(10, getHeight() - top - 20));
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, std::min(105, getWidth()),
                               getHeight() < 150 ? 18 : 24);
        learnButton.setBounds(getWidth() / 2 - std::min(70, getWidth() - 8) / 2, getHeight() - 18,
                              std::min(70, getWidth() - 8), 17);
    } else {
        int count = int(choices.size()), buttonWidth = getWidth() / count;
        int height = std::min(32, std::max(18, getHeight() - top - 19));
        for (int i = 0; i < count; ++i)
            choices[size_t(i)]->setBounds(i * buttonWidth, top, buttonWidth - 4, height);
        learnButton.setBounds(getWidth() / 2 - 32, top + height + 2, 64, 16);
    }
}
} // namespace da::ui
