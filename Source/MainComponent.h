/*
===============================================================================

Copyright (C) 2021 Bernardo Escalona. All Rights Reserved.

  This file is part of Pipe Dreamer, found at:
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
#include "GameRenderer.h"


// ---- Forward declarations ----

class Controller;
class ScoreWindow;


// ---- Class Definition ----

/**
 * GUI Component that occupies the entire game window.
 * Owns the Controller and delegates all rendering to GameRenderer.
 */
class MainComponent  :	public juce::Component,
						public juce::Timer,
						public juce::ChangeListener
{
public:
	/**
	 * Class constructor. Creates the Controller and starts the GUI timer.
	 */
    MainComponent();

	/**
	 * Class destructor.
	 */
	~MainComponent() override;

	/**
	 * Get the width & height of a tile piece, in pixels.
	 *
	 * @return Tile size in pixels.
	 */
	int GetTileSize() const;

	/**
	 * Reimplemented from juce::Component.
	 */
	void paint(juce::Graphics&) override;

	/**
	 * Reimplemented from juce::Component.
	 */
	void resized() override;

	/**
	 * Reimplemented from juce::Component.
	 */
	void mouseDown(const juce::MouseEvent& event) override;

	/**
	 * Reimplemented from juce::Timer.
	 */
	void timerCallback() override;

	/**
	 * Reimplemented from juce::ChangeListener.
	 */
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
	/**
	 * Owns the game Controller for the lifetime of this component.
	 */
	std::unique_ptr<Controller> m_controller;

	/**
	 * Handles all game scene rendering.
	 */
	GameRenderer m_renderer;

	/**
	 * Subcomponent for displaying the player's score after each round.
	 */
	std::unique_ptr<ScoreWindow> m_scoreWindow;

	/**
	 * Number of timerCallback ticks until Ooze starts pumping out.
	 */
	int m_countDown = 0;

	int m_blockInteraction = 0;

	juce::CriticalSection m_lock;

	/**
	 * Hyperlink to the project URL.
	 */
	std::unique_ptr<juce::HyperlinkButton> m_hyperlink;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
