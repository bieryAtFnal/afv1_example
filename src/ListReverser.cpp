/**
 * @file ListReverser.cpp ListReverser class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "CommonIssues.hpp"
#include "ListReverser.hpp"

#include <ers/ers.h>
#include "TRACE/trace.h"

#include <chrono>
#include <thread>

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "ListReverser" // NOLINT
#define TLVL_ENTER_EXIT_METHODS 10
#define TLVL_LIST_REVERSAL 15

namespace dunedaq {
namespace afv1_example {

ListReverser::ListReverser(const std::string& name)
  : DAQModule(name)
  , thread_(std::bind(&ListReverser::do_work, this))
  , inputQueue_(nullptr)
  , outputQueue_(nullptr)
  , queueTimeout_(100)
  , outputQueueName_("undefined")
{
  register_command("start", &ListReverser::do_start);
  register_command("stop", &ListReverser::do_stop);
}

void
ListReverser::init()
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";
  inputQueue_.reset(new dunedaq::appfwk::DAQSource<std::vector<int>>(get_config()["input"].get<std::string>()));
  outputQueueName_ = get_config()["output"].get<std::string>();
  outputQueue_.reset(new dunedaq::appfwk::DAQSink<std::vector<int>>(outputQueueName_));
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
}

void
ListReverser::do_start([[maybe_unused]] const std::vector<std::string>& args)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
  thread_.start_working_thread_();
  ERS_LOG(get_name() << " successfully started");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
}

void
ListReverser::do_stop([[maybe_unused]] const std::vector<std::string>& args)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
  thread_.stop_working_thread_();
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
ListReverser::do_work()
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";
  int receivedCount = 0;
  int sentCount = 0;
  std::vector<int> workingVector;

  // being cautious - make sure queues are defined
  bool queuesAreDefined = true;
  if (inputQueue_.get() == nullptr)
  {
    ers::fatal(InvalidQueueFatalError(ERS_HERE, get_name(), "input"));
    queuesAreDefined = false;
  }
  if (outputQueue_.get() == nullptr)
  {
    ers::fatal(InvalidQueueFatalError(ERS_HERE, get_name(), "output"));
    queuesAreDefined = false;
  }

  while (queuesAreDefined && thread_.thread_running()) {
    TLOG(TLVL_LIST_REVERSAL) << get_name() << ": Going to receive data from input queue";
    if (!inputQueue_->pop(workingVector, queueTimeout_)) {continue;}

    ++receivedCount;
    TLOG(TLVL_LIST_REVERSAL) << get_name() << ": Received list #" << receivedCount
                             << ". It has size " << workingVector.size() << ". Reversing its contents";
    std::reverse(workingVector.begin(), workingVector.end());

    std::ostringstream oss_prog;
    oss_prog << "Reversed list #" << receivedCount << ", new contents " << workingVector
             << " and size " << workingVector.size() << ". ";
    ers::debug(ProgressUpdate(ERS_HERE, get_name(), oss_prog.str()));

    bool successfullyWasSent = false;
    while (!successfullyWasSent && thread_.thread_running())
    {
      TLOG(TLVL_LIST_REVERSAL) << get_name() << ": Pushing the reversed list onto the output queue";
      try
      {
        outputQueue_->push(workingVector, queueTimeout_);
        successfullyWasSent = true;
        ++sentCount;
      }
      catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt)
      {
        std::ostringstream oss_warn;
        oss_warn << "push to output queue \"" << outputQueueName_ << "\"";
        ers::warning(dunedaq::appfwk::QueueTimeoutExpired(ERS_HERE, get_name(), oss_warn.str(),
                     std::chrono::duration_cast<std::chrono::milliseconds>(queueTimeout_).count()));
      }
    }
    TLOG(TLVL_LIST_REVERSAL) << get_name() << ": End of do_work loop";
  }

  std::ostringstream oss_summ;
  oss_summ << ": Exiting do_work() method, received " << receivedCount
           << " lists, and successfully sent " << sentCount << ". ";
  ers::info(ProgressUpdate(ERS_HERE, get_name(), oss_summ.str()));
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
}

} // namespace afv1_example
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::afv1_example::ListReverser)
