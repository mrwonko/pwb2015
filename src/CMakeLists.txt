cmake_minimum_required(VERSION 2.6)

project( "Octarine Optimizer" )

# enable C++11 support
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread -O3")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_executable( octarine_optimizer
    "main.cpp"
    "result_manager.hpp" "result_manager.cpp"
    "result.hpp"
    "field.hpp" "field.cpp"
    "parser.hpp" "parser.cpp"
    "step/immediate_score.hpp" "step/immediate_score.cpp"
    "step/resulting_field.hpp" "step/resulting_field.cpp"
    "algorithms.hpp"
)
