#include "pch.h"
#include "NeuralNetwork.hpp"

float bypasser(float val) {
	return val;
}

TEST(LayerConnection_getOutputs, NEURAL_NETWORK_TESTS) {
	using namespace std;

	NeuralNetwork::NeuralNetwork<bypasser, bypasser>::LayerConnection layer(4, 3);
	layer.setWeights({ {1.f, 4.f, 7.f, 0.f},
					   {2.f, 5.f, 8.f, 1.f},
					   {3.f, 6.f, 9.f, 2.f}});

	layer.setShifts({ 0.f, 0.f, 0.f });

	auto output = layer.getOutputs({ 1.f, 2.f, 3.f, 4.f });

	EXPECT_EQ(output.size(), 3); //1.f / (1.f + exp(outputs[i]))
	EXPECT_EQ(int(output[0]), 30);
	EXPECT_EQ(int(output[1]), 40);
	EXPECT_EQ(int(output[2]), 50);
}

TEST(NeuralNetwork_feedForward, NEURAL_NETWORK_TESTS) {
	NeuralNetwork::NeuralNetwork<NeuralNetwork::ActivationFunctions::tanh,
								 NeuralNetwork::ActivationFunctions::tanhDx> network{4, 3, 4, 2};

	for (auto t = 0; t < 5; t++) {
		network.feedForward({ 1.f, 0.f, 0.f, 1.f });
		network.backPropagation({ 1.f, 0.f }, 0.5f);
		network.feedForward({ 1.f, 1.f, 1.f, 0.f });
		network.backPropagation({ 0.f, 1.f }, 0.5f);
	}

	auto result1 = network.feedForward({ 1.f, 0.f, 0.f, 1.f });
	auto result2 = network.feedForward({ 1.f, 1.f, 1.f, 0.f });

	const auto cmpflt = [](float a, float b) {
		return std::abs(roundf(a) - roundf(b)) < 0.0000001;
	};

	EXPECT_TRUE(std::ranges::equal(result1, std::vector<float>{ 1.f, 0.f }, cmpflt));
	EXPECT_TRUE(std::ranges::equal(result2, std::vector<float>{ 0.f, 1.f }, cmpflt));
}