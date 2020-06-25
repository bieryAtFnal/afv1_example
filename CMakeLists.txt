cmake_minimum_required(VERSION 3.12)
project(afv1_example)

##############################################################################
point_build_to( src )

add_library(afv1_example_ListReverser_duneDAQModule src/ListReverser.cpp)
target_link_libraries(afv1_example_ListReverser_duneDAQModule appfwk)

add_library(afv1_example_RandomDataListGenerator_duneDAQModule src/RandomDataListGenerator.cpp)
target_link_libraries(afv1_example_RandomDataListGenerator_duneDAQModule appfwk)

add_library(afv1_example_ReversedListValidator_duneDAQModule src/ReversedListValidator.cpp)
target_link_libraries(afv1_example_ReversedListValidator_duneDAQModule appfwk)

##############################################################################
point_build_to( test )

file(COPY test/list_reversal_app.json DESTINATION test)
