#include <iostream>
#include "tree.h"
#include "newick.h"
#include "cherrypick.h"
#include <chrono>
#include "cxxopts.cpp"
#include "cluster_reduction.h"
#include "cherrypick_semitemporal.h"
#include "util.h"


int run(std::vector<std::string> files, CherryPickAlgorithm method, int timeout, bool benchmark, bool doClusterReduction, bool writeSolution);


int main(int argc, char** argv){

    try {
        cxxopts::Options options("cherrypick_cpp", "Searches special cherry picking sequences for trees");
        options.positional_help("[input files]")
                .show_positional_help();
        options.add_options()
                ("h,help", "help")
                ("m,method", "algorithm (0=default, 1=semitemporal, 2=nonbinary)", cxxopts::value<int>()->default_value("1"))
                ("v,verbose", "print detailed progress info", cxxopts::value<bool>())
                ("t,timeout", "timeout in seconds", cxxopts::value<int>()->default_value("0"))
                ("c,clusters", "disable cluster reduction", cxxopts::value<bool>())
                ("s,solution", "print solution", cxxopts::value<bool>())
                ("input",
                 "input files containing problem instances in newick format",
                 cxxopts::value<std::vector<std::string>>());
        options.parse_positional({"input"});
        auto result = options.parse(argc, argv);
        if (result.count("help")) {
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(0);
        }
        if (result["method"].as<int>() < 0 || result["method"].as<int>() > 2) {
            std::cerr << "Invalid value for method" << std::endl;
            exit(1);
        }
        CherryPickAlgorithm algorithm = static_cast<CherryPickAlgorithm>(result["method"].as<int>());

        run(result["input"].as<std::vector<std::string>>(), algorithm, result["timeout"].as<int>(),
            result["verbose"].as<bool>(), !result["clusters"].as<bool>(), result["solution"].as<bool>());
    }
    catch (const cxxopts::OptionException& e)
    {
        std::cerr << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }
    return 0;
}


void writeVerboseResult(std::string filename, std::tuple<CherryPickResult, int, int> result, std::vector<float> times, float totalTime) {
    std::cout << "Took " << totalTime << " seconds" << std::endl;
    switch (std::get<0>(result)) {
        case CherryPickResult::no_solution:
            std::cout << "No solution" << std::endl;
            break;
        case CherryPickResult::no_solution_for_k:
            std::cout << "No solution for this k: " << std::get<1>(result) << std::endl;
            break;
        case CherryPickResult::timeout:
            std::cout << "Timeout at k=" << std::get<1>(result) << std::endl;
            break;
        case CherryPickResult::success:
            std::cout << "Solution found for k=" << std::get<1>(result) << " and p=" << std::get<2>(result) << std::endl;
            break;
    }
}

void writeBenchmarkLine(std::string filename, std::tuple<CherryPickResult, int, int> result, std::vector<float> times, float totalTime, int instanceNumber) {
    std::string lowerBound = std::to_string(std::get<1>(result));
    std::string value = std::get<0>(result) == CherryPickResult::success ? lowerBound : "null";
    bool did_timeout =std::get<0>(result) == CherryPickResult ::timeout;
    std::cout << filename << "\t" << value << "\t" << totalTime << "\t" << lowerBound <<"\t" << (did_timeout ? "True" : "False") << "\t" << instanceNumber
            << "\t" << std::get<2>(result)
            << std::endl;
}

int run(std::vector<std::string> files, CherryPickAlgorithm method, int timeout, bool verbose, bool doClusterReduction, bool writeSolution) {
    if (!verbose) {
        std::cout << "filename" << "\t" << "value" << "\t" << "running_time" << "\t" << "time_out" << "\t" << "lower_bound" << "\t" << "number_of_clusters" << "\t"  << "temporal_distance" << std::endl;
    }
    int i = 0;
    for (auto &filename: files) {
        std::ifstream t(filename);
        if (!t) {
            std::cerr << "File does not exist " << filename << std::endl;
            return 1;
        }
        if (verbose) {
            if (i!=0)
                std::cout  << std::endl;
            std::cout << "Processing file " << i << "/" << files.size() << ": "  << filename << std::endl;
        }
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::vector<Tree<std::string> *> intrees = parseFile(buffer.str());
        auto trees = labelNumeric(intrees, false);
        auto beforeTime = std::chrono::high_resolution_clock::now();
        auto instances = doClusterReduction ? clusterReduction(trees) : std::vector<std::vector<Tree<tree_leaf>*>>();
        if (!doClusterReduction) {
            instances.push_back(trees);
        }
        auto timeoutTime = timeout > 0 ? beforeTime + std::chrono::duration<int>(timeout) : beforeTime.max();
        std::vector<float> times;

        CherryPickResult resultType = CherryPickResult::success;
        int resultNumber = 0;
        int nonTemporalNumber = 0;
        if (verbose) {
            std::cout << "Reduce to " << instances.size() << " instances" << std::endl;
        }


        for (auto &instance: instances) {
            CherryPickResult partResult;
            int partHybridizationNumber;
            int partTemporalDistance = 0;
            if (method == CherryPickAlgorithm::SemiTemporal) {
                auto res = cherrypickSemiTemporalRepeatedly(instance, method, timeoutTime, verbose);
                partResult = std::get<0>(res);
                partHybridizationNumber = std::get<1>(res);
                partTemporalDistance = std::get<2>(res);
            } else {
                std::vector<float> times;
                auto res = cherrypickRepeatedly(instance, method, 0, timeoutTime, verbose, times);
                partResult = std::get<0>(res);
                partHybridizationNumber = std::get<1>(res);
            }
            resultType = static_cast<CherryPickResult>(resultType | partResult);
            if (resultType == CherryPickResult::no_solution) {
                break;
            }
            resultNumber += partHybridizationNumber;
            nonTemporalNumber += partTemporalDistance;
        }
        auto result = std::make_tuple(resultType, resultNumber, nonTemporalNumber);
        auto totalTime = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - beforeTime).count();
        if (verbose) {
            writeVerboseResult(filename, result, times, totalTime);
        }
        else {
            writeBenchmarkLine(filename, result, times, totalTime, instances.size());
        }
        for (auto &tree: intrees) {
            delete tree;
        }
        for (auto &tree: trees) {
            delete tree;
        }
        i++;
    }



    return 0;
}

