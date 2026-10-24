#include "HardwareLook.h"
#include <DAAssets.h>
namespace da::ui {
namespace {
juce::Path bevel(juce::Rectangle<float> r, float cut = 8.f) {
    juce::Path p;
    auto x = r.getX(), y = r.getY(), w = r.getWidth(), h = r.getHeight();
    cut = std::min(cut, std::min(w, h) * .45f);
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
HardwareLook::HardwareLook(juce::Colour accent) : accentColour(accent) {
    regular = juce::Typeface::createSystemTypefaceFor(DAAssets::RajdhaniMedium_ttf,
                                                      DAAssets::RajdhaniMedium_ttfSize);
    semibold = juce::Typeface::createSystemTypefaceFor(DAAssets::RajdhaniSemiBold_ttf,
                                                       DAAssets::RajdhaniSemiBold_ttfSize);
    setAccent(accent);
}
void HardwareLook::setAccent(juce::Colour c) {
    accentColour = c;
    setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff111114));
    setColour(juce::TextEditor::textColourId, silver);
    setColour(juce::TextEditor::outlineColourId, juce::Colour(0xff393940));
    setColour(juce::TextEditor::focusedOutlineColourId, accentColour.withAlpha(.8f));
    setColour(juce::TextEditor::highlightColourId, accentColour.withAlpha(.3f));
    setColour(juce::ComboBox::textColourId, silver);
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff19191d));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff414146));
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff141418));
    setColour(juce::PopupMenu::highlightedBackgroundColourId,
              juce::Colour(0xff141418).interpolatedWith(accentColour, .28f));
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffd2d2db));
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
juce::Font HardwareLook::font(float size, bool bold) const {
    return juce::Font(juce::FontOptions(bold ? semibold : regular).withHeight(size));
}
juce::Typeface::Ptr HardwareLook::getTypefaceForFont(const juce::Font &f) {
    return f.isBold() ? semibold : regular;
}
juce::Font HardwareLook::getComboBoxFont(juce::ComboBox &) { return font(16, true); }
juce::Font HardwareLook::getTextButtonFont(juce::TextButton &, int) { return font(16, true); }
juce::Font HardwareLook::getLabelFont(juce::Label &) { return font(18, true); }
void label(juce::Graphics &g, const HardwareLook &look, juce::String text,
           juce::Rectangle<float> box, float size, juce::Colour colour, juce::Justification align,
           bool bold) {
    g.setFont(look.font(size, bold));
    g.setColour(colour);
    g.drawText(text, box, align, false);
}
void cutPanel(juce::Graphics &g, juce::Rectangle<float> r, juce::Colour fill, juce::Colour stroke,
              float cut) {
    auto path = bevel(r, cut);
    g.setColour(fill);
    g.fillPath(path);
    g.setColour(stroke);
    g.strokePath(path, juce::PathStrokeType(1.f));
}
void HardwareLook::drawRotarySlider(juce::Graphics &g, int x, int y, int w, int h, float position,
                                    float, float, juce::Slider &slider) {
    int customRadius = int(slider.getProperties()["knobRadius"]);
    float r = customRadius > 0 ? float(customRadius) : (h >= 85 ? 28.f : 24.f);
    juce::Point<float> c{float(x) + w * .5f, float(y) + h * .5f};
    g.setOpacity(slider.isEnabled() ? 1.f : .48f);
    // 1. Recessed shadow ring: circle(cx, cy+4, r+4, '#08080a', '#19191e')
    auto shadowRing = juce::Rectangle<float>((r + 4.f) * 2.f, (r + 4.f) * 2.f).withCentre({c.x, c.y + 4.f});
    g.setColour(juce::Colour(0xff08080a));
    g.fillEllipse(shadowRing);
    g.setColour(juce::Colour(0xff19191e));
    g.drawEllipse(shadowRing, 1.f);
    // 2. Bevel collar: circle(cx, cy, r+2, 'url(#bevel)', '#35353c')
    auto bevelRing = juce::Rectangle<float>((r + 2.f) * 2.f, (r + 2.f) * 2.f).withCentre(c);
    juce::ColourGradient bevelGrad(juce::Colour(0xff37373a), c.x, bevelRing.getY(),
                                   juce::Colour(0xff0c0c0e), c.x, bevelRing.getBottom(), false);
    bevelGrad.addColour(.15, juce::Colour(0xff242427));
    g.setGradientFill(bevelGrad);
    g.fillEllipse(bevelRing);
    g.setColour(juce::Colour(0xff35353c));
    g.drawEllipse(bevelRing, 1.f);
    // 3. Dial face: circle(cx, cy, r, 'url(#dial)', '#111116')
    auto dialFace = juce::Rectangle<float>(r * 2.f, r * 2.f).withCentre(c);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff18181b), c.x, dialFace.getY(),
                                           juce::Colour(0xff27272a), c.x, dialFace.getBottom(), false));
    g.fillEllipse(dialFace);
    g.setColour(juce::Colour(0xff111116));
    g.drawEllipse(dialFace, 1.f);
    // 4. Minimalist pointer line from 0.62*r to 0.90*r at angle -225 deg + 270 deg * position
    float rad = juce:: degreesToRadians(-225.f + 270.f * position);
    juce::Point<float> p0{c.x + std::cos(rad) * r * .62f, c.y + std::sin(rad) * r * .62f};
    juce::Point<float> p1{c.x + std::cos(rad) * r * .90f, c.y + std::sin(rad) * r * .90f};
    g.setColour(juce::Colour(0xffceced4));
    g.drawLine({p0, p1}, 2.f);
    g.setOpacity(1.f);
}
void HardwareLook::drawButtonBackground(juce::Graphics &g, juce::Button &b, const juce::Colour &,
                                        bool hover, bool down) {
    auto r = b.getLocalBounds().toFloat().reduced(.5f);
    bool isChoice = bool(b.getProperties()["choice"]);
    bool accent = b.getToggleState() || bool(b.getProperties()["accent"]);
    float cut = isChoice ? 5.f : 7.f;
    juce::Colour fill = isChoice
                            ? (accent ? juce::Colour(0xff141418).interpolatedWith(accentColour, .18f)
                                      : juce::Colour(0xff141418))
                            : juce::Colour(0xff19191d);
    juce::Colour stroke = accent ? accentColour.withAlpha(hover || down ? 1.f : .92f)
                                 : juce::Colour(isChoice ? 0xff393940 : 0xff414146);
    cutPanel(g, r, fill, stroke, cut);
}
void HardwareLook::drawButtonText(juce::Graphics &g, juce::TextButton &b, bool, bool) {
    bool isChoice = bool(b.getProperties()["choice"]);
    bool accent = b.getToggleState() || bool(b.getProperties()["accent"]);
    juce::Colour textCol = b.isEnabled()
                               ? (accent ? (isChoice ? accentColour.interpolatedWith(juce::Colours::white, .62f)
                                                     : accentColour)
                                         : juce::Colour(isChoice ? 0xffb9b9c5 : 0xffdcdce2))
                               : muted.withAlpha(.55f);
    label(g, *this, b.getButtonText(), b.getLocalBounds().toFloat().reduced(2, 0), 16.f, textCol,
          juce::Justification::centred, true);
}
void HardwareLook::drawComboBox(juce::Graphics &g, int w, int h, bool, int, int, int, int,
                                juce::ComboBox &box) {
    cutPanel(g, {0.5f, 0.5f, float(w - 1), float(h - 1)}, juce::Colour(0xff19191d),
             juce::Colour(0xff414146), 7.f);
    label(g, *this, box.getText(), {12.f, 0.f, float(w - 36), float(h)}, 16.f,
          juce::Colour(0xffdcdce2), juce::Justification::centred, true);
    g.setColour(muted);
    juce::Path p;
    p.addTriangle(float(w - 22), h * .44f, float(w - 12), h * .44f, float(w - 17), h * .60f);
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
juce::Colour Layout::accent() const {
    auto s = data["accent"].toString().trimCharactersAtStart("#");
    if (s.length() == 6)
        return juce::Colour(juce::uint32(0xff000000) | juce::uint32(s.getHexValue32()));
    return juce::Colour(0xffa28bd0);
}
void panel(juce::Graphics &g, juce::Rectangle<float> r, bool) {
    cutPanel(g, r, juce::Colour(0xff17171b), juce::Colour(0xff3a3a40), 11.f);
}
Chassis::Chassis(const Layout &l, const HardwareLook &f) : layout(l), look(f) {
    setInterceptsMouseClicks(false, false);
    logo = juce::ImageFileFormat::loadFrom(DAAssets::DAmark_png, DAAssets::DAmark_pngSize);
}
void Chassis::paint(juce::Graphics &g) {
    g.fillAll(juce::Colour(0xff101416));
    // 1. Matte Noir Black outer body (30, 26, 1140, 1188, rx=30)
    auto body = juce::Rectangle<float>(30.f, 26.f, 1140.f, 1188.f);
    juce::DropShadow(juce::Colours::black.withAlpha(.45f), 14, {0, 8})
        .drawForPath(g, [](juce::Rectangle<float> r) {
            juce::Path p;
            p.addRoundedRectangle(r, 30.f);
            return p;
        }(body));
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff222224), 30.f, 26.f,
                                           juce::Colour(0xff131315), 30.f, 1214.f, false));
    g.fillRoundedRectangle(body, 30.f);
    g.setColour(juce::Colour(0xff323235));
    g.drawRoundedRectangle(body, 30.f, 1.f);
    g.setColour(juce::Colours::white.withAlpha(.03f));
    g.drawRoundedRectangle({35.f, 31.f, 1130.f, 1178.f}, 26.f, 1.f);

    // 2. Brand logo (alpha silhouette filled with #d1d1d3 at 77, 85, 155x68) & untouched product name
    g.setColour(juce::Colour(0xffd1d1d3));
    g.drawImageWithin(logo, 77, 85, 155, 68, juce::RectanglePlacement::stretchToFit, true);
    label(g, look, layout.title("name").toUpperCase(), {278.f, 84.f, 310.f, 44.f}, 37.f,
          juce::Colour(0xffd8d8db), juce::Justification::centredLeft, false);
    label(g, look, layout.title("subtitle"), {280.f, 138.f, 320.f, 24.f}, 16.f,
          juce::Colour(0xffbebec4), juce::Justification::centredLeft, true);
    label(g, look, "FACTORY PRESET / 50", {660.f, 70.f, 300.f, 22.f}, 16.f,
          juce::Colour(0xffc2c2ca), juce::Justification::centredLeft, true);

    // 3. Inset display housing (52, 190, 1096, 378, rx=29) & inner screen (70, 207, 1060, 342, rx=20)
    auto dispOuter = juce::Rectangle<float>(52.f, 190.f, 1096.f, 378.f);
    juce::ColourGradient dispGrad(juce::Colour(0xff37373a), 52.f, 190.f,
                                  juce::Colour(0xff0c0c0e), 52.f, 568.f, false);
    dispGrad.addColour(.15, juce::Colour(0xff242427));
    g.setGradientFill(dispGrad);
    g.fillRoundedRectangle(dispOuter, 29.f);
    g.setColour(juce::Colour(0xff29292c));
    g.drawRoundedRectangle(dispOuter, 29.f, 1.f);
    auto dispInner = juce::Rectangle<float>(70.f, 207.f, 1060.f, 342.f);
    g.setColour(juce::Colour(0xff0e0e11));
    g.fillRoundedRectangle(dispInner, 20.f);
    g.setColour(juce::Colour(0xff222226));
    g.drawRoundedRectangle(dispInner, 20.f, 1.f);

    // 4. Horizontal Learning toolbar (55, 590, 1090, 72)
    cutPanel(g, {55.f, 590.f, 1090.f, 72.f}, juce::Colour(0xff111114), juce::Colour(0xff36363b),
             10.f);
    label(g, look, "LEARNING", {76.f, 613.f, 145.f, 26.f}, 16.f, juce::Colour(0xffdcdce2),
          juce::Justification::centredLeft, true);

    // 5. Main & Advanced two-tier control panels
    label(g, look, layout.title("slogan"), {73.f, 664.f, 440.f, 22.f}, 16.f,
          juce::Colour(0xffc0c0ca), juce::Justification::centredLeft, true);
    cutPanel(g, {55.f, 693.f, 1090.f, 177.f}, juce::Colour(0xff17171b), juce::Colour(0xff3a3a40),
             11.f);
    label(g, look, layout.title("advancedTitle"), {73.f, 877.f, 440.f, 22.f}, 16.f,
          juce::Colour(0xffc0c0ca), juce::Justification::centredLeft, true);
    label(g, look, "ADVANCED / EXPANDED", {760.f, 877.f, 370.f, 22.f}, 16.f,
          look.accentColour.interpolatedWith(juce::Colour(0xffb8b8c2), .45f),
          juce::Justification::centredRight, true);
    cutPanel(g, {55.f, 906.f, 1090.f, 229.f}, juce::Colour(0xff17171b), juce::Colour(0xff3a3a40),
             11.f);

    if (layout.title("name") == "DEQ") {
        const int bands[3]{1, 3, 4};
        for (int j = 0; j < 3; ++j) {
            float bx = 83.f + float(j * 357);
            label(g, look, "BAND 0" + juce::String(bands[j]), {bx + 12.f, 916.f, 180.f, 22.f}, 16.f,
                  look.accentColour, juce::Justification::centredLeft, true);
            if (j < 2) {
                g.setColour(juce::Colour(0xff35353e));
                g.drawLine(bx + 331.f, 929.f, bx + 331.f, 1114.f, 1.f);
            }
        }
    } else if (layout.title("name") == "DTrans" || layout.title("name") == "DStereo") {
        label(g, look, "ALL PARAMETERS VISIBLE / NO DUPLICATED CONTROLS",
              {200.f, 1092.f, 800.f, 22.f}, 16.f, juce::Colour(0xffabaab8),
              juce::Justification::centred, true);
    }

    // 6. Footer guide
    label(g, look, "PREVIEW AUDITIONS / APPLY KEEPS / REJECT RESTORES",
          {74.f, 1150.f, 620.f, 22.f}, 16.f, juce::Colour(0xffb5b5c2),
          juce::Justification::centredLeft, true);

    if (overlay)
        overlay(g);
}
ParameterControl::ParameterControl(juce::AudioProcessorValueTreeState &state,
                                   const da::Parameter &m, const juce::var &spec, HardwareLook &f,
                                   std::function<void()> action)
    : metadata(m), caption(spec["label"].toString()), styleName(spec["style"].toString()),
      knobRadius(int(spec["radius"]) > 0 ? int(spec["radius"]) : 28),
      learnable(bool(spec["learnable"])), look(f) {
    setComponentID(m.id);
    setTitle(m.module + " / " + m.name);
    setDescription(learnable ? "Audio-informed learning available" : "Manual control");
    slider.getProperties().set("knobRadius", knobRadius);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                               juce::MathConstants<float>::pi * 2.75f, true);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 110, 22);
    isBandRow = (styleName == "band_row");
    bool isDiscreteRotary = (styleName == "rotary_discrete");
    auto *options = spec["options"].getArray();
    juce::StringArray optionStrings;
    if (options)
        for (auto &o : *options)
            optionStrings.add(o.toString());

    bool percent =
        (m.unit == "%" || m.id == "mix" || m.id == "width" || m.id == "density" ||
         m.id == "diffusion" || m.id == "modulation" || m.id == "sensitivity" || m.id == "depth" ||
         m.id == "focus" || m.id == "feedback" || m.id == "delayMix" || m.id == "reverbMix");
    slider.textFromValueFunction = [m, percent, optionStrings](double v) {
        if (optionStrings.size() > 0) {
            int idx = std::clamp(int(std::round(v)), 0, optionStrings.size() - 1);
            return optionStrings[idx];
        }
        if (percent)
            return juce::String(int(std::round(v * 100))) + "%";
        if (m.unit == "Hz")
            return v >= 1000 ? juce::String(v / 1000, v >= 10000 ? 0 : 1) + " kHz"
                             : juce::String(int(std::round(v))) + " Hz";
        if (m.unit == "ms")
            return juce::String(v, v < 10 ? 1 : 0) + " ms";
        if (m.unit == ":1")
            return juce::String(v, 0) + ":1";
        if (m.unit == "s")
            return juce::String(v, 2) + " s";
        if (m.unit == "dB")
            return (v > 0.05 ? "+" : "") + juce::String(v, std::abs(v - std::round(v)) > .05 ? 1 : 0) + " dB";
        return juce::String(v, std::abs(v - std::round(v)) > .05 ? 3 : 0);
    };
    slider.valueFromTextFunction = [m, percent, optionStrings](const juce::String &s) {
        if (optionStrings.size() > 0) {
            int idx = optionStrings.indexOf(s.trim(), true);
            if (idx >= 0)
                return double(idx);
        }
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
    addChildComponent(learnButton);
    learnButton.setVisible(learnable);
    isMenu = (styleName == "menu");
    if (isMenu) {
        for (int i = 0; i < optionStrings.size(); ++i)
            choiceMenu.addItem(optionStrings[i], i + 1);
        choiceMenu.onChange = [this] {
            slider.setValue(choiceMenu.getSelectedId() - 1, juce::sendNotificationSync);
        };
        addAndMakeVisible(choiceMenu);
    }
    if (options && !isMenu && !isDiscreteRotary && !isBandRow)
        for (int i = 0; i < options->size(); ++i) {
            auto button = std::make_unique<juce::TextButton>((*options)[i].toString());
            button->getProperties().set("choice", true);
            button->onClick = [this, i] { slider.setValue(i, juce::sendNotificationSync); };
            addAndMakeVisible(*button);
            choices.push_back(std::move(button));
        }
    slider.onValueChange = [this] {
        if (isMenu)
            choiceMenu.setSelectedId(int(slider.getValue()) + 1, juce::dontSendNotification);
        for (size_t i = 0; i < choices.size(); ++i)
            choices[i]->setToggleState(int(std::round(slider.getValue())) == int(i),
                                       juce::dontSendNotification);
        repaint();
    };
    if (choices.empty() && !isMenu)
        addAndMakeVisible(slider);
    if (isBandRow) {
        slider.setSliderStyle(juce::Slider::LinearBar);
        slider.setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);
    }
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
    if (isBandRow) {
        label(g, look, caption, {4.f, 4.f, 92.f, float(getHeight() - 8)}, 16.f,
              juce::Colour(0xffc6c6d0), juce::Justification::centredLeft, true);
        if (learnable) {
            g.setColour(look.accentColour);
            g.fillEllipse(88.f, float(getHeight()) * .5f - 3.f, 5.f, 5.f);
        }
        return;
    }
    if (caption.isNotEmpty()) {
        label(g, look, caption, {0.f, 0.f, float(getWidth()), 22.f}, 16.f,
              juce::Colour(0xffcbcdd6), juce::Justification::centred, true);
        if (learnable) {
            float dotX = float(getWidth()) * .5f + float(caption.length()) * 5.2f + 10.f;
            dotX = std::min(dotX, float(getWidth()) - 5.f);
            g.setColour(look.accentColour);
            g.fillEllipse(dotX - 2.4f, 6.f, 4.8f, 4.8f);
        }
    }
}
void ParameterControl::resized() {
    if (isBandRow) {
        if (learnable)
            learnButton.setBounds(118, (getHeight() - 25) / 2, 88, 25);
        slider.setBounds(getWidth() - 96, 2, 94, getHeight() - 4);
        return;
    }
    int top = caption.isEmpty() ? 0 : 22;
    if (isMenu) {
        choiceMenu.setBounds(0, top, getWidth(), std::min(36, getHeight() - top));
        return;
    }
    if (choices.empty()) {
        int learnH = learnable ? 25 : 0;
        slider.setBounds(0, top, getWidth(), std::max(10, getHeight() - top - learnH - 2));
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, std::min(114, getWidth()), 22);
        if (learnable)
            learnButton.setBounds(getWidth() / 2 - 36, getHeight() - 25, 72, 25);
    } else {
        int count = int(choices.size());
        int gap = count == 2 ? (getWidth() > 180 ? 12 : 10) : (count == 3 ? 10 : 8);
        int buttonWidth = (getWidth() - gap * (count - 1)) / count;
        int height = 39;
        int cy = top + 16;
        for (int i = 0; i < count; ++i)
            choices[size_t(i)]->setBounds(i * (buttonWidth + gap), cy, buttonWidth, height);
        if (learnable)
            learnButton.setBounds(getWidth() / 2 - 36, cy + height + 6, 72, 25);
    }
}
} // namespace da::ui
