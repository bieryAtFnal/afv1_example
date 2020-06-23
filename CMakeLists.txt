cmake_minimum_required(VERSION 3.12)
project(KurtTest)

find_package(cetlib REQUIRED)
find_package(folly REQUIRED)

set(DAQ_LIBRARIES_PACKAGE ${CETLIB} ${CETLIB_EXCEPT} appfwk)

##############################################################################
point_build_to( src )

add_library(KurtTest_ListReverser_duneDAQModule src/ListReverser.cpp)
target_link_libraries(KurtTest_ListReverser_duneDAQModule Folly::folly)

add_library(KurtTest_RandomDataListGenerator_duneDAQModule src/RandomDataListGenerator.cpp)
target_link_libraries(KurtTest_RandomDataListGenerator_duneDAQModule Folly::folly)

add_library(KurtTest_ReversedListValidator_duneDAQModule src/ReversedListValidator.cpp)
target_link_libraries(KurtTest_ReversedListValidator_duneDAQModule Folly::folly)

file(COPY src/list_reversal_app.json DESTINATION src)
