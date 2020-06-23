/**
 * @file CommonIssues.hpp
 *
 * This file contains the definitions of ERS Issues that are common
 * to two or more of the DAQModules in this package.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef KURTTEST_SRC_COMMONISSUES_HPP_
#define KURTTEST_SRC_COMMONISSUES_HPP_

#include "appfwk/DAQModule.hpp"
#include <ers/Issue.h>

namespace dunedaq {

ERS_DECLARE_ISSUE_BASE(afv1_example,
                       ProgressUpdate,
                       appfwk::GeneralDAQModuleIssue,
                       name << ": " << message,
                       ERS_EMPTY,
                       ((std::string)name)((std::string)message))

ERS_DECLARE_ISSUE_BASE(afv1_example,
                       InvalidQueueFatalError,
                       appfwk::GeneralDAQModuleIssue,
                       name << ": The " << queueType << " queue was not valid when it was needed. Has initialization been successfully completed?",
                       ERS_EMPTY,
                       ((std::string)name)((std::string)queueType))

} // namespace dunedaq

#endif // KURTTEST_SRC_COMMONISSUES_HPP_
