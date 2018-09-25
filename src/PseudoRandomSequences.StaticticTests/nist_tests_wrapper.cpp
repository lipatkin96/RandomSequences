#include "nist_tests_wrapper.h"

#include <omp.h>
//#include <stdlib.h>

#ifdef __linux__
#include <unistd.h>
#elif(defined _MSC_VER) || (defined __BORLANDC__) || (defined _WIN32)
//#include <WinBase.h>
#endif

namespace statistical_tests_space {

namespace {

IStatisticalTest::TestResultType parsePackOfResults(std::vector<double> const & packOfPValues) {
    const double LOCAL_MEANING_VALUE = 0.1;
    int failureCount = 0;
    for (auto & elem : packOfPValues) {
        failureCount +=
                (parseTestResult(elem) == IStatisticalTest::TestResultType::FAILURE);
    }
    const size_t size = packOfPValues.size();
    //double average = double(successCount) / size; // bad because zero division
    return (size == 0 ? IStatisticalTest::TestResultType::CANCELLED
                     : ((double(failureCount) / size < LOCAL_MEANING_VALUE)
                        ? IStatisticalTest::TestResultType::SUCCESS
                        : IStatisticalTest::TestResultType::FAILURE));
}

}

IStatisticalTest::ReturnValueType
BlockFrequencyTest::test(BoolIterator sequenceIter, size_type size) {
    ReturnValueType container;
    for (auto & param : getTestParameters().blockFrequencyTest)
        container.push_back(parseTestResult(doBlockFrequencyTest(param, size, sequenceIter)));
    return std::move(container);
}

IStatisticalTest::ReturnValueType
NonOverlappingTemplateMatchingsTest::test(BoolIterator sequenceIter, size_type size) {
    ReturnValueType container;
    for (auto & param : getTestParameters().nonOverlappingTemplateMatchingsTest) {
        container.push_back(
                    parsePackOfResults(
                        doNonOverlappingTemplateMatchingsTest(param, size, sequenceIter)));
    }
    return std::move(container);
}

IStatisticalTest::ReturnValueType
OverlappingTemplateMatchingsTest::test(BoolIterator sequenceIter, size_type size) {
    ReturnValueType container;
    for (auto & param : getTestParameters().overlappingTemplateMatchingsTest)
        container.push_back(
                    parseTestResult(
                        doOverlappingTemplateMatchingsTest(param, size, sequenceIter)));
    return std::move(container);
}

IStatisticalTest::ReturnValueType
LinearComplexityTest::test(BoolIterator sequenceIter, size_type size) {
    ReturnValueType container;
    for (auto & param : getTestParameters().linearComplexityTest)
        container.push_back(
                    parseTestResult(
                        doLinearComplexityTest(param, size, sequenceIter)));
    return std::move(container);
}

IStatisticalTest::ReturnValueType
SerialTest::test(BoolIterator sequenceIter, size_type size) {
    ReturnValueType container;
    for (auto & param : getTestParameters().serialTest) {
        auto res = doSerialTest(param, size, sequenceIter);
        container.push_back(parseTestResult(res.first));
        container.push_back(parseTestResult(res.second));
    }
    return std::move(container);
}

IStatisticalTest::ReturnValueType
ApproximateEntropyTest::test(BoolIterator sequenceIter, size_type size) {
    ReturnValueType container;
    for (auto & param : getTestParameters().approximateEntropyTest) {
        // (M + 1) - bit block is used to compare
        container.push_back(
                    parseTestResult(
                        doApproximateEntropyTest(param, size, sequenceIter)));
    }
    return std::move(container);
}

IStatisticalTest::ReturnValueType
CumulativeSumsTest::test(BoolIterator sequenceIter, size_type size) {
    ReturnValueType container;
    auto res = doCumulativeSums(size, sequenceIter);
    container.push_back(parseTestResult(res.first));
    container.push_back(parseTestResult(res.second));
    return std::move(container);
}

IStatisticalTest::ReturnValueType
RandomExcursionsTest::test(BoolIterator sequenceIter, size_type size) {
    return { parsePackOfResults(doRandomExcursionsTest(size, sequenceIter)) };
}

IStatisticalTest::ReturnValueType
RandomExcursionsVariantTest::test(BoolIterator sequenceIter, size_type size) {
    return { parsePackOfResults(doRandomExcursionsVariantTest(size, sequenceIter)) };
}

IStatisticalTest::ReturnValueType
BookStackTest::test(BoolIterator sequenceIter, size_type size) {
    ReturnValueType container;

#ifdef __linux__
    auto pid = getpid();
#elif(defined _MSC_VER) || (defined __BORLANDC__) || (defined _WIN32)
	//auto pid = GetCurrentProcessId();
	int pid = std::rand();
#endif
	
    // TODO: add pid of process to make the file more uniquely
    string inputFile = "bookStackInput" + std::to_string(omp_get_thread_num())
             + "_" + std::to_string(size) + "_" + std::to_string(pid) + ".dat";
    {
        typedef unsigned char BlockReadType;   //when was char instead of u_char nothing to change

        std::ofstream outFile;
        outFile.exceptions ( std::ofstream::failbit | std::ofstream::badbit );
        outFile.open(inputFile, std::ios::trunc);
        auto outIter = std::ostream_iterator<BlockReadType>(outFile);
        int bitPos = 0;
        char buffer = 0;
        auto iter = sequenceIter;
        for (size_t i = 0; i < size; i++) {
            buffer |= (*(iter++)) << (bitPos++);
            if (bitPos >= 8) {
                bitPos = 0;
                *(outIter++) = buffer;
                buffer = 0;
            }
        }
        if (bitPos > 0)
            *(outIter) = buffer;
        outFile.close();
    }

    // ! Each bit means 0 or 1 (you can't pass to bookStackTest 0 or 1 in whole byte for example)
    for (auto param : getTestParameters().bookStackTest) {
        string sizeStr = std::to_string(size);
        string dimStr = std::to_string(param.dimension);
        string upperPartStr = std::to_string(param.upperPart);
        string filename = inputFile;
        std::vector<const char *> arguments{ "bs.exe",
            "-f", filename.c_str(),
            "-n", sizeStr.c_str(),	// file size (in bits)
            "-w", dimStr.c_str(),				// word size (or alphabet symbol length (see yourself book stack version)
            //"-b", "0",				// blank between words
            "-u", upperPartStr.c_str()				// size of upper part book stack
        };
        if (param.upperPart > (1LL << 28))
            continue;
        container.push_back(
                    parseTestResult(
                        doBookStackTest(static_cast<int>(arguments.size()), &arguments[0])));
    }
    std::remove(inputFile.c_str());

    return std::move(container);
}

//----------------------Test parameters----------------------//

TestParameters::TestParameters()
    : blockFrequencyTest({ 2, //EPSILON_SIZE / 4, EPSILON_SIZE / 2,
                         //16, 32, 128
                         10
                         }),
      nonOverlappingTemplateMatchingsTest({ 3, 5, 7, //14
                                          }),                    //slow test!!!!
      overlappingTemplateMatchingsTest({ 2, 6, 12 })
{
//    EPSILON_SIZE++;
    for (int upperPart = 0; upperPart < 3; upperPart++) {
        for (size_type dim = 8; dim <= 32; dim *= 2) {	//8, 16, 32
            size_type upperPartSize = (upperPart == 0) ? 16LL
                : ((upperPart == 1) ? (1LL << (dim / 2)) : (1LL << (dim - 1)));
            if (upperPartSize > (1LL << 28))
                continue;
            bookStackTest.push_back({ upperPartSize, dim });
        }
    }

//    auto sqrtSize = uint64_t(std::floor(std::pow(EPSILON_SIZE, 0.5)));
//    auto sqrtSqrtSize = uint64_t(std::floor(std::pow(EPSILON_SIZE, 0.25)));
    // param must be [500; 5000]; EpsilonSize >= 10^6
    linearComplexityTest = { 8, 10, //sqrtSqrtSize
                             12, 14 //1000 //is checked on linux
                           //  sqrtSize
                           };                   //slow test!!!!
//    uint64_t logSize = uint64_t(std::floor(std::log2(EPSILON_SIZE)) - 2);
    //Choose m and n such that m < (log2 n) - 2.
    serialTest = { 2, 3, //logSize / 2, logSize
                 5, 11};
//    uint64_t logSize2 = uint64_t(std::floor(std::log2(EPSILON_SIZE)) - 5);
    approximateEntropyTest = { 1, 2, //logSize2 / 2, logSize2
                             4, 8};
    // upper part - dimension
    orderTest = { {8, 4} };
}

}
