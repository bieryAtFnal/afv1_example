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

#ifndef AFV1_EXAMPLE_SRC_COMMONISSUES_HPP_
#define AFV1_EXAMPLE_SRC_COMMONISSUES_HPP_

#include "appfwk/DAQModule.hpp"
#include <ers/Issue.h>

namespace dunedaq {

ERS_DECLARE_ISSUE_BASE(afv1_example,
                       ProgressUpdate,
                       appfwk::GeneralDAQModuleIssue,
                       message,
                       ((std::string)name),
                       ((std::string)message))

ERS_DECLARE_ISSUE_BASE(afv1_example,
                       InvalidQueueFatalError,
                       appfwk::GeneralDAQModuleIssue,
                       "The " << queueType << " queue was not successfully created.",
                       ((std::string)name),
                       ((std::string)queueType))

} // namespace dunedaq

#endif // AFV1_EXAMPLE_SRC_COMMONISSUES_HPP_
