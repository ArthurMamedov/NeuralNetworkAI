//#include <iostream>
//#include <algorithm>
//#include <numeric>
//#include <cassert>

//namespace NeuralNetwork {
//
//	using Signal = float;
//	using Error = float;
//	using Weight = float;
//	using std::vector;
//
//	class Neuron {
//	public:
//		const vector<Weight>& getWeights() const noexcept {
//			return weights;
//		}
//
//		void setWeights(const vector<Weight> weights) {
//			Neuron::weights = weights;
//		}
//
//		void setRandomWeights(const int weightsNumber) {
//			weights.resize(weightsNumber);
//			std::for_each(weights.begin(), weights.end(), [](Weight weight) {
//				return rand();
//			});
//		}
//
//		Signal getSignal(const vector<Signal>& inputSignals) {
//			assert(inputSignals.size() == weights.size());
//			return std::inner_product(weights.begin(), weights.end(), inputSignals.begin(), 0.f);
//		}
//
//	private:
//		vector<Weight> weights;
//	};
//
//	class Layer {
//	public:
//		Layer(const int neuronNumber, const int prevLayerNeuronNumber): neurons(neuronNumber) {
//			for (auto& neuron : neurons) {
//				neuron.setRandomWeights(prevLayerNeuronNumber);
//			}
//		}
//
//		std::vector<Signal> getSignals(const vector<Signal>& inputSignals) {
//			std::vector<Signal> outputSignals(neurons.size());
//			std::transform(neurons.begin(), neurons.end(), outputSignals, 
//						   [&inputSignals](Neuron& neuron) { return neuron.getSignal(inputSignals); });
//			return outputSignals;
//		}
//
//	private:
//		vector<Neuron> neurons;
//	};
//
//
//	class NeuralNetwork {
//	public:
//		NeuralNetwork(const vector<int>& layerNeurons) {
//			layers.reserve(layerNeurons.size());
//			int prevLayerNeuronNumber = 1;
//			for (int i = 0; i < layerNeurons.size(); ++i) {
//				layers.emplace_back(layerNeurons[i], prevLayerNeuronNumber);
//				prevLayerNeuronNumber = layerNeurons[i];
//			}
//		}
//
//		std::vector<Signal> feedForward(vector<Signal> signals) {
//			for (auto& layer : layers) {
//				signals = std::move(layer.getSignals(signals));
//			}
//			return signals;
//		}
//
//		void learn(const std::vector<Signal>& expected) {
//			
//		}
//
//	private:
//		vector<Layer> layers;
//	};
//}






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
 	for (int i = 0; i < v1.size(); ++i) {
 		result.push_back(v1[i] * v2[i]);
 	}
 	/*std::transform(v1.begin(), v1.end(), v2.begin(), std::back_inserter(result), [](const T& t1, const T& t2) {
 		return t1 * t2;
 	});*/
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

 template<class T>
 std::vector<std::vector<T>> operator*(std::vector<std::vector<T>> v1, const std::vector<T>& v2) {
 	assert (v1.size() == v2.size());
 	for (int i = 0; i < v1.size(); i++) {
 		v1[i] = std::move(v1[i] * v2[i]);
 	}
 	return v1;
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
 		return sigmVal / (1.f - sigmVal);
 	}

	

 	inline std::vector<float> sigmDx(std::vector<float> xs) {
 		for (auto& value : xs) {
 			value = sigmDx(value);
 		}
 		return xs;
 	}

	inline float tanhDx(const float x) {
		float tanhVal = std::tanhf(x);
		return 1.f - std::powf(tanhVal, 2);
	}

	inline std::vector<float> tanhDx(std::vector<float> xs) {
		for (auto& value : xs) {
			value = tanhDx(value);
		}
        return xs;
	}


 	class LayerConnection {
 	public:
 		LayerConnection(const int neuronsOnThisLayer, const int neuronsOnNextLayer) :
 			neuronNumber(neuronsOnThisLayer),
 			nextLayerNeuronNumber(neuronsOnNextLayer),
 			inputs(neuronsOnThisLayer, 0.f),
 			outputs(neuronsOnThisLayer, 0.f),
 			nuller(neuronsOnNextLayer, 0.f),
 			weights(neuronsOnThisLayer, std::vector<Weight>(neuronsOnNextLayer, 0.f)) {
 			for (auto& ws : weights) {
 				for (auto& weight : ws) {
 					weight = (static_cast<float>(rand() % 1000) - 500.f) / 1000.f;
 				}
 			}

 			for (auto& w : nuller) {
 				w = (static_cast<float>(rand() % 1000) - 500.f) / 1000.f;
 			}
 		}

 		const std::vector<Signal>& getOutputs(const std::vector<Signal>& inputSignals) const {
 			assert(inputSignals.size() == neuronNumber);
 			inputs = inputSignals;
 			outputs = std::move(Signals(nextLayerNeuronNumber, 0.f));
 			outputsBeforeSigm = std::move(Signals(nextLayerNeuronNumber, 0.f));

 			if (inputSignals.size() != neuronNumber)
 				throw std::runtime_error("Input signals count is not exual to the neurons number in the layer.");

 			for (int i = 0; i < nextLayerNeuronNumber; ++i) {
 				for (int j = 0; j < neuronNumber; ++j) {
 					outputsBeforeSigm[i] += weights[j][i] * inputs[j];
 				}
 				outputsBeforeSigm[i] += nuller[i];
 				outputs[i] = tanh(outputsBeforeSigm[i]);
 			}

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
 		mutable std::vector<Signal> outputsBeforeSigm;
 		mutable std::vector<Weight> nuller;
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

 		void backPropagation(const std::vector<Signal>& expected, const float learningRate) {
 			std::vector<Error> errors = std::move(teachLastLayer(expected, learningRate));
			
 			for (auto layer = layerConnections.rbegin() + 1; layer != layerConnections.rend(); ++layer) {
 				const std::vector<Error> innerErrors = std::move(errors * tanhDx(layer->outputsBeforeSigm));

 				errors = std::move(std::vector<Error>(innerErrors.size(), 0.f));
 				for (int i = 0; i < layer->inputs.size(); ++i) {
 					for (int j = 0; j < layer->outputs.size(); ++j) {
 						errors[j] += innerErrors[j] * layer->weights[i][j];
 					}
 				}
				
 				for (int i = 0; i < layer->inputs.size(); ++i) {
 					for (int j = 0; j < layer->outputs.size(); ++j) {
 						layer->weights[i][j] += learningRate * innerErrors[j] * layer->inputs[i];
 					}
 				}
				
 				for (int i = 0; i < layer->outputs.size(); ++i) {
 					layer->nuller[i] += learningRate * innerErrors[i];
 				}
 			}
 		}

 	private:
 		std::vector<Error> teachLastLayer(const Signals& expected, const float learningRate) {
 			auto& lastLayer = layerConnections.back();
 			const std::vector<Error> errors = (expected - lastLayer.outputs) * tanhDx(lastLayer.outputsBeforeSigm);

 			std::vector<Error> outputErrors(lastLayer.inputs.size(), 0.f);
 			for (int i = 0; i < lastLayer.inputs.size(); ++i) {
 				for (int j = 0; j < lastLayer.outputs.size(); ++j) {
 					outputErrors[i] += errors[j] * lastLayer.weights[i][j];
 				}
 			}

 			for (int i = 0; i < lastLayer.inputs.size(); ++i) {
 				for (int j = 0; j < lastLayer.outputs.size(); ++j) {
 					lastLayer.weights[i][j] += learningRate * errors[j] * lastLayer.inputs[i];
 				}
 			}
			
 			for (int i = 0; i < lastLayer.outputs.size(); ++i) {
 				lastLayer.nuller[i] += learningRate * errors[i];
 			}

 			return outputErrors;
 		}

 	private:

 		std::vector<LayerConnection> layerConnections;
 	};
 }