cmake_minimum_required(VERSION 3.22)
project(DAPluginsBundle VERSION 0.2.0)
include(FetchContent)
FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.6 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)
set(DA_PLUGIN_CODES DVerb DDelay DComp DEQ DSat DTrans DStereo DSess DGate DFX)
foreach(PLUGIN_CODE IN LISTS DA_PLUGIN_CODES)
  juce_add_plugin(${PLUGIN_CODE}
    COMPANY_NAME "D.A Plugins" BUNDLE_ID "com.daplugins.${PLUGIN_CODE}"
    PLUGIN_MANUFACTURER_CODE DAPL PLUGIN_CODE ${PLUGIN_CODE}
    FORMATS VST3 AU Standalone PRODUCT_NAME "${PLUGIN_CODE}"
    IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE IS_MIDI_EFFECT FALSE)
  target_sources(${PLUGIN_CODE} PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
  target_compile_definitions(${PLUGIN_CODE} PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 DA_PLUGIN_NAME="${PLUGIN_CODE}")
  target_link_libraries(${PLUGIN_CODE} PRIVATE juce::juce_audio_utils juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags)
endforeach()
