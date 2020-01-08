cmake_minimum_required(VERSION 3.10)
project(cherrypick_cpp)

set(CMAKE_CXX_STANDARD 17)

file(GLOB test_SRC
        "*.h"
        "*.cpp"
        )
list(REMOVE_ITEM test_SRC "./main.cpp")
list(FILTER test_SRC EXCLUDE REGEX ".*/main.cpp$")


add_executable(cherrypick_cpp
        cherrypick_semitemporal.h tree.h newick.h cherrypick.h cherrypick_default.h cherrypick_default.cpp cherrypick_nonbinary.h common.h newick.cpp tree.cpp  main.cpp cherrypick.cpp common.cpp
        cherrypick_semitemporal.cpp util.cpp util.h cxxopts.cpp cherrypick_nonbinary.cpp
        greedy_pick.cpp greedy_pick.h  cluster_reduction.cpp cluster_reduction.h)
#target_link_libraries(cherrypick_cpp ${Boost_LIBRARIES})