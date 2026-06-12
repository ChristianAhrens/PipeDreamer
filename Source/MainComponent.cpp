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


#include "MainComponent.h"
#include "LayoutConstants.h"
#include "Controller.h"
#include "ScoreWindow.h"


// ---- Class Implementation ----

MainComponent::MainComponent()
{
    // Config must be created first so onConfigUpdated() can apply settings to the
    // LookAndFeel before any UI is shown. Dumper is registered now; watcher is
    // registered last (after full construction) to avoid calling virtual overrides
    // before all members are initialised.
    m_config = std::make_unique<PipeDreamerAppConfiguration>(
        juce::File(PipeDreamerAppConfiguration::getDefaultConfigFilePath()));
    if (!m_config->isValid())
        m_config->ResetToDefault();
    m_config->addDumper(this);

    m_controller = std::make_unique<Controller>();

    m_boardComponent = std::make_unique<BoardComponent>();
    addAndMakeVisible(m_boardComponent.get());

    m_queueComponent = std::make_unique<QueueComponent>();
    addAndMakeVisible(m_queueComponent.get());

    m_progressComponent = std::make_unique<ProgressComponent>();
    addAndMakeVisible(m_progressComponent.get());

    m_hyperlink = std::make_unique<juce::HyperlinkButton>(
        juce::String("https://github.com/escalonely/PipeDreamer"),
        juce::URL("https://github.com/escalonely/PipeDreamer"));
    addAndMakeVisible(m_hyperlink.get());

    setSize(Layout::WINDOW_DEFAULT_W, Layout::WINDOW_DEFAULT_H);

    m_maxCountDown = m_controller->GetCurrentCountdown();
    m_countDown    = m_maxCountDown;
    m_progressComponent->SetCountDown(m_countDown, m_maxCountDown);
    startTimer(Layout::GUI_REFRESH_RATE);

    // Register watcher and apply persisted settings now that all members exist.
    m_config->addWatcher(this, false);
    onConfigUpdated();
}

MainComponent::~MainComponent()
{
}

int MainComponent::GetTileSize() const
{
    return m_boardComponent->GetTileSize();
}

