cmake_minimum_required(VERSION 3.12)
project(afv1_example)

find_package(cetlib REQUIRED)
find_package(folly REQUIRED)

set(DAQ_LIBRARIES_PACKAGE ${CETLIB} ${CETLIB_EXCEPT} appfwk)

##############################################################################
point_build_to( src )

add_library(afv1_example_ListReverser_duneDAQModule src/ListReverser.cpp)
target_link_libraries(afv1_example_ListReverser_duneDAQModule Folly::folly)

add_library(afv1_example_RandomDataListGenerator_duneDAQModule src/RandomDataListGenerator.cpp)
target_link_libraries(afv1_example_RandomDataListGenerator_duneDAQModule Folly::folly)

add_library(afv1_example_ReversedListValidator_duneDAQModule src/ReversedListValidator.cpp)
target_link_libraries(afv1_example_ReversedListValidator_duneDAQModule Folly::folly)

##############################################################################
point_build_to( test )

file(COPY test/list_reversal_app.json DESTINATION test)
