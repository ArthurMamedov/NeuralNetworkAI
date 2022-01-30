#include "pch.h"
#include "NeuralNetwork.hpp"

//constexpr float MUL_CONST = 1000;

TEST(LayerConnection_getOutputs, NEURAL_NETWORK_TESTS) {
	using namespace std;
	NeuralNetwork::LayerConnection layer(4, 3);
	layer.setWeights({ {1.f, 2.f, 3.f},
					   {4.f, 5.f, 6.f},
					   {7.f, 8.f, 9.f},
					   {0.f, 1.f, 2.f} });

	auto output = layer.getOutputs({ 1.f, 2.f, 3.f, 4.f });

	EXPECT_EQ(output.size(), 3); //1.f / (1.f + exp(outputs[i]))
	EXPECT_EQ(int(output[0] * 1000), 3230);
	EXPECT_EQ(int(output[1] * 1000), 3862);
	EXPECT_EQ(int(output[2] * 1000), 3952);
}

TEST(NeuralNetwork_feedForward, NEURAL_NETWORK_TESTS) {
	NeuralNetwork::NeuralNetwork network{3, 2, 2, 1};
	std::vector<NeuralNetwork::Signal> expected { 1.f };
	auto resultBefore = network.feedForward({1,2,3});
	for (auto res : resultBefore)
		std::cout << res << std::endl;
	network.backPropagation(resultBefore, expected , 0.1);
	auto resultAfter = network.feedForward({ 1,2,3 });
	for (auto res : resultAfter)
		std::cout << res << std::endl;
	EXPECT_TRUE(true);
}