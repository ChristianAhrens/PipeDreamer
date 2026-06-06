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
#include <map>
#include <memory>
#include <atomic>


// ---- Class Definition ----

/**
 * Manages all game audio: device initialisation, sound loading, and playback.
 * Playback requests are dispatched through a dedicated AudioThread so the
 * game logic thread is never blocked waiting on audio I/O.
 *
 * Owned by Controller via std::unique_ptr.
 */
class AudioManager
{
public:
    /**
     * Game sound identifiers.
     */
    enum SoundID
    {
        SOUND_NONE = 0,
        SOUND_CLICK,
        SOUND_EXPLODE,
        SOUND_NOTIFY,
        SOUND_LEVEL_UP,
        SOUND_GAME_OVER,
        SOUND_MAX
    };

    /**
     * Class constructor. Initialises the audio device and loads all sound resources.
     */
    AudioManager();

    /**
     * Class destructor. Stops the AudioThread and releases audio resources.
     */
    ~AudioManager();

    /**
     * Schedule a sound for playback on the AudioThread.
     * Safe to call from any thread.
     *
     * @param soundID  Sound to play.
     */
    void QueueSound(SoundID soundID);

private:
    /**
     * Dedicated thread for audio playback, keeping audio I/O off the message thread.
     */
    class AudioThread : public juce::Thread
    {
    public:
        /**
         * @param owner  The AudioManager that owns this thread and handles PlaySound().
         */
        explicit AudioThread(AudioManager& owner);

        /** Reimplemented from juce::Thread. */
        void run() override;

        /**
         * Wake the thread and request playback of the given sound.
         *
         * @param soundID  Sound to trigger once the thread is running.
         */
        void QueueSound(SoundID soundID);

    private:
        AudioManager& m_owner;
        std::atomic<SoundID> m_soundID = { SOUND_NONE };
    };

    void Init();
    void Shutdown();

    /**
     * Trigger immediate playback of a sound. Called from within AudioThread::run().
     *
     * @param soundID  Sound to play.
     */
    void PlaySound(SoundID soundID);

    AudioThread m_audioThread;

    juce::AudioDeviceManager  m_audioDeviceManager;
    juce::AudioSourcePlayer   m_audioPlayer;
    juce::MixerAudioSource    m_audioMixer;

    typedef std::unique_ptr<juce::AudioFormatReaderSource> SoundSource;
    std::map<SoundID, SoundSource> m_soundSources;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioManager)
};
