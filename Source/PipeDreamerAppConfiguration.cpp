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


#include "PipeDreamerAppConfiguration.h"


// ---- Class Implementation ----

PipeDreamerAppConfiguration::PipeDreamerAppConfiguration(const juce::File& file)
{
    InitializeBase(file, Version(CONFIG_VERSION));
}

bool PipeDreamerAppConfiguration::isValid()
{
    // Delegate root-tag check to the base, then verify our required section exists.
    if (!AppConfigurationBase::isValid())
        return false;

    auto lookAndFeelXml = getConfigState(getTagName(LOOKANDFEEL));
    if (!lookAndFeelXml)
        return false;

    return true;
}

bool PipeDreamerAppConfiguration::ResetToDefault()
{
    // Build a minimal valid XML tree with hardcoded defaults:
    //   LookAndFeel  = 0 (PS_Dark)
    //   HighlightColour = 0xff0077cc (medium blue)
    auto root = std::make_unique<juce::XmlElement>(juce::JUCEApplication::getInstance()->getApplicationName());
    root->setAttribute("configVersion", CONFIG_VERSION);

    auto* lafXml = root->createNewChildElement(getTagName(LOOKANDFEEL));
    lafXml->addTextElement("0");

    auto* colourXml = root->createNewChildElement(getTagName(HIGHLIGHTCOLOUR));
    colourXml->addTextElement("ff1e90ff"); // dodgerblue

    return resetConfigState(std::move(root));
}

bool PipeDreamerAppConfiguration::HandleConfigVersionConflict(const Version& configVersionFound)
{
    // For now: accept any version found on disk rather than wiping user settings.
    juce::ignoreUnused(configVersionFound);
    return true;
}
