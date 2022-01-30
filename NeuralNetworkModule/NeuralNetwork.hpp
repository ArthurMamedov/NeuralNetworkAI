#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <numeric>
#include <algorithm>

template<class T>
std::vector<T> operator-(const std::vector<T>& v1, const std::vector<T>& v2) {
	assert(v1.size() == v2.size());
	std::vector<T> result;
	result.reserve(std::min(v1.size(), v2.size()));
	std::transform(v1.begin(), v1.end(), v2.begin(), std::back_inserter(result), [](const T& t1, const T& t2) {
		return t1 - t2;
	});
	return result;
}

template<class T>
std::vector<T> operator-(const std::vector<T>& v1, const T& val) {
	std::vector<T> result;
	result.reserve(v1.size());
	std::transform(v1.begin(), v1.end(), std::back_inserter(result), [&val](const T& t1) {
		return t1 - val;
	});
	return result;
}

template<class T>
std::vector<T> operator*(const std::vector<T>& v1, const std::vector<T>& v2) {
	assert(v1.size() == v2.size());
	std::vector<T> result;
	result.reserve(std::min(v1.size(), v2.size()));
	std::transform(v1.begin(), v1.end(), v2.begin(), std::back_inserter(result), [](const T& t1, const T& t2) {
		return t1 * t2;
	});
	return result;
}

template<class T>
std::vector<T> operator*(const std::vector<T>& v1, const T& val) {
	std::vector<T> result;
	result.reserve(v1.size());
	std::transform(v1.begin(), v1.end(), std::back_inserter(result), [&val](const T& t1) {
		return t1 * val;
	});
	return result;
}

namespace NeuralNetwork {
	class NeuralNetwork;

	using Weight = float;
	using Signal = float;
	using Error = float;

	using Weights = std::vector<std::vector<Weight>>;
	using Signals = std::vector<Signal>;
	using Errors = std::vector<std::vector<Error>>;

	inline float sigm(float x) {
		return 1.f / (1.f + std::exp(-x));
	}

	inline float sigmDx(float x) {
		float sigmVal = sigm(x);
		return sigmVal / (1 - sigmVal);
	}

	inline std::vector<float> sigmDx(std::vector<float> xs) {
		for (auto& value : xs) {
			value = sigmDx(value);
		}
		return xs;
	}

	class LayerConnection {
	public:
		LayerConnection(const int neuronsOnThisLayer, const int neuronsOnNextLayer) :
			neuronNumber(neuronsOnThisLayer),
			nextLayerNeuronNumber(neuronsOnNextLayer),
			inputs(neuronsOnThisLayer, 1.f),
			outputs(neuronsOnThisLayer, 1.f),
			weights(neuronsOnThisLayer, std::vector<Weight>(neuronsOnNextLayer, 1.f)) {}

		std::vector<Signal> getOutputs(const std::vector<Signal>& inputSignals) const {
			inputs = inputSignals;
			std::vector<Signal> outputSignals(nextLayerNeuronNumber, 0.f);

			if (inputSignals.size() != neuronNumber)
				throw std::runtime_error("Input signals count is not exual to the neurons number in the layer.");

			for (int i = 0; i < nextLayerNeuronNumber; ++i) {
				for (int j = 0; j < neuronNumber; ++j) {
					outputSignals[i] += sigm(weights[j][i] * inputs[j]);
				}
			}
			LayerConnection::outputs = std::move(outputSignals);
			return outputs;
		}

		const std::vector<Signal>& getInputSignals() const {
			if (inputs.size() == 0)
				throw std::runtime_error("Signals are not set yet.");
			return inputs;
		}

		const std::vector<Signal>& getOutputSiganls() const {
			if (outputs.size() == 0)
				throw std::runtime_error("Outputs are not set yet.");
			return outputs;
		}


#ifdef TEST
		void setWeights(const std::vector<std::vector<Weight>>& weights) {
			LayerConnection::weights = weights;
			assert(neuronNumber == weights.size());
			for (const auto& w : weights)
				assert(w.size() == nextLayerNeuronNumber);
		}
#endif

	private:
		std::vector<std::vector<Weight>>& getWeights() noexcept {
			return weights;
		}

	private:
		friend class NeuralNetwork;

		const int neuronNumber;
		const int nextLayerNeuronNumber;
		std::vector<std::vector<Weight>> weights;
		mutable std::vector<Signal> inputs;
		mutable std::vector<Signal> outputs;
	};

	class NeuralNetwork {
	public:
		NeuralNetwork(std::initializer_list<int> neuronNumbersInLayers) {
			auto beg = neuronNumbersInLayers.begin();
			const auto end = neuronNumbersInLayers.end() - 1;
			layerConnections.reserve(neuronNumbersInLayers.size() - 1);
			for (; beg != end; ++beg) {
				layerConnections.emplace_back(*beg, *(beg + 1));
			}
		}

		std::vector<float> feedForward(std::vector<Signal> signals) {
			for (auto& layerConnection : layerConnections) {
				signals = layerConnection.getOutputs(signals);
			}
			return signals;
		}

		void backPropagation(const std::vector<Signal>& actuals, const std::vector<Signal>& expected, const float learningRate) {
			Errors errors = std::move(teachLastLayer(actuals, expected, learningRate));
			
			for (auto layer = layerConnections.rbegin() + 1; layer != layerConnections.rend(); ++layer) {
				auto deltas = std::move(errors * sigmDx(layer->outputs));
				std::transform(layer->weights.begin(), layer->weights.end(), deltas.begin(), layer->weights.begin(),
				[&layer, learningRate](std::vector<Weight>& neuronWeights, std::vector<float>& deltas) {
					return neuronWeights - layer->inputs * deltas * learningRate;
				});

				errors.clear();
				std::transform(layer->weights.begin(),
							   layer->weights.end(),
							   deltas.begin(),
							   std::back_inserter(errors),
							   [](const std::vector<Weight>& weights, const std::vector<float>& delta) {
								   return weights * delta;
							   });
			}
		}

	private:
		Errors teachLastLayer(const Signals& actual, const Signals& expected, const float learningRate) {
			std::vector<Error> lastLayerErrors = actual - expected;
			auto& lastLayer = layerConnections.back();
			auto deltas = lastLayerErrors * sigmDx(lastLayer.outputs);

			for (int i = 0; i < lastLayer.weights.size(); ++i) {
				lastLayer.weights[i] = std::move(lastLayer.weights[i] - deltas * lastLayer.inputs[i] * learningRate);
			}

			Errors errorsForNextLayer;
			for (auto& newWeights : lastLayer.weights) {
				errorsForNextLayer.push_back(newWeights * deltas);
			}

			return errorsForNextLayer;
		}

	private:

		std::vector<LayerConnection> layerConnections;
	};
}