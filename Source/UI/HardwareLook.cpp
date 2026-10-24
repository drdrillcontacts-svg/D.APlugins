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
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff4f4fa));
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
juce::Font HardwareLook::font(float size, bool bold) const {
    return juce::Font(juce::FontOptions(bold ? semibold : regular).withHeight(size))
        .withHorizontalScale(1.08f);
}
juce::Typeface::Ptr HardwareLook::getTypefaceForFont(const juce::Font &f) {
    return f.isBold() ? semibold : regular;
}
juce::Font HardwareLook::getComboBoxFont(juce::ComboBox &) { return font(18.5f, true); }
juce::Font HardwareLook::getTextButtonFont(juce::TextButton &, int) { return font(17.5f, true); }
juce::Font HardwareLook::getLabelFont(juce::Label &) { return font(20.0f, true); }
void label(juce::Graphics &g, const HardwareLook &look, juce::String text,
           juce::Rectangle<float> box, float size, juce::Colour colour, juce::Justification align,
           bool bold) {
    g.setFont(look.font(size, bold));
    g.setColour(colour);
    g.drawText(text, box, align, false);
    if (bold) {
        g.setColour(colour.withAlpha(.55f));
        g.drawText(text, box.translated(.45f, 0.f), align, false);
    }
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
                                    float startAngle, float endAngle, juce::Slider &slider) {
    int customRadius = int(slider.getProperties()["knobRadius"]);
    float maxFit = std::min(float(w), float(h)) * .5f - 5.f;
    float r = std::min(maxFit, customRadius > 0 ? float(customRadius) : (h >= 85 ? 28.f : 24.f));
    r = std::max(14.f, r);
    juce::Point<float> c{float(x) + w * .5f, float(y) + h * .5f};
    g.setOpacity(slider.isEnabled() ? 1.f : .48f);

    // Outer track & active value arc in the plugin's accent colour
    float arcR = r + 4.2f;
    juce::Path bgArc;
    bgArc.addCentredArc(c.x, c.y, arcR, arcR, 0.f, startAngle, endAngle, true);
    g.setColour(juce::Colour(0xff26262d));
    g.strokePath(bgArc, juce::PathStrokeType(2.4f, juce::PathStrokeType::curved,
                                             juce::PathStrokeType::rounded));
    float valAngle = startAngle + position * (endAngle - startAngle);
    if (position > .003f) {
        juce::Path valArc;
        valArc.addCentredArc(c.x, c.y, arcR, arcR, 0.f, startAngle, valAngle, true);
        g.setColour(accentColour);
        g.strokePath(valArc, juce::PathStrokeType(2.6f, juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));
    }

    // 1. Recessed shadow ring
    auto shadowRing =
        juce::Rectangle<float>((r + 2.5f) * 2.f, (r + 2.5f) * 2.f).withCentre({c.x, c.y + 3.f});
    g.setColour(juce::Colour(0xff08080a));
    g.fillEllipse(shadowRing);

    // 2. Bevel collar
    auto bevelRing = juce::Rectangle<float>((r + 1.8f) * 2.f, (r + 1.8f) * 2.f).withCentre(c);
    juce::ColourGradient bevelGrad(juce::Colour(0xff3a3a40), c.x, bevelRing.getY(),
                                   juce::Colour(0xff0c0c0e), c.x, bevelRing.getBottom(), false);
    bevelGrad.addColour(.15, juce::Colour(0xff252529));
    g.setGradientFill(bevelGrad);
    g.fillEllipse(bevelRing);
    g.setColour(juce::Colour(0xff383840));
    g.drawEllipse(bevelRing, 1.f);

    // 3. Dial face
    auto dialFace = juce::Rectangle<float>(r * 2.f, r * 2.f).withCentre(c);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff19191d), c.x, dialFace.getY(),
                                           juce::Colour(0xff28282d), c.x, dialFace.getBottom(),
                                           false));
    g.fillEllipse(dialFace);
    g.setColour(juce::Colour(0xff111116));
    g.drawEllipse(dialFace, 1.f);

    // 4. Crisp high-contrast pointer line from 0.52*r to 0.90*r
    float rad = juce::degreesToRadians(-225.f + 270.f * position);
    juce::Point<float> p0{c.x + std::cos(rad) * r * .52f, c.y + std::sin(rad) * r * .52f};
    juce::Point<float> p1{c.x + std::cos(rad) * r * .90f, c.y + std::sin(rad) * r * .90f};
    g.setColour(juce::Colour(0xfff4f4fa));
    g.drawLine({p0, p1}, 2.4f);
    g.setOpacity(1.f);
}
void HardwareLook::drawButtonBackground(juce::Graphics &g, juce::Button &b, const juce::Colour &,
                                        bool hover, bool down) {
    auto r = b.getLocalBounds().toFloat().reduced(.5f);
    bool isChoice = bool(b.getProperties()["choice"]);
    bool accent = b.getToggleState() || bool(b.getProperties()["accent"]);
    float cut = isChoice ? 5.f : 7.f;
    juce::Colour fill =
        isChoice ? (accent ? juce::Colour(0xff141418).interpolatedWith(accentColour, .24f)
                           : juce::Colour(0xff141418))
                 : juce::Colour(0xff19191e);
    juce::Colour stroke = accent ? accentColour.withAlpha(hover || down ? 1.f : .95f)
                                 : juce::Colour(isChoice ? 0xff45454e : 0xff484852);
    cutPanel(g, r, fill, stroke, cut);
}
void HardwareLook::drawButtonText(juce::Graphics &g, juce::TextButton &b, bool, bool) {
    bool isChoice = bool(b.getProperties()["choice"]);
    bool accent = b.getToggleState() || bool(b.getProperties()["accent"]);
    juce::Colour textCol =
        b.isEnabled() ? (accent ? (isChoice ? accentColour.interpolatedWith(juce::Colours::white, .72f)
                                            : accentColour.interpolatedWith(juce::Colours::white, .25f))
                                : juce::Colour(isChoice ? 0xffe4e4ee : 0xfff0f0f7))
                      : muted.withAlpha(.55f);
    float fsize = b.getHeight() <= 27 ? 16.5f : (isChoice ? 16.5f : 17.5f);
    label(g, *this, b.getButtonText(), b.getLocalBounds().toFloat().reduced(2, 0), fsize, textCol,
          juce::Justification::centred, true);
}
void HardwareLook::positionComboBoxText(juce::ComboBox &, juce::Label &lbl) {
    // Prevent JUCE's internal ComboBox Label from double-drawing over drawComboBox().
    lbl.setBounds(0, 0, 0, 0);
    lbl.setVisible(false);
}
void HardwareLook::drawComboBox(juce::Graphics &g, int w, int h, bool, int, int, int, int,
                                juce::ComboBox &box) {
    cutPanel(g, {0.5f, 0.5f, float(w - 1), float(h - 1)}, juce::Colour(0xff19191d),
             juce::Colour(0xff464650), 7.f);
    label(g, *this, box.getText(), {14.f, 0.f, float(w - 38), float(h)}, 18.0f,
          juce::Colour(0xfff2f2f8), juce::Justification::centredLeft, true);
    g.setColour(accentColour);
    juce::Path p;
    p.addTriangle(float(w - 22), h * .42f, float(w - 12), h * .42f, float(w - 17), h * .60f);
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
    // 1. Edge-to-edge Matte Noir Black chassis (zero wasted side margin!)
    auto body = juce::Rectangle<float>(1.f, 1.f, 1118.f, 858.f);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff222225), 0.f, 0.f,
                                           juce::Colour(0xff131316), 0.f, 860.f, false));
    g.fillRoundedRectangle(body, 12.f);
    g.setColour(juce::Colour(0xff36363d));
    g.drawRoundedRectangle(body, 12.f, 1.2f);

    // 2. Brand logo & untouched product name in compact top header (y=8..68)
    g.setColour(juce::Colour(0xffdcdce2));
    g.drawImageWithin(logo, 18, 12, 126, 52, juce::RectanglePlacement::stretchToFit, true);
    label(g, look, layout.title("name").toUpperCase(), {164.f, 8.f, 310.f, 36.f}, 34.f,
          juce::Colour(0xfff4f4f8), juce::Justification::centredLeft, true);
    label(g, look, layout.title("subtitle"), {166.f, 44.f, 360.f, 22.f}, 16.5f,
          juce::Colour(0xffd0d0da), juce::Justification::centredLeft, true);
    label(g, look, "FACTORY PRESET / 50", {600.f, 6.f, 320.f, 20.f}, 15.5f,
          juce::Colour(0xffd4d4de), juce::Justification::centredLeft, true);

    // 3. Full-width inset display housing (14, 74, 1092, 220) & inner screen (22, 82, 1076, 204)
    auto dispOuter = juce::Rectangle<float>(14.f, 74.f, 1092.f, 220.f);
    juce::ColourGradient dispGrad(juce::Colour(0xff343439), 14.f, 74.f,
                                  juce::Colour(0xff0d0d10), 14.f, 294.f, false);
    dispGrad.addColour(.15, juce::Colour(0xff222226));
    g.setGradientFill(dispGrad);
    g.fillRoundedRectangle(dispOuter, 16.f);
    g.setColour(juce::Colour(0xff323238));
    g.drawRoundedRectangle(dispOuter, 16.f, 1.f);
    auto dispInner = juce::Rectangle<float>(22.f, 82.f, 1076.f, 204.f);
    g.setColour(juce::Colour(0xff0d0d10));
    g.fillRoundedRectangle(dispInner, 12.f);
    g.setColour(juce::Colour(0xff26262c));
    g.drawRoundedRectangle(dispInner, 12.f, 1.f);

    // 4. Horizontal Learning toolbar (14, 302, 1092, 48)
    cutPanel(g, {14.f, 302.f, 1092.f, 48.f}, juce::Colour(0xff111114), juce::Colour(0xff3a3a42),
             8.f);
    label(g, look, "LEARNING", {28.f, 313.f, 136.f, 26.f}, 18.5f, juce::Colour(0xfff0f0f6),
          juce::Justification::centredLeft, true);

    // 5. Main & Advanced two-tier control panels (full-width x=14..1106)
    label(g, look, layout.title("slogan"), {18.f, 355.f, 460.f, 24.f}, 18.0f,
          juce::Colour(0xffe8e8f2), juce::Justification::centredLeft, true);
    cutPanel(g, {14.f, 382.f, 1092.f, 192.f}, juce::Colour(0xff17171b), juce::Colour(0xff3c3c44),
             10.f);
    label(g, look, layout.title("advancedTitle"), {18.f, 581.f, 480.f, 24.f}, 18.0f,
          juce::Colour(0xffe8e8f2), juce::Justification::centredLeft, true);
    label(g, look, "ADVANCED / EXPANDED", {730.f, 581.f, 372.f, 24.f}, 17.0f,
          look.accentColour.interpolatedWith(juce::Colour(0xffe0e0ea), .35f),
          juce::Justification::centredRight, true);
    cutPanel(g, {14.f, 608.f, 1092.f, 194.f}, juce::Colour(0xff17171b), juce::Colour(0xff3c3c44),
             10.f);

    if (layout.title("name") == "DEQ") {
        const int bands[3]{1, 3, 4};
        for (int j = 0; j < 3; ++j) {
            float bx = 22.f + float(j * 362);
            label(g, look, "BAND 0" + juce::String(bands[j]), {bx + 10.f, 613.f, 180.f, 24.f},
                  18.5f, look.accentColour, juce::Justification::centredLeft, true);
            if (j < 2) {
                g.setColour(juce::Colour(0xff35353e));
                g.drawLine(bx + 356.f, 618.f, bx + 356.f, 792.f, 1.f);
            }
        }
    }

    if (overlay)
        overlay(g);
}
ParameterControl::ParameterControl(juce::AudioProcessorValueTreeState &state,
                                   const da::Parameter &m, const juce::var &spec, HardwareLook &f,
                                   std::function<void()> action, std::function<void()> onChange)
    : metadata(m), caption(spec["label"].toString()), styleName(spec["style"].toString()),
      knobRadius(int(spec["radius"]) > 0 ? int(spec["radius"]) : 28),
      learnable(bool(spec["learnable"])), look(f), onChanged(std::move(onChange)) {
    setComponentID(m.id);
    setTitle(m.module + " / " + m.name);
    setDescription(learnable ? "Audio-informed learning available" : "Manual control");
    slider.getProperties().set("knobRadius", knobRadius);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                               juce::MathConstants<float>::pi * 2.75f, true);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 126, 25);
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
            return (v > 0.05 ? "+" : "") +
                   juce::String(v, std::abs(v - std::round(v)) > .05 ? 1 : 0) + " dB";
        return juce::String(v, std::abs(v - std::round(v)) > .05 ? 2 : 0);
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
    slider.setTooltip(m.module + " / " + m.name + " - double-click to reset");
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
        if (onChanged)
            onChanged();
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
        label(g, look, caption, {4.f, 4.f, 92.f, float(getHeight() - 8)}, 19.f,
              juce::Colour(0xfff4f4fa), juce::Justification::centredLeft, true);
        if (learnable) {
            g.setColour(look.accentColour);
            g.fillEllipse(88.f, float(getHeight()) * .5f - 3.f, 6.f, 6.f);
        }
        return;
    }
    if (caption.isNotEmpty()) {
        float titleFont = getWidth() < 118 ? 18.5f : (caption.length() >= 10 && getWidth() < 140 ? 19.5f : 22.0f);
        label(g, look, caption, {0.f, 0.f, float(getWidth()), 26.f}, titleFont,
              juce::Colour(0xfff8f8fc), juce::Justification::centred, true);
        if (learnable) {
            float dotX = float(getWidth()) * .5f + float(caption.length()) * 5.8f + 10.f;
            dotX = std::min(dotX, float(getWidth()) - 6.f);
            g.setColour(look.accentColour);
            g.fillEllipse(dotX - 3.f, 8.f, 6.f, 6.f);
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
    int top = caption.isEmpty() ? 0 : 26;
    if (isMenu) {
        choiceMenu.setBounds(0, top, getWidth(), std::min(36, getHeight() - top));
        return;
    }
    if (choices.empty()) {
        int learnH = learnable ? 26 : 0;
        slider.setBounds(0, top, getWidth(), std::max(10, getHeight() - top - learnH - 2));
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, std::min(128, getWidth()), 24);
        if (learnable)
            learnButton.setBounds(getWidth() / 2 - 42, getHeight() - 26, 84, 26);
    } else {
        int count = int(choices.size());
        int gap = count == 2 ? (getWidth() > 180 ? 12 : 8) : (count == 3 ? 8 : 6);
        int buttonWidth = (getWidth() - gap * (count - 1)) / count;
        int height = caption.isEmpty() ? getHeight() : 38;
        int cy = caption.isEmpty() ? 0 : top + 24;
        for (int i = 0; i < count; ++i)
            choices[size_t(i)]->setBounds(i * (buttonWidth + gap), cy, buttonWidth, height);
        if (learnable)
            learnButton.setBounds(getWidth() / 2 - 42, cy + height + 6, 84, 26);
    }
}
} // namespace da::ui
