/**
 * @file ReversedListValidator.cpp ReversedListValidator class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "CommonIssues.hpp"
#include "ReversedListValidator.hpp"

#include <ers/ers.h>
#include "TRACE/trace.h"

#include <chrono>
#include <functional>
#include <thread>

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "ReversedListValidator" // NOLINT
#define TLVL_ENTER_EXIT_METHODS 10
#define TLVL_LIST_VALIDATION 15

namespace dunedaq {
namespace afv1_example {

ReversedListValidator::ReversedListValidator(const std::string& name)
  : DAQModule(name)
  , thread_(std::bind(&ReversedListValidator::do_work, this))
  , reversedDataQueue_(nullptr)
  , originalDataQueue_(nullptr)
  , queueTimeout_(100)
{
  register_command("start", &ReversedListValidator::do_start);
  register_command("stop", &ReversedListValidator::do_stop);
}

void
ReversedListValidator::init()
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";

  try
  {
    reversedDataQueue_.reset(new dunedaq::appfwk::DAQSource<std::vector<int>>(get_config()["reversed_data_input"].get<std::string>()));
  }
  catch (const ers::Issue& excpt)
  {
    throw InvalidQueueFatalError(ERS_HERE, get_name(), "reversed data input", excpt);
  }

  try
  {
    originalDataQueue_.reset(new dunedaq::appfwk::DAQSource<std::vector<int>>(get_config()["original_data_input"].get<std::string>()));
  }
  catch (const ers::Issue& excpt)
  {
    throw InvalidQueueFatalError(ERS_HERE, get_name(), "original data input", excpt);
  }

  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
}

void
ReversedListValidator::do_start([[maybe_unused]] const std::vector<std::string>& args)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
  thread_.start_working_thread();
  ERS_LOG(get_name() << " successfully started");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
}

void
ReversedListValidator::do_stop([[maybe_unused]] const std::vector<std::string>& args)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
  thread_.stop_working_thread();
  ERS_LOG(get_name() << " successfully stopped");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_stop() method";
}

/**
 * @brief Format a std::vector<int> to a stream
 * @param t ostream Instance
 * @param ints Vector to format
 * @return ostream Instance
 */
std::ostream&
operator<<(std::ostream& t, std::vector<int> ints)
{
  t << "{";
  bool first = true;
  for (auto& i : ints) {
    if (!first)
      t << ", ";
    first = false;
    t << i;
  }
  return t << "}";
}

void
ReversedListValidator::do_work()
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";
  int reversedCount = 0;
  int comparisonCount = 0;
  int failureCount = 0;
  std::vector<int> reversedData;
  std::vector<int> originalData;

  while (thread_.thread_running()) {
    TLOG(TLVL_LIST_VALIDATION) << get_name() << ": Going to receive data from the reversed list queue";
    if (!reversedDataQueue_->pop(reversedData, queueTimeout_)) {continue;}
    ++reversedCount;

    TLOG(TLVL_LIST_VALIDATION) << get_name() << ": Received reversed list #" << reversedCount
                             << ". It has size " << reversedData.size()
                             << ". Now going to receive data from the original data queue.";
    bool originalWasSuccessfullyReceived = false;
    while (!originalWasSuccessfullyReceived && thread_.thread_running())
    {
      TLOG(TLVL_LIST_VALIDATION) << get_name() << ": Popping the next element off the original data queue";
      if (originalDataQueue_->pop(originalData, queueTimeout_))
      {
        originalWasSuccessfullyReceived = true;
        ++comparisonCount;
      }
      else
      {
        std::ostringstream oss_warn;
        oss_warn << "pop from original data queue";
        ers::warning(dunedaq::appfwk::QueueTimeoutExpired(ERS_HERE, get_name(), oss_warn.str(),
                     std::chrono::duration_cast<std::chrono::milliseconds>(queueTimeout_).count()));
      }
    }

    if (originalWasSuccessfullyReceived)
    {
      std::ostringstream oss_prog;
      oss_prog << "Validating list #" << reversedCount << ", original contents " << originalData
               << " and reversed contents " << reversedData << ". ";
      ers::debug(ProgressUpdate(ERS_HERE, get_name(), oss_prog.str()));

      TLOG(TLVL_LIST_VALIDATION) << get_name() << ": Reversing the copy of the original data";
      std::reverse(originalData.begin(), originalData.end());

      TLOG(TLVL_LIST_VALIDATION) << get_name() << ": Comparing the two reversed lists";
      if (originalData != reversedData)  // "original" has been reversed at this point
      {
        //ers::error();
        ++failureCount;
      }
    }
    TLOG(TLVL_LIST_VALIDATION) << get_name() << ": End of do_work loop";
  }

  std::ostringstream oss_summ;
  oss_summ << ": Exiting do_work() method, received " << reversedCount << " reversed lists, "
           << "compared " << comparisonCount << " of them to their original data, and found "
           << failureCount << " mismatches. ";
  ers::info(ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
}

} // namespace afv1_example
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::afv1_example::ReversedListValidator)
