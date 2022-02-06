#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <numeric>
#include <algorithm>

namespace NeuralNetwork {
	using ActivationFunction = float(float);

	using Weight = float;
	using Signal = float;
	using Error = float;
	using Shift = float;

	using Weights = std::vector<std::vector<Weight>>;
	using Signals = std::vector<Signal>;
	using Errors = std::vector<Error>;
	using Shifts = std::vector<Shift>;

	namespace ActivationFunctions {
		inline float tanh(const float x) {
			return std::tanhf(x);
		}

		inline float tanhDx(const float x) {
			float tanhVal = std::tanhf(x);
			return 1.f - std::powf(tanhVal, 2);
		}

		inline float sigm(const float x) {
			return 1.f / (1.f + std::exp(-x));
		}

		inline float sigmDx(const float x) {
			float sigmVal = sigm(x);
			return sigmVal * (1.f - sigmVal);
		}

		inline float atan(const float x) {
			return atanf(x);
		}

		inline float atanDx(const float x) {
			return 1.f / (x * x + 1.f);
		}
 	}

	template<ActivationFunction actFunc, ActivationFunction actFuncDx>
	class NeuralNetwork {
	public:
		class LayerConnection;

	public:
		NeuralNetwork(std::initializer_list<int> neuronNumbersInLayers);
		Signals feedForward(Signals signals);
		void backPropagation(const Signals& expected, const float learningRate);

	public:
		class LayerConnection {
		public:
			LayerConnection(const int neuronsOnThisLayer, const int neuronsOnNextLayer);
			const Signals& getOutputs(const Signals& inputSignals) const;

#ifdef TEST
			void setWeights(const Weights& weights) {
				LayerConnection::weights = weights;
				assert(nextLayerNeuronNumber == weights.size());
				for (const auto& w : weights)
					assert(w.size() == neuronNumber);
			}

			void setShifts(const Shifts& shifts) {
				assert(shifts.size() == nextLayerNeuronNumber);
				LayerConnection::shifts = shifts;
			}
#endif

		private:
			friend class NeuralNetwork;

			const int neuronNumber;
			const int nextLayerNeuronNumber;
			Weights weights;
			mutable Signals inputs;
			mutable Signals outputs;
			mutable Signals beforeActivation;
			mutable Shifts shifts;
		};

	private:
		void setCurrentError(const Errors& previousErrors, const Signals& beforeActivation, Errors& currentErrors);
		void calculateErrorsForNextLayer(const Errors& currentErrors, LayerConnection& layer, Errors& result);
		void learnWeights(const Errors& errors, const float learningRate, LayerConnection& layer);
		void learnShifts(Shifts& shifts, const Errors& errors, const float learningRate);

	private:
		std::vector<LayerConnection> layerConnections;
	};

	template<ActivationFunction actFunc, ActivationFunction actFuncDx>
	inline void NeuralNetwork<actFunc, actFuncDx>::backPropagation(const Signals& expected, const float learningRate) {
			assert(expected.size() == layerConnections.back().outputs.size());
			Errors currentErrors, previousErrors;
			std::ranges::transform(expected, layerConnections.back().outputs, std::back_inserter(previousErrors), [](Signal s1, Signal s2) { return s1 - s2; });

			for (auto layer = layerConnections.rbegin(); layer != layerConnections.rend(); ++layer) {
				setCurrentError(previousErrors, layer->beforeActivation, currentErrors);
				calculateErrorsForNextLayer(currentErrors, *layer, previousErrors);
				learnWeights(currentErrors, learningRate, *layer);
				learnShifts(layer->shifts, currentErrors, learningRate);
			}
		}


	template<ActivationFunction actFunc, ActivationFunction actFuncDx>
	inline NeuralNetwork<actFunc, actFuncDx>::LayerConnection::LayerConnection(const int neuronsOnThisLayer, const int neuronsOnNextLayer) :
		neuronNumber(neuronsOnThisLayer),
		nextLayerNeuronNumber(neuronsOnNextLayer),
		inputs(neuronsOnThisLayer, 0.f),
		outputs(neuronsOnThisLayer, 0.f),
		shifts(neuronsOnNextLayer, 0.f),
		weights(neuronsOnNextLayer, std::vector<Weight>(neuronsOnThisLayer, 0.f)) {

		const auto randFloat = [](float) {
			return (static_cast<float>(rand() % 1000) - 500.f) / 1000.f;
		};

		for (auto& ws : weights) {
			std::ranges::transform(ws, ws.begin(), randFloat);
		}
		std::ranges::transform(shifts, shifts.begin(), randFloat);
	}


