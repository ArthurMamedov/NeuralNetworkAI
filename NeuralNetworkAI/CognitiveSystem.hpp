#pragma once
#include <vector>
#include <numeric>
#include <compare>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <numeric>
#include "PositioningSystem.hpp"
#include "Utils.hpp"

namespace CognitiveSystems {
    enum class NeuronType {
        Input, Normal, Output
    };

    class Neuron {
    public:
        Neuron(size_t inputNeuronNumber, NeuronType type)
            : type(type), output(-1.f), delta(-1.f) {
            weights.resize(inputNeuronNumber, 1);
        }

        const std::vector<float>& getWeights() const noexcept {
            return weights;
        }

        float getWeight(size_t index) const noexcept {
            if (index >= weights.size())
                throw std::runtime_error("Weights number is less the index provided");
            return weights[index];
        }

        NeuronType getNeuronType() const noexcept {
            return type;
        }

        void learn(float error, float learningRate) {
            static const auto sigm = [](float x) { return 1.f / (1.f + expf(-x)); };
            if (type == NeuronType::Input)
                return;

            if (weights.size() != inputs.size())
                throw std::runtime_error("width mismatch!");

            delta = error * (sigm(output) / (1.f - sigm(output)));

            for (int i = 0; i < weights.size(); ++i) {
                auto weight = weights[i];
                auto input = inputs[i];
                auto newWeight = weight - input * delta * learningRate;
                weights[i] = newWeight;
            }
        }

        float getOutput() const noexcept {
            return output;
        }

        float getDelta() const {
            return delta;
        }

        float feedForward(const std::vector<float>& inputs) const {
            if (weights.size() != inputs.size())
                throw std::runtime_error("input signals number is no equal to the weights number");
            Neuron::inputs = inputs;

            float sum = std::inner_product(weights.begin(), weights.end(), inputs.begin(), 0.f);

            return output = type == NeuronType::Input? sum : (1 / (1 + exp(-sum)));
        }

    private:
        std::vector<float> weights;
        mutable std::vector<float> inputs;
        NeuronType type;
        float delta;
        mutable float output;
    };

    class Layer {
    public:
        using LayerIterator = std::vector<Neuron>::iterator;
        using ReverseLayerIterator = std::vector<Neuron>::reverse_iterator;

    public:
        Layer(const int neuronsInLayer, const int eachNeuronsInputsNumber, NeuronType layerNeuronsType)
            : type(layerNeuronsType) {
            neurons.resize(neuronsInLayer, Neuron(eachNeuronsInputsNumber, layerNeuronsType));
        }

        const std::vector<Neuron>& getNeurons() const noexcept {
            return neurons;
        }

        std::vector<Neuron>& getModifiableNeurons() noexcept {
            return neurons;
        }

        Neuron& getNeuron(const size_t index) noexcept {
            if (neurons.size() <= index) {
                throw std::runtime_error("Neurons number in layer is less than index provided");
            }
            return neurons[index];
        }

        int getSize() const noexcept {
            return static_cast<int>(neurons.size());
        }

        LayerIterator begin() noexcept {
            return neurons.begin();
        }

        LayerIterator end() noexcept {
            return neurons.end();
        }

        ReverseLayerIterator rbegin()noexcept {
            return neurons.rbegin();
        }

        ReverseLayerIterator rend() noexcept {
            return neurons.rend();
        }

        std::vector<float> getSignals() const noexcept {
            std::vector<float> signals;
            for (auto& neuron : neurons) {
                signals.push_back(neuron.getOutput());
            }
            return signals;
        }

    private:
        std::vector<Neuron> neurons;
        NeuronType type;
    };

    class Topology {
    public:
        Topology(int inputNeuronsNumber, std::initializer_list<int> hiddenLayersNeuronsNumbers, int outputNeuronsNumber)
            : inputNumber(inputNeuronsNumber), hiddenLayers(hiddenLayersNeuronsNumbers), outputNumber(outputNeuronsNumber) {}

        Topology(int inputNeuronsNumber, const std::vector<int>& hiddenLayersNeuronsNumeber, int outputNeuronsNumber)
            : inputNumber(inputNeuronsNumber), hiddenLayers(hiddenLayers), outputNumber(outputNeuronsNumber) {}

        Topology(int inputNeuronsNumber, std::vector<int>&& hiddenLayersNeuronsNumeber, int outputNeuronsNumber)
            : inputNumber(inputNeuronsNumber), hiddenLayers(std::move(hiddenLayers)), outputNumber(outputNeuronsNumber) {}

        int getInputNumber() const noexcept {
            return inputNumber;
        }

        const std::vector<int>& getHiddenLayers() const noexcept {
            return hiddenLayers;
        }

        int getOutputNumber() const noexcept {
            return outputNumber;
        }

    private:
        int inputNumber;
        std::vector<int> hiddenLayers;
        int outputNumber;
    };

