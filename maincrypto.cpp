
#include "book_stack_test.h"
#include "discrete_fourier_transform_test.h"
#include "sequence_converter.h"
#include "statisticChiSquared.h"

#include <iterator>
#include <bitset>
#include <string>
#include <array>

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <iomanip>
#include <limits>
#include <cmath>
#include <chrono>

#include "statTests\include\stat_fncs.h"

using namespace PseudoRandomSequences;
using namespace std::chrono;

//TODO: try to use GoogleTests

typedef std::vector<bool> Sequence;
//typedef MatrixRandomConverter<Sequence> VectorMatrixRandomGenerator;

#include <boost/math/distributions/inverse_chi_squared.hpp>

int main(int argc, char *argv[]) {
	time_t t;
	std::srand(unsigned int(std::time(&t)));
	using std::string;
	using std::vector;
	using std::cout;
	using std::endl;

	//double possibility = boost::math::cdf(boost::math::chi_squared_distribution<double>(n), alpha);
	//cout << "Alpha = " << possibility << endl;
	///*double chi2 = boost::math::pdf(boost::math::inverse_chi_squared_distribution<double>(n), alpha);
	//cout << "Chi2 = " << chi2 << endl;*/

	if (argc < 5 || std::strlen(argv[3]) < 5) {
		cout << "Not enough parameters ( matrix dimension, sequence size, testKey, input possibility)" 
			<< endl;
		return -1;
	}

	// TODO: add test performance for conversation and tests

	uint32_t dimension = uint32_t(boost::lexical_cast<double>(argv[1]));
	MatrixRandomConverter<> converter(dimension);

	Sequence seq(size_t(boost::lexical_cast<double>(argv[2])));
	// TODO: bad computation of input possibility (use another input format)
	int inputOppositePossibility = static_cast<int>(
		std::round(1.0 / boost::lexical_cast<double>(argv[4]))
	);
	std::generate(seq.begin(), seq.end(), 
		[&inputOppositePossibility]() -> bool {
			return (rand() % inputOppositePossibility == 0);
	});
	

	Sequence result(seq.size());
	//steady_clock::time_point start = steady_clock::now();
	volatile clock_t start = clock();
	converter.converse(result, seq);
	cout << " Conversation time: " << (clock() - start + 0.) / (CLOCKS_PER_SEC / 1000.)
		//<< " rows = " << converter.rows() 
		<< endl;

	std::ofstream outFile;
	outFile.open(argv[5], std::ios::out | std::ios::trunc);
	std::copy(result.begin(), result.end(), std::ostream_iterator<bool>(outFile, ""));
	outFile.close();
	
	string testKey(argv[3]);
	if (testKey[0] == '1') {
		start = clock();
		cout << "ChiSquared = " << statisticChiSquaredTest(result, dimension) << endl;
		cout << " Time: " << (clock() - start + 0.) / (CLOCKS_PER_SEC / 1000.) << endl;
	}
	if (testKey[1] == '1') {
		start = clock();
		cout << "BookStack stat = " << bookStackTest(result, dimension) << endl;
		cout << " Time: " << (clock() - start) / (CLOCKS_PER_SEC / 1000.) << endl;
		//TBookStack bookStack()
	}
	if (testKey[2] == '1') {
		start = clock();
		cout << "Fourier stat = "; 
		DiscreteFourierTransform(result.size(), result);
		cout << " Time: " << (clock() - start + 0.) / (CLOCKS_PER_SEC / 1000.) << endl;
	}
	if (testKey[3] == '1') {
		start = clock();
		cout << "Matrix stat = ";
		std::swap(epsilon, result);
		Rank(epsilon.size());
		cout << " Time: " << (clock() - start + 0.) / (CLOCKS_PER_SEC / 1000.) << endl;
	}
	// Warning: result haven't already contained current sequence
	if (testKey[4] == '1') {
		start = clock();
		cout << "Matrix stat = ";
		BlockFrequency(2, epsilon.size());
		cout << " Time: " << (clock() - start + 0.) / (CLOCKS_PER_SEC / 1000.) << endl;
	}
	
	

	//-------------------Test Fourier---------------------//

	// 1 Test (0^n)
	//std::fill(seq.begin(), seq.end(), 0);
	// 2 Test (0011)^n
	/*std::generate(seq.begin(), seq.end(), 
		[] () -> bool { 
		static size_t i = 0;
		return ((i++) % 4 < 2);
	});*/
	// 3 Test (std::rand)
	/*std::generate(seq.begin(), seq.end(),
		[]() -> bool {
		return rand() % 2;
	});*/
	// 4 Test
	/*std::generate(seq.begin(), seq.end(),
		[&seq]() -> bool {
		static size_t i = 0;
		return ((i++) % (seq.size() / 4) < seq.size() / 8);
	});*/

	// 5 Test	(110100)^n
	/*std::generate(seq.begin(), seq.end(),
		[&seq]() -> bool {
		static size_t i = 0;
		size_t j = (i++) % 6;
		return (j < 2 || j == 3);
	});*/

	// 6 Test	(1110011000)^n
	//std::generate(seq.begin(), seq.end(),
	//	[&seq]() -> bool {
	//	static size_t i = 0;
	//	size_t j = (i++) % 10;
	//	return (j < 3 || j == 5 || j == 6);
	//});

	// 7 Test	(1 rand() 0)^n
	/*std::generate(seq.begin(), seq.end(),
		[&seq]() -> bool {
		static size_t i = 0;
		size_t j = (i++) % 3;
		return ((j == 1) ? rand() % 2 : ((j < 1) ? 1 : 0));
	});*/

	

	return 0;
}