void MainComponent::resized()
{
    const int W = getWidth();
    const int H = getHeight();
    const bool portrait = H > W;

    // ---- Fixed zone heights ----
    const int H_header   = std::max(40, H / 10);
    const int H_footer   = std::max(28, H / 18);
    const int H_progress = std::max(50, H / 10);

    // ---- Zone rectangles ----
    juce::Rectangle<int> headerBounds(0, 0, W, H_header);
    juce::Rectangle<int> contentBounds(0, H_header, W, H - H_header - H_progress - H_footer);
    juce::Rectangle<int> progressBounds(0, H - H_footer - H_progress, W, H_progress);
    juce::Rectangle<int> footerBounds(0, H - H_footer, W, H_footer);

    // ---- Progress + Footer ----
    m_progressComponent->setBounds(progressBounds);

    auto versionFont = m_renderer.GetFont(GameRenderer::LABEL_VERSION);
    auto textWidth   = juce::GlyphArrangement::getStringWidthInt(versionFont, m_hyperlink->getButtonText());
    m_hyperlink->setFont(versionFont, false);
    m_hyperlink->setBounds(footerBounds.withSizeKeepingCentre(textWidth + 8, footerBounds.getHeight()));

    // ---- Font ref bounds for GameRenderer ----
    juce::Rectangle<int> fontRefBounds = portrait
        ? juce::Rectangle<int>(0, 0, H, W)   // virtual landscape (swap dims)
        : juce::Rectangle<int>(0, 0, W, H);

    m_renderer.SetLayout(headerBounds, fontRefBounds);

    // ---- Board + Queue placement ----
    const int cW = contentBounds.getWidth();
    const int cH = contentBounds.getHeight();
    const int cY = contentBounds.getY();

    if (!portrait)
    {
        // Landscape: queue is a vertical strip in a left sidebar.
        // Sidebar is sized to snugly wrap one tile column with proportional padding,
        // rather than a fixed window fraction, to reduce empty space around the queue.
        int T = cH / 7;  // start from height bound
        if (T < 1) T = 1;
        const int queuePad   = std::max(8, T / 4);
        const int sidebarW   = T + 2 * queuePad;
        const int boardAreaW = cW - sidebarW;
        T = std::min(T, (boardAreaW + 9) / 10);  // account for 1px tile border overlap
        if (T < 1) T = 1;

        // Exact pixel area for a grid with 1px shared borders: N*T - (N-1)
        const int boardW = 10 * T - 9;
        const int boardH = 7 * T - 6;
        const int boardX = sidebarW + (boardAreaW - boardW) / 2;
        const int boardY = cY + (cH - boardH) / 2;

        m_boardComponent->setBounds(boardX, boardY, boardW, boardH);
        m_boardComponent->setTransform(juce::AffineTransform()); // identity

        const int queueW = T;
        const int queueH = 5 * T - 4;  // exact pixel area for 5-tile column
        const int queueX = (sidebarW - queueW) / 2;
        const int queueY = cY + (cH - queueH) / 2;

        m_queueComponent->setBounds(queueX, queueY, queueW, queueH);
        m_queueComponent->setTransform(juce::AffineTransform());
    }
    else
    {
        // Portrait: board rotated 90° CW (visual 7T × 10T) sits at top of content area.
        // Queue is a horizontal strip (5T × T) immediately below the board.
        // T is chosen so board + queue exactly fill the content height.
        // 10T (board) + gap + T (queue) = cH  →  T = (cH - gap) / 11
        const int gap = 4;
        // Exact visual sizes after rotation: board is (7T-6) wide × (10T-9) tall,
        // queue is (5T-4) wide × T tall.
        // Fit constraint: (10T-9) + gap + T = cH  →  T = (cH - gap + 9) / 11
        // Width constraint: 7T-6 ≤ cW  →  T ≤ (cW + 6) / 7
        int T = std::min((cH - gap + 9) / 11, (cW + 6) / 7);
        if (T < 1) T = 1;

        const int boardVisualW = 7 * T - 6;
        const int boardVisualH = 10 * T - 9;
        const int boardX       = (W - boardVisualW) / 2; // centred horizontally

        // rotation(+π/2).translated(boardVisualW + boardX, cY) maps the component's
        // logical (0, 7T-6) to the visual top-left (boardX, cY). Verified:
        //   (0, 7T-6) →rotate→ (-(7T-6), 0) →translate→ (boardX, cY)  ✓
        m_boardComponent->setBounds(0, 0, 10 * T - 9, 7 * T - 6);
        m_boardComponent->setTransform(
            juce::AffineTransform::rotation(juce::MathConstants<float>::halfPi)
            .translated(static_cast<float>(boardVisualW + boardX), static_cast<float>(cY)));

        const int queueW = 5 * T - 4;  // exact pixel area for 5-tile row
        const int queueH = T;
        const int queueX = (W - queueW) / 2;
        const int queueY = cY + boardVisualH + gap;

        m_queueComponent->setBounds(queueX, queueY, queueW, queueH);
        m_queueComponent->setTransform(juce::AffineTransform());
    }

    // Resize the ScoreWindow if one is visible.
    if (m_scoreWindow)
        m_scoreWindow->resized();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));
    m_renderer.Render(g);
}

void MainComponent::lookAndFeelChanged()
{
    m_hyperlink->setColour(juce::HyperlinkButton::textColourId,
                           findColour(juce::Label::textColourId).withAlpha(0.7f));
    repaint();
}

