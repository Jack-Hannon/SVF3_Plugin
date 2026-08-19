// This plugin was developed using Jan Wilczek's (WolfSound)
// Audio Plugin Template:
// https://github.com/JanWilczek/audio-plugin-template

/*
==============================================================================

BEGIN_JUCE_MODULE_DECLARATION

   ID:            SVF3
   vendor:        Jack Hannon
   version:       1.0
   name:          3rd Order Nonlinear Filter
   description:   3rd Order State Variable Filter with a tunable nonlinearity to transition between linear and chaotic operation regions
   dependencies:  juce_audio_utils

END_JUCE_MODULE_DECLARATION

==============================================================================
*/

#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <ranges>
#include <algorithm>
#include <array>
#include <cmath>
#include <deque>
#include <span>
#include <algorithm>
#include <juce_audio_processors/juce_audio_processors.h>
#include "include/PluginProcessor.h"
#include "include/PluginEditor.h"
#include "include/Svf3/Svf3.h"
#include "include/Svf3/Parameters.h"
#include "include/Svf3/JsonSerializer.h"
