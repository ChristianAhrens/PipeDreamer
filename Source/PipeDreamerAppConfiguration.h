/*
===============================================================================

Copyright (C) 2026 Christian Ahrens. All Rights Reserved.

  This file is part of the PipeDreamer fork maintained by Christian Ahrens, found at:
  https://github.com/christianahrens/PipeDreamer

  Original project by Bernardo Escalona:
  https://github.com/escalonely/PipeDreamer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

===============================================================================
*/


#pragma once

#include <JuceHeader.h>
#include <AppConfigurationBase.h>


// ---- Class Definition ----

/**
 * Application-specific configuration for PipeDreamer.
 * Persists appearance settings (palette style and highlight colour) to disk
 * using the AppConfigurationBase XML scheme.
 */
class PipeDreamerAppConfiguration : public JUCEAppBasics::AppConfigurationBase
{
public:
    /** XML tag names for each persisted section. */
    enum TagID
    {
        LOOKANDFEEL,        //< Palette style index (0 = Dark, 1 = Light).
        HIGHLIGHTCOLOUR,    //< Highlight colour stored as a 32-bit ARGB hex string.
    };

    static juce::String getTagName(TagID id)
    {
        switch (id)
        {
        case LOOKANDFEEL:     return "LookAndFeel";
        case HIGHLIGHTCOLOUR: return "HighlightColour";
        default:              return {};
        }
    }

    explicit PipeDreamerAppConfiguration(const juce::File& file);
    ~PipeDreamerAppConfiguration() override = default;

    bool isValid() override;
    bool ResetToDefault();

protected:
    bool HandleConfigVersionConflict(const Version& configVersionFound) override;

private:
    static constexpr const char* CONFIG_VERSION = "1.0.0";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PipeDreamerAppConfiguration)
};