void MainComponent::timerCallback()
{
    const juce::ScopedLock lock(m_lock);

    Controller* controller(m_controller.get());
    Controller::GameState state(controller->GetState());

    if (state == Controller::STATE_RUNNING)
    {
        m_boardComponent->Tick();

        if (m_countDown > 0)
        {
            if (controller->GetFastForward())
                m_countDown -= 5;
            else
                m_countDown -= 1;
        }
        else
        {
            bool contained = controller->Pump();
            if (!contained)
                startTimer(2000);
        }
    }
    else if (state == Controller::STATE_STOPPED)
    {
        stopTimer();

        Controller::ScoreDetails details(controller->GetScoreDetails());
        juce::Point<int> windowOrigin(0, 0);
        if (details.advance)
            windowOrigin = juce::Point<int>(getLocalBounds().getWidth() / 3, getLocalBounds().getHeight() / 4);

        m_scoreWindow.reset(ScoreWindow::CreateScoreWindow(details));
        m_scoreWindow->addChangeListener(this);
        addAndMakeVisible(m_scoreWindow.get());
        m_scoreWindow->setTopLeftPosition(windowOrigin);
        m_scoreWindow->resized();
    }

    m_progressComponent->SetCountDown(m_countDown, m_maxCountDown);
    repaint();
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    (void)source;

    const juce::ScopedLock lock(m_lock);

    if (m_scoreWindow != nullptr)
    {
        switch (m_scoreWindow->GetCommand())
        {
            case Controller::CMD_RESTART:
            case Controller::CMD_CONTINUE:
                {
                    m_controller->Reset(m_scoreWindow->GetCommand());
                    m_maxCountDown = m_controller->GetCurrentCountdown();
                    m_countDown    = m_maxCountDown;
                    m_boardComponent->ResetInteraction();
                    m_progressComponent->SetCountDown(m_countDown, m_maxCountDown);
                    startTimer(Layout::GUI_REFRESH_RATE);
                }
                break;

            case Controller::CMD_QUIT:
                juce::JUCEApplicationBase::quit();
                break;

            default:
                break;
        }

        m_scoreWindow = nullptr;
    }
}

void MainComponent::performConfigurationDump()
{
    auto* laf = dynamic_cast<JUCEAppBasics::CustomLookAndFeel*>(
        &juce::LookAndFeel::getDefaultLookAndFeel());

    int paletteIndex = 0;
    if (laf)
        paletteIndex = (laf->getPaletteStyle() == JUCEAppBasics::CustomLookAndFeel::PS_Light) ? 1 : 0;

    auto lafXml = std::make_unique<juce::XmlElement>(
        PipeDreamerAppConfiguration::getTagName(PipeDreamerAppConfiguration::LOOKANDFEEL));
    lafXml->addTextElement(juce::String(paletteIndex));
    m_config->setConfigState(std::move(lafXml));

    auto colourXml = std::make_unique<juce::XmlElement>(
        PipeDreamerAppConfiguration::getTagName(PipeDreamerAppConfiguration::HIGHLIGHTCOLOUR));
    colourXml->addTextElement(m_highlightColour.toString());
    m_config->setConfigState(std::move(colourXml));
}

void MainComponent::onConfigUpdated()
{
    auto* laf = dynamic_cast<JUCEAppBasics::CustomLookAndFeel*>(
        &juce::LookAndFeel::getDefaultLookAndFeel());

    auto lafXml = m_config->getConfigState(
        PipeDreamerAppConfiguration::getTagName(PipeDreamerAppConfiguration::LOOKANDFEEL));
    if (lafXml && laf)
    {
        int paletteIndex = lafXml->getAllSubText().getIntValue();
        laf->setPaletteStyle(paletteIndex == 1
            ? JUCEAppBasics::CustomLookAndFeel::PS_Light
            : JUCEAppBasics::CustomLookAndFeel::PS_Dark);
        sendLookAndFeelChange(); // propagates to all child components
    }

    auto colourXml = m_config->getConfigState(
        PipeDreamerAppConfiguration::getTagName(PipeDreamerAppConfiguration::HIGHLIGHTCOLOUR));
    if (colourXml)
        m_highlightColour = juce::Colour::fromString(colourXml->getAllSubText());
}
