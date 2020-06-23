/**
 * @file RandomDataListGenerator.cpp RandomDataListGenerator class
 * implementation
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "CommonIssues.hpp"
#include "RandomDataListGenerator.hpp"

#include <ers/ers.h>
#include <TRACE/trace.h>

#include <chrono>
#include <cstdlib>
#include <thread>

/**
 * @brief Name used by TRACE TLOG calls from this source file
 */
#define TRACE_NAME "RandomDataListGenerator" // NOLINT
#define TLVL_ENTER_EXIT_METHODS 10
#define TLVL_LIST_GENERATION 15

namespace dunedaq {
namespace KurtTest {

RandomDataListGenerator::RandomDataListGenerator(const std::string& name)
  : dunedaq::appfwk::DAQModule(name)
  , thread_(std::bind(&RandomDataListGenerator::do_work, this))
  , outputQueues_()
  , queueTimeout_(100)
  , nIntsPerList_(4)
  , waitBetweenSendsMsec_(1000)
  , outputQueueNames_()
{
  register_command("configure", &RandomDataListGenerator::do_configure);
  register_command("start",  &RandomDataListGenerator::do_start);
  register_command("stop",  &RandomDataListGenerator::do_stop);
}

void RandomDataListGenerator::init()
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";
  for (auto& output : get_config()["outputs"]) {
    std::string outputQueueName = output.get<std::string>();
    outputQueues_.emplace_back(new dunedaq::appfwk::DAQSink<std::vector<int>>(outputQueueName));
    outputQueueNames_.emplace_back(outputQueueName);
  }
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
}

void
RandomDataListGenerator::do_configure([[maybe_unused]] const std::vector<std::string>& args)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_configure() method";
  nIntsPerList_ = get_config().value<size_t>("nIntsPerList", 4);
  waitBetweenSendsMsec_ = get_config().value<size_t>("waitBetweenSendsMsec", 1000);
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_configure() method";
}

void
RandomDataListGenerator::do_start([[maybe_unused]] const std::vector<std::string>& args)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_start() method";
  thread_.start_working_thread_();
  ERS_INFO(get_name() << " successfully started");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_start() method";
}

void
RandomDataListGenerator::do_stop([[maybe_unused]] const std::vector<std::string>& args)
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_stop() method";
  thread_.stop_working_thread_();
  ERS_INFO(get_name() << " successfully stopped");
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
RandomDataListGenerator::do_work()
{
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering do_work() method";
  size_t generatedCount = 0;
  size_t sentCount = 0;
  std::ostringstream oss;

  while (thread_.thread_running()) {
    TLOG(TLVL_LIST_GENERATION) << get_name() << ": Creating list of length " << nIntsPerList_;
    std::vector<int> theList(nIntsPerList_);

    TLOG(TLVL_LIST_GENERATION) << get_name() << ": Start of fill loop";
    for (size_t idx = 0; idx < nIntsPerList_; ++idx)
    {
      theList[idx] = (rand() % 1000) + 1;
    }
    generatedCount++;
    oss << "Generated list #" << generatedCount << " with contents " << theList
        << " and size " << theList.size() << ". ";
    ers::debug(ProgressUpdate(ERS_HERE, get_name(), oss.str()));
    oss.str("");

    TLOG(TLVL_LIST_GENERATION) << get_name() << ": Pushing list onto " << outputQueues_.size() << " outputQueues";
    int tmpIdx = 0;
    for (auto& outQueue : outputQueues_)
    {
      std::string thisQueueName = outputQueueNames_[tmpIdx++];
      bool successfullyWasSent = false;
      while (!successfullyWasSent && thread_.thread_running())
      {
        TLOG(TLVL_LIST_GENERATION) << get_name() << ": Pushing the generated list onto queue " << thisQueueName;
        try
        {
          outQueue->push(theList, queueTimeout_);
          successfullyWasSent = true;
          ++sentCount;
        }
        catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt)
        {
          oss << "push to output queue \"" << thisQueueName << "\"";
          ers::warning(dunedaq::appfwk::QueueTimeoutExpired(ERS_HERE, get_name(), oss.str(),
                       std::chrono::duration_cast<std::chrono::milliseconds>(queueTimeout_).count()));
          oss.str("");
        }
      }
    }
    if (outputQueues_.size() == 0)
    {
      ers::warning(NoOutputQueuesAvailableWarning(ERS_HERE, get_name()));
    }

    TLOG(TLVL_LIST_GENERATION) << get_name() << ": Start of sleep between sends";
    std::this_thread::sleep_for(std::chrono::milliseconds(waitBetweenSendsMsec_));
    TLOG(TLVL_LIST_GENERATION) << get_name() << ": End of do_work loop";
  }

  oss << ": Exiting the do_work() method, generated " << generatedCount << " lists, and successfully sent "
      << sentCount << " copies. ";
  ers::info(ProgressUpdate(ERS_HERE, get_name(), oss.str()));
  oss.str("");
  TLOG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting do_work() method";
}

} // namespace KurtTest 
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::KurtTest::RandomDataListGenerator)