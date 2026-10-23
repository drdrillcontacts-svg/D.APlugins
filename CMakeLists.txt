cmake_minimum_required(VERSION 3.22)
project(DAPluginsBundle VERSION 0.3.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(DA_BUILD_PLUGINS "Build JUCE plugin formats (requires system development packages)" ON)
include(CTest)
add_library(da_core STATIC Source/Core/Suite.cpp)
target_include_directories(da_core PUBLIC Source)
set_target_properties(da_core PROPERTIES POSITION_INDEPENDENT_CODE ON)
if(BUILD_TESTING)
  add_executable(da_core_tests Tests/CoreTests.cpp)
  target_link_libraries(da_core_tests PRIVATE da_core)
  add_test(NAME suite_core COMMAND da_core_tests)
  find_package(Threads REQUIRED)
  add_executable(da_scope_tests Tests/ScopeTests.cpp)
  target_link_libraries(da_scope_tests PRIVATE da_core Threads::Threads)
  add_test(NAME scope_fifo COMMAND da_scope_tests)
  add_executable(da_capture_tests Tests/CaptureTests.cpp)
  target_link_libraries(da_capture_tests PRIVATE da_core Threads::Threads)
  add_test(NAME capture_lifecycle COMMAND da_capture_tests)
  find_package(Python3 COMPONENTS Interpreter QUIET)
  if(Python3_Interpreter_FOUND)
    add_test(NAME ui_layouts COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/test_ui_layouts.py)
  endif()
endif()
if(NOT DA_BUILD_PLUGINS)
  return()
endif()
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()
include(FetchContent)
# JUCE 8.0.6, immutable source archive and verified digest.
FetchContent_Declare(JUCE
  URL https://codeload.github.com/juce-framework/JUCE/tar.gz/51a8a6d7aeae7326956d747737ccf1575e61e209
  URL_HASH SHA256=20b740351cc030086951d87614bc08b42d072f1e4db68db1ef1d988e66dbbdbe
)
FetchContent_MakeAvailable(JUCE)
juce_add_binary_data(da_ui_assets NAMESPACE DAAssets HEADER_NAME DAAssets.h SOURCES
  Design/Brand/DAmark.png Design/Fonts/RajdhaniMedium.ttf Design/Fonts/RajdhaniSemiBold.ttf
  Design/UI/Layouts.json)
set_target_properties(da_ui_assets PROPERTIES POSITION_INDEPENDENT_CODE ON)
set(DA_PLUGIN_NAMES DVerb DDelay DComp DEQ DSat DTrans DStereo DSess DGate DFX)
set(DA_PLUGIN_IDS Dv01 Dd01 Dc01 De01 Ds01 Dt01 Dw01 Dx01 Dg01 Df01)
set(DA_FORMATS VST3 Standalone)
if(APPLE)
  list(APPEND DA_FORMATS AU)
endif()
foreach(INDEX RANGE 0 9)
  list(GET DA_PLUGIN_NAMES ${INDEX} PLUGIN_NAME)
  list(GET DA_PLUGIN_IDS ${INDEX} PLUGIN_ID)
  juce_add_plugin(${PLUGIN_NAME}
    COMPANY_NAME "D.A Plugins" BUNDLE_ID "com.daplugins.${PLUGIN_NAME}"
    PLUGIN_MANUFACTURER_CODE Dapl PLUGIN_CODE ${PLUGIN_ID}
    FORMATS ${DA_FORMATS} PRODUCT_NAME "${PLUGIN_NAME}"
    IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)
  juce_generate_juce_header(${PLUGIN_NAME})
  target_sources(${PLUGIN_NAME} PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp Source/UI/HardwareLook.cpp Source/UI/AnalysisDisplay.cpp)
  target_compile_definitions(${PLUGIN_NAME} PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0 DA_PLUGIN_NAME="${PLUGIN_NAME}")
  target_link_libraries(${PLUGIN_NAME} PRIVATE da_core da_ui_assets juce::juce_audio_utils juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags)
endforeach()

option(DA_BUILD_UI_SNAPSHOTS "Build native JUCE UI snapshot runner (requires display)" OFF)
if(DA_BUILD_UI_SNAPSHOTS)
  juce_add_console_app(da_ui_snapshots PRODUCT_NAME "da_ui_snapshots")
  juce_generate_juce_header(da_ui_snapshots)
  target_sources(da_ui_snapshots PRIVATE Tests/UISnapshots.cpp
    Source/PluginProcessor.cpp Source/PluginEditor.cpp Source/UI/HardwareLook.cpp Source/UI/AnalysisDisplay.cpp)
  target_include_directories(da_ui_snapshots PRIVATE Source)
  target_compile_definitions(da_ui_snapshots PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0
    JUCE_STANDALONE_APPLICATION=1 JUCE_MODAL_LOOPS_PERMITTED=1)
  target_link_libraries(da_ui_snapshots PRIVATE da_core da_ui_assets juce::juce_audio_utils juce::juce_dsp)
endif()
