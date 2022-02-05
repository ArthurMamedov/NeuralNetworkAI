#include "pch.h"
#include "NeuralNetwork.hpp"
//#include "..\NeuralNetworkAI\CognitiveSystem.hpp"

//constexpr float MUL_CONST = 1000;

//TEST(NeuralNetwork, NeuralNetwork) {
//	CognitiveSystems::Topology topology(3, { 2, 2 }, 1);
//	CognitiveSystems::NeuralNetwork network(topology);
//	
//	auto res1 = network.feedForward({ 1,2,3});
//	for (auto res : res1) {
//		std::cout << res << ' ';
//	} std::cout << std::endl;
//
//	for (int i = 0; i < 1000; i++) {
//		res1 = network.feedForward({ 1,2,3});
//		auto r = network.backPropagation({ 0 }, res1, 0.1);
//	}
//	auto res2 = network.feedForward({ 1,2,3 });
//	for (auto res : res2) {
//		std::cout << res << ' ';
//	} std::cout << std::endl;
//
//}

//TEST(LayerConnection_getOutputs, NEURAL_NETWORK_TESTS) {
//	using namespace std;
//	NeuralNetwork::LayerConnection layer(4, 3);
//	layer.setWeights({ {1.f, 2.f, 3.f},
//					   {4.f, 5.f, 6.f},
//					   {7.f, 8.f, 9.f},
//					   {0.f, 1.f, 2.f} });
//
//	auto output = layer.getOutputs({ 1.f, 2.f, 3.f, 4.f });
//
//	EXPECT_EQ(output.size(), 3); //1.f / (1.f + exp(outputs[i]))
//	EXPECT_EQ(int(output[0] * 1000), 3230);
//	EXPECT_EQ(int(output[1] * 1000), 3862);
//	EXPECT_EQ(int(output[2] * 1000), 3952);
//}
//
TEST(NeuralNetwork_feedForward, NEURAL_NETWORK_TESTS) {
	NeuralNetwork::NeuralNetwork network{4, 3, 3, 2};

	auto resultBefore = network.feedForward({ 1.f, 0.f, 0.f, 1.f });
	for (auto res : resultBefore)
		std::cout << res << ' ';
	std::cout << std::endl;

	auto resultBefore2 = network.feedForward({ 1.f, 1.f, 1.f, 0.f });
	for (auto res : resultBefore2)
		std::cout << res << ' ';
	std::cout << std::endl;

	for (auto t = 0; t < 10; t++) {
		auto output1 = network.feedForward({ 1.f, 0.f, 0.f, 1.f });
		network.backPropagation({ 1.f, 0.f }, 1.0f);
		auto output2 = network.feedForward({ 1.f, 1.f, 1.f, 0.f });
		network.backPropagation({ 0.f, 1.f }, 1.0f);
	}

	auto resultAfter = network.feedForward({ 1.f, 0.f, 0.f, 1.f });
	for (auto res : resultAfter)
		std::cout << res << ' ';
	std::cout << std::endl;
	auto resultAfter2 = network.feedForward({ 1.f, 1.f, 1.f, 0.f });
	for (auto res : resultAfter2)
		std::cout << res << ' ';
	std::cout << std::endl;
	EXPECT_TRUE(true);
}