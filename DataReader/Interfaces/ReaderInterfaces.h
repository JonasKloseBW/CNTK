#pragma once

#include <vector>
#include <memory>
#include <map>
#include "Sequences.h"
#include "DataTensor.h"

namespace Microsoft { namespace MSR { namespace CNTK {

    // Epoch configuration. Is communicated to the packer, randomizer, bundler.
    struct EpochConfiguration
    {
        size_t workerRank;
        size_t numberOfWorkers;

        size_t minibatchSize;
        size_t totalSize;

        size_t index;
    };

    typedef size_t InputId;

    typedef std::shared_ptr<ImageLayout> SampleLayoutPtr;

    // Input description.
    struct InputDescription
    {
        std::wstring name;
        InputId id;
        SampleLayoutPtr sampleLayout;
        std::string targetLayoutType;
        std::map<std::string, std::string> properties;
    };
    typedef std::shared_ptr<InputDescription> InputDescriptionPtr;


    struct Layout
    {
        MBLayoutPtr columns;
        SampleLayoutPtr rows;
    };

    typedef std::shared_ptr<Layout> LayoutPtr;

    // Input data.
    class Input
    {
        void* data_;
        size_t data_size_;
        LayoutPtr layout_;

    public:
        Input(void* data, size_t dataSize, LayoutPtr layout)
            : data_(data)
            , data_size_(dataSize)
            , layout_(layout)
        {
        }

        const void* GetData() const
        {
            return data_;
        }

        size_t GetDataSize() const
        {
            return data_size_;
        }

        LayoutPtr GetLayout() const
        {
            return layout_;
        }
    };
    typedef std::shared_ptr<Input> InputPtr;

    // Memory provider. Should be used for allocating storage according to the Layout.
    class MemoryProvider
    {
    public:
        virtual void* Alloc(size_t element, size_t numberOfElements) = 0;
        virtual void Free(void* ptr) = 0;
        virtual ~MemoryProvider() = 0 {}
    };
    typedef std::shared_ptr<MemoryProvider> MemoryProviderPtr;

    // Represents a single minibatch.
    struct Minibatch
    {
        bool atEndOfEpoch;
        std::map<size_t /*id from the Input description*/, InputPtr> minibatch;

        operator bool() const
        {
            return !atEndOfEpoch;
        }
    };

    class Epoch
    {
    public:
        virtual Minibatch ReadMinibatch() = 0;
        virtual ~Epoch() = 0 {};
    };
    typedef std::unique_ptr<Epoch> EpochPtr;

    // Main Reader interface. The border interface between the CNTK and Reader.
    class Reader
    {
    public:
        virtual std::vector<InputDescriptionPtr> GetInputs() = 0;
        virtual EpochPtr StartNextEpoch(const EpochConfiguration& config) = 0;
        virtual ~Reader() = 0 {};
    };
    typedef std::unique_ptr<Reader> ReaderPtr;
}}}