	template<ActivationFunction actFunc, ActivationFunction actFuncDx>
	inline const Signals& NeuralNetwork<actFunc, actFuncDx>::LayerConnection::getOutputs(const Signals& inputSignals) const {
		if (inputSignals.size() != neuronNumber)
			throw std::runtime_error("Input signals count is not equal to the neurons number in the layer.");

		inputs = inputSignals;
		outputs.resize(nextLayerNeuronNumber, 0.f);
		std::ranges::fill(outputs, 0.f);
		beforeActivation.resize(nextLayerNeuronNumber, 0.f);

		for (int i = 0; i < nextLayerNeuronNumber; ++i) {
			beforeActivation[i] = std::inner_product(weights[i].begin(), weights[i].end(), inputs.begin(), shifts[i]);
			outputs[i] = actFunc(beforeActivation[i]);
		}

		return outputs;
	}

	template<ActivationFunction actFunc, ActivationFunction actFuncDx>
	inline void NeuralNetwork<actFunc, actFuncDx>::setCurrentError(const Errors& previousErrors, const Signals& beforeActivation, Errors& currentErrors) {
		assert(previousErrors.size() == beforeActivation.size());

		const auto action = [](const Error& prevError, const Signal& beforeActivation) {
			return prevError * actFuncDx(beforeActivation);
		};

		currentErrors.resize(previousErrors.size());
		std::ranges::transform(previousErrors, beforeActivation, currentErrors.begin(), action);
	}


	template<ActivationFunction actFunc, ActivationFunction actFuncDx>
	inline void NeuralNetwork<actFunc, actFuncDx>::calculateErrorsForNextLayer(const Errors& currentErrors, LayerConnection& layer, Errors& result) {
		result.resize(layer.inputs.size());
		std::ranges::fill(result, 0.f);
		for (int i = 0; i < layer.inputs.size(); ++i) {
			for (int j = 0; j < layer.outputs.size(); ++j) {
				result[i] += currentErrors[j] * layer.weights[j][i];
			}
		}
	}

	template<ActivationFunction actFunc, ActivationFunction actFuncDx>
	inline void NeuralNetwork<actFunc, actFuncDx>::learnWeights(const Errors& errors, const float learningRate, LayerConnection& layer) {
		for (int i = 0; i < layer.inputs.size(); ++i) {
			for (int j = 0; j < layer.outputs.size(); ++j) {
				layer.weights[j][i] += learningRate * errors[j] * layer.inputs[i];
			}
		}
	}

	template<ActivationFunction actFunc, ActivationFunction actFuncDx>
	inline void NeuralNetwork<actFunc, actFuncDx>::learnShifts(Shifts& shifts, const Errors& errors, const float learningRate) {
		assert(shifts.size() == errors.size());
		const auto action = [&learningRate](const Shift& shift, const Error& error) {
			return shift + (error * learningRate);
		};
		std::ranges::transform(shifts, errors, shifts.begin(), action);
	}

	template<ActivationFunction actFunc, ActivationFunction actFuncDx>
	inline NeuralNetwork<actFunc, actFuncDx>::NeuralNetwork(std::initializer_list<int> neuronNumbersInLayers) {
		auto beg = neuronNumbersInLayers.begin();
		const auto end = neuronNumbersInLayers.end() - 1;
		layerConnections.reserve(neuronNumbersInLayers.size() - 1);
		for (; beg != end; ++beg) {
			layerConnections.emplace_back(*beg, *(beg + 1));
		}
	}

	template<ActivationFunction actFunc, ActivationFunction actFuncDx>
	inline Signals NeuralNetwork<actFunc, actFuncDx>::feedForward(Signals signals) {
		for (auto& layerConnection : layerConnections) {
			signals = layerConnection.getOutputs(signals);
		}
		return signals;
	}
}