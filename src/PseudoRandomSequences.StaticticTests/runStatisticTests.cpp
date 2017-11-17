#include "pseudoRandomSequences.h"

#include <iterator>
#include <bitset>
#include <string>
#include <array>
#include <cmath>
#include <random>

#include <iostream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <iomanip>
#include <limits>
#include <cmath>
#include <chrono>

#include "statTests\include\stat_fncs.h"
#include "lipaboyLibrary\src\maths\fixed_precision_number.h"

using namespace PseudoRandomSequences;

void PseudoRandomSequences::runTests(BoolAnyRange epsilon,
	std::vector<std::string> & testNames,
	bool isSaveNames,
	std::vector<double> & testResults,
	std::string const & testKey,
	std::string const & inputFile) 
{
	using std::string;
	using std::cout;
	using std::endl;

	const int EPSILON_SIZE = int(epsilon.size());
	// #Parameterized
	if (testKey[0] == '1') {
		// ! Each bit means 0 or 1 (you can't pass to bookStackTest 0 or 1 in whole byte for example)
		for (int upperPart = 0; upperPart < 3; upperPart++) {
			for (uint64_t dim = 8; dim <= 32; dim *= 2) {	//8, 16, 32
				string sizeStr = std::to_string(EPSILON_SIZE);
				string dimStr = std::to_string(dim);
				uint64_t upperPartSize = (upperPart == 0) ? 16LL
					: ((upperPart == 1) ? (1LL << (dim / 2)) : (1LL << (dim - 1)));
				string upperPartStr = std::to_string(upperPartSize);
				string filename = (inputFile);
				std::vector<const char *> arguments{ "bs.exe",
					"-f", filename.c_str(),
					"-n", sizeStr.c_str(),	// file size (in bits)
					"-w", dimStr.c_str(),				// word size (or alphabet symbol length (see yourself book stack version)
					//"-b", "0",				// blank between words
					"-u", upperPartStr.c_str()				// size of upper part book stack
				};
				if (upperPartSize > (1LL << 28))
					continue;
				//std::cout << "up=" << upperPartStr << ", dim = " << dimStr << std::endl;
				if (isSaveNames) testNames.push_back("BookStackTest_" + dimStr + "_" + upperPartStr);
				testResults.push_back(bookStackTestMain(int(arguments.size()), &arguments[0]));
			}
		}
	}
	if (testKey[1] == '1') {
		auto start = clock();
		if (isSaveNames) testNames.push_back("Frequency");
		testResults.push_back(Frequency(EPSILON_SIZE, epsilon));
		//cout << "Time: " << clock() - start << endl;
	}
	// #Parameterized
	if (testKey[2] == '1') {
		auto start = clock();
		const int min = 2, avg = EPSILON_SIZE / 4, max = EPSILON_SIZE / 2;
		//doesn't equal frequency monobit with M = 1
		if (isSaveNames) testNames.push_back("BlockFrequency_" + std::to_string(min));
		testResults.push_back(BlockFrequency(min, EPSILON_SIZE, epsilon));
		if (isSaveNames) testNames.push_back("BlockFrequency_" + std::to_string(avg));
		testResults.push_back(BlockFrequency(avg, EPSILON_SIZE, epsilon));
		if (isSaveNames) testNames.push_back("BlockFrequency_" + std::to_string(max));
		testResults.push_back(BlockFrequency(max, EPSILON_SIZE, epsilon));
		//cout << "Time: " << clock() - start << endl;
	}
	if (testKey[3] == '1') {
		auto start = clock();
		if (isSaveNames) testNames.push_back("Runs");
		testResults.push_back(Runs(EPSILON_SIZE, epsilon));
		//cout << "Time: " << clock() - start << endl;
	}
	if (testKey[4] == '1') {
		auto start = clock();
		if (isSaveNames) testNames.push_back("LongestRunOfOnes");
		testResults.push_back(LongestRunOfOnes(EPSILON_SIZE, epsilon));
		//cout << "Time: " << clock() - start << endl;
	}
	if (testKey[5] == '1') {
		auto start = clock();
		if (isSaveNames) testNames.push_back("Rank");
		testResults.push_back(Rank(EPSILON_SIZE, epsilon));
		//cout << "Time: " << clock() - start << endl;
	}
	if (testKey[6] == '1') {
		auto start = clock();
		// Has a little difference between results of my own discreteFourier Test version
		if (isSaveNames) testNames.push_back("DiscreteFourierTransform");
		testResults.push_back(DiscreteFourierTransform(EPSILON_SIZE, epsilon));
		cout << "Time: " << clock() - start << endl;
	}
	// #Slow test
	// #Parameterized
	if (testKey[7] == '1') {	// TODO: need to check
		auto start = clock();
		//2 - is minimum (depends on existing files)
		std::vector<int> blockSizes { 3 };
		// from 2 to 16
		for (auto param : blockSizes) {
			if (isSaveNames) testNames.push_back("NonOverlappingTemplateMatchings_" 
				+ std::to_string(param));
			std::vector<double> temp;
			temp = NonOverlappingTemplateMatchings(param, EPSILON_SIZE, epsilon);
			double average = 0.;
			for (auto elem : temp) {
				average += (elem >= ALPHA);
			}
			const size_t size = temp.size();
			average /= size;
			// TODO: here
			testResults.push_back(size == 0 ? -1.
				: average + size * (ALPHA - (size - 1.) / size + 1e-3) * (1. - average));
		}
		cout << "Time: " << clock() - start << endl;
	}
	// #Parameterized
	if (testKey[8] == '1') {
		auto start = clock();
		std::vector<int> blockSizes { 2, EPSILON_SIZE / 2, EPSILON_SIZE };		// TODO: CHECK
		for (auto param : blockSizes) {
			if (isSaveNames) testNames.push_back("OverlappingTemplateMatchings_" + std::to_string(param));
			testResults.push_back(OverlappingTemplateMatchings(param, EPSILON_SIZE, epsilon));
		}
		cout << "Time: " << clock() - start << endl;
	}
	if (testKey[9] == '1') {
		auto start = clock();
		if (EPSILON_SIZE >= 387840) {
			if (isSaveNames) testNames.push_back("Universal");
			testResults.push_back(Universal(EPSILON_SIZE, epsilon));
		}
		//cout << "Time: " << clock() - start << endl;
	}
	// #Parameterized
	//if (testKey[10] == '1') {		// think: neccessary try all the variant of blockSize (read documentation of test)
	//	int sqrtSize = int(std::floor(std::sqrt(EPSILON_SIZE)));
	//	std::vector<int> blockSizes{ 8, sqrtSize / 2, sqrtSize};	// must be > 3
	//	for (auto param : blockSizes) {
	//		if (isSaveNames) testNames.push_back("LinearComplexity_" + std::to_string(param));
	//		testResults.push_back(LinearComplexity(param, EPSILON_SIZE, epsilon));
	//	}
	//}
	// #Slow
	// #Parameterized
	if (testKey[11] == '1') {// think: neccessary try all the variant of blockSize (read documentation of test)
		auto start = clock();
		int logSize = int(std::floor(std::log2(EPSILON_SIZE)) - 2);
		std::vector<int> blockSizes{ 3};
		for (auto param : blockSizes) {
			auto res = Serial(param, EPSILON_SIZE, epsilon);
			if (isSaveNames) testNames.push_back("Serial_" + std::to_string(param) + "_1");
			testResults.push_back(res.first);
			if (isSaveNames) testNames.push_back("Serial_" + std::to_string(param) + "_2");
			testResults.push_back(res.second);
		}
		cout << "Time: " << clock() - start << endl;
	}
	// #Slow
	// #Parameterized
	if (testKey[12] == '1') {// think: neccessary try all the variant of blockSize (read documentation of test)
		auto start = clock();
		int logSize = int(std::floor(std::log2(EPSILON_SIZE)) - 5);
		std::vector<int> blockSizes{ 1 };
		for (auto param : blockSizes) {
			if (isSaveNames) testNames.push_back("ApproximateEntropy_" + std::to_string(param));
			// (M + 1) - bit block is used to compare
			testResults.push_back(ApproximateEntropy(param, EPSILON_SIZE, epsilon));
		}
		cout << "Time: " << clock() - start << endl;
	}
	if (testKey[13] == '1') {
		auto start = clock();
		auto res = CumulativeSums(EPSILON_SIZE, epsilon);
		if (isSaveNames) testNames.push_back("CumulativeSums_1");
		testResults.push_back(res.first);
		if (isSaveNames) testNames.push_back("CumulativeSums_2");
		testResults.push_back(res.second);
		//cout << "Time: " << clock() - start << endl;
	}
	//if (testKey[14] == '1') {
	//	if (isSaveNames) testNames.push_back("RandomExcursions");
	//	auto result = RandomExcursions(EPSILON_SIZE, epsilon);
	//	double average = 0.;
	//	for (auto elem : result) {
	//		average += (elem >= ALPHA);
	//	}
	//	size_t size = result.size();
	//	average /= size;
	//	testResults.push_back(size == 0 ? -1.
	//		: average + size * (ALPHA - (size - 1.) / size + 1e-3) * (1. - average));
	//}
	//if (testKey[15] == '1') {		// For more longer sequences (> 1e6)
	//	if (isSaveNames) testNames.push_back("RandomExcursionsVariant");
	//	auto result = RandomExcursionsVariant(EPSILON_SIZE, epsilon);
	//	double average = 0.;
	//	for (auto elem : result) {
	//		average += (elem >= ALPHA);
	//	}
	//	size_t size = result.size();
	//	average /= size;
	//	testResults.push_back(size == 0 ? -1.
	//		: average + size * (ALPHA - (size - 1.) / size + 1e-3) * (1. - average));
	//}
}
