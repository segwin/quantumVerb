/*
  ==============================================================================

    Task.h

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace reverb
{

    //==============================================================================
    /**
     * Abstract task object used to represent various processing elements
     */
    class Task
    {
    public:
        //==============================================================================
        Task(juce::AudioProcessor * processor) : processor(processor) {}

        //==============================================================================
        using Ptr = std::shared_ptr<Task>;

        //==============================================================================
        /**
         * @brief Updates parameters from processor parameter tree
         *
         * @param [in] params   Processor parameter tree
         * @param [in] blockId  ID of block whose paramters should be checked
         */
        virtual void updateParams(const juce::AudioProcessorValueTreeState& params,
                                  const juce::String& blockId) = 0;

        /**
         * @brief Apply block logic to input buffer
         */
        virtual void exec(juce::AudioSampleBuffer&) = 0;
        
        /**
         * @brief Tells caller whether block must be run for current block
         *
         * May be overriden by IR blocks since these are executed sparingly.
         */
        virtual bool needsToRun() const { return mustExec; }

    protected:
        //==============================================================================
        /**
         * @brief Internal method used to get (and check) a parameter's value
         */
        float getParam(const juce::AudioProcessorValueTreeState& params,
                       const juce::String& blockId) const
        {
            float* paramPtr = params.getRawParameterValue(blockId);
        
            if (!paramPtr)
            {
                throw std::invalid_argument("Parameter not found: " + blockId.toStdString());
            }

            return *paramPtr;
        }

        //==============================================================================
        juce::AudioProcessor * processor;

        //==============================================================================
        bool mustExec = true;
    };

}
