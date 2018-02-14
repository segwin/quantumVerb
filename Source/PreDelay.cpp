/*
  ==============================================================================

    PreDelay.cpp
    Created: 14 Feb 2018 3:26:12pm
    Author:  Eric Seguin

  ==============================================================================
*/

#include "PreDelay.h"

#include <algorithm>

namespace reverb
{

    //==============================================================================
    /**
     * @brief Constructs a PreDelay object associated with an AudioProcessor
     *
     * @param [in] processor    Pointer to main processor
     */
    PreDelay::PreDelay(juce::AudioProcessor * processor)
        : Task(processor)
    {
    }

    //==============================================================================
    /**
     * @brief Applies predelay to impulse response
     *
     * Zero-pads beginning of impulse response buffer to match requested predelay.
     *
     * @param [in,out] ir   Impulse response to modify
     *
     * @throws std::invalid_argument
     */
    void PreDelay::exec(juce::AudioSampleBuffer& ir)
    {
        const int numSamplesToAdd = (int)std::ceil(processor->getSampleRate() * (delayMs/1000.0));

        // Ensure IR buffer only contains one channel
        if (ir.getNumChannels() != 1)
        {
            throw std::invalid_argument("Received multi-channel IR in mono task block");
        }

        // Ensure pre-delay value is within valid range
        if (delayMs > MAX_DELAY_MS)
        {
            throw std::invalid_argument("Requested pre-delay is too large (max. 1s)");
        }

        // Create intermediate buffer with prepended zeros
        juce::AudioSampleBuffer irCopy(1, numSamplesToAdd + ir.getNumSamples());

        float * irCopyWritePtr = irCopy.getWritePointer(0);

        for (int i = 0; i < numSamplesToAdd; ++i)
        {
            irCopyWritePtr[i] = 0;
        }

        // Copy IR values to end of intermediate buffer
        auto irReadPtr = ir.getReadPointer(0);
        std::copy( &irReadPtr[0],
                   &irReadPtr[ir.getNumSamples()],
                   &irCopyWritePtr[numSamplesToAdd] );

        // Use move semantics to replace IR buffer with created buffer
        ir = std::move(irCopy);
    }

}