    class NeuralNetwork {
    public:
        using DataSet = std::vector<
                            std::tuple<
                                std::vector<float>,
                                std::vector<float>
                            >
                        >;
    
    public:
        NeuralNetwork(const Topology& topology)
            : topology(topology) {
            createInputLayer();
            createHiddenLayers();
            createOutputLayer();
        }

        NeuralNetwork(Topology&& topology)
            : topology(std::move(topology)) {
            createInputLayer();
            createHiddenLayers();
            createOutputLayer();
        }

        const Topology& getTopology() const noexcept {
            return topology;
        }

        const std::vector<Layer>& getLayers() const noexcept {
            return layers;
        }

        std::vector<float> feedForward(const std::vector<float>& inputSignals) {
            auto& inputLayer = layers.front();
            if (inputSignals.size() != inputLayer.getSize())
                throw std::runtime_error("input signals number is not equal to the input layer neurons number");

            std::vector<float> previousLayerSignals(inputLayer.getSize(), 0.f);

            //TODO... Throw to a separate functoins

            std::transform(inputLayer.begin(), inputLayer.end(),
                           inputSignals.begin(), previousLayerSignals.begin(),
                           [](Neuron& neuron, float signal) {
                                return neuron.feedForward({ signal });
                           });

            for (auto layerIter = layers.begin() + 1; layerIter != layers.end(); ++layerIter) {
                auto& layer = *layerIter;
                std::vector<float> currentLayerSignals(layer.getSize(), 0.f);
                std::transform(layer.begin(), layer.end(),
                               currentLayerSignals.begin(),
                               [&previousLayerSignals](Neuron& neuron) {
                                    return neuron.feedForward(previousLayerSignals);
                               });

                previousLayerSignals = std::move(currentLayerSignals);
            }
            
            return previousLayerSignals;
        }

        std::vector<float> backPropagation(const std::vector<float>& expected, const std::vector<float>& actuals, const float learningRate) noexcept {
            //std::vector<float> actuals = feedForward(inputs);
            std::vector<float> diffs(expected.size(), 0.f);
            assert(actuals.size() == expected.size() && expected.size() == diffs.size());
            std::transform(actuals.begin(), actuals.end(), expected.begin(), diffs.begin(), [](const auto& t1, const auto& t2) { return t1 - t2; });
            auto& lastLayerNeurons = layers.back().getModifiableNeurons();

            if (lastLayerNeurons.size() != diffs.size())
                throw std::runtime_error("differences and neuron counts mismatch!");

            for (int i = 0; i < diffs.size(); i++) {
                lastLayerNeurons[i].learn(diffs[i], 0.01);
            }
            
            for (auto layer = layers.rbegin() + 1; layer != layers.rend(); ++layer){
                auto previousLayer = layer - 1;
                for (int i = 0; i < layer->getSize(); ++i) {
                    auto& neuron = layer->getNeuron(i);
                    for (int j = 0; j < previousLayer->getSize(); ++j){
                        auto& previousNeuron = previousLayer->getNeuron(j);
                        auto error = previousNeuron.getWeight(i) * previousNeuron.getDelta();
                        neuron.learn(error, learningRate);
                    }
                }
            }

            std::for_each(diffs.begin(), diffs.end(), [](float flt) {return flt * flt; });
            return diffs;
        }

        std::vector<float> learn(DataSet& dataset, const size_t epoch, const float learningRate) {
            constexpr int expectedVal = 1;
            constexpr int inputs = 0;
            std::vector<float> errors(std::get<expectedVal>(dataset.back()).size(), 0.f);

            for (size_t _ = 0; _ < epoch; ++_) {//_ is a dummy name for no variable
                for (auto& data : dataset) {
                    auto tmpErrors = backPropagation(std::get<expectedVal>(data), std::get<inputs>(data), learningRate);
                    for (int i = 0; i < tmpErrors.size(); ++i) {
                        errors[i] += tmpErrors[i];
                    }
                }
            }

            std::for_each(errors.begin(), errors.end(), [epoch](float flt) {return flt / epoch; });
            return errors;
        }

    private:
        void createInputLayer() {
            layers.emplace_back(topology.getInputNumber(), 1, NeuronType::Input);
        }

        void createOutputLayer() {
            layers.emplace_back(topology.getOutputNumber(),
                                layers.back().getSize(),
                                NeuronType::Output);
        }

        void createHiddenLayers() {
            for (auto neuronsNumber : topology.getHiddenLayers()) {
                layers.emplace_back(neuronsNumber,
                                    layers.back().getSize(),
                                    NeuronType::Normal);
            }
        }

    private:
        Topology topology;
        std::vector<Layer> layers;
    };

    void foo() {
        std::vector<int> f{ 1, 2, 3, 4 };
        Topology top(1, f, 3);
    }
}