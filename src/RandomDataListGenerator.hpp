/**
 * @file RandomDataListGenerator.hpp
 *
 * RandomDataListGenerator is a simple DAQModule implementation that
 * periodically generates a list of random integers.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef KURTTEST_SRC_RANDOMDATALISTGENERATOR_HPP_
#define KURTTEST_SRC_RANDOMDATALISTGENERATOR_HPP_

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/ThreadHelper.hpp"

#include <ers/Issue.h>

#include <memory>
#include <string>
#include <vector>

namespace dunedaq {
namespace KurtTest {

/**
 * @brief RandomDataListGenerator creates vectors of ints and writes
 * them to the configured output queues.
 */
class RandomDataListGenerator : public dunedaq::appfwk::DAQModule
{
public:
  /**
   * @brief RandomDataListGenerator Constructor
   * @param name Instance name for this RandomDataListGenerator instance
   */
  explicit RandomDataListGenerator(const std::string& name);

  RandomDataListGenerator(const RandomDataListGenerator&) =
    delete; ///< RandomDataListGenerator is not copy-constructible
  RandomDataListGenerator& operator=(const RandomDataListGenerator&) =
    delete; ///< RandomDataListGenerator is not copy-assignable
  RandomDataListGenerator(RandomDataListGenerator&&) =
    delete; ///< RandomDataListGenerator is not move-constructible
  RandomDataListGenerator& operator=(RandomDataListGenerator&&) =
    delete; ///< RandomDataListGenerator is not move-assignable

  void init() override;

private:
  // Commands
  void do_configure(const std::vector<std::string>& args);
  void do_start(const std::vector<std::string>& args);
  void do_stop(const std::vector<std::string>& args);

  // Threading
  dunedaq::appfwk::ThreadHelper thread_;
  void do_work();

  // Configuration
  std::vector<std::unique_ptr<dunedaq::appfwk::DAQSink<std::vector<int>>>> outputQueues_;
  std::chrono::milliseconds queueTimeout_;
  size_t nIntsPerList_;
  size_t waitBetweenSendsMsec_;
  std::vector<std::string> outputQueueNames_;
};
} // namespace KurtTest

ERS_DECLARE_ISSUE_BASE(KurtTest,
                       NoOutputQueuesAvailableWarning,
                       appfwk::GeneralDAQModuleIssue,
                       name << ": No output queues were available, so the generated list of integers will be dropped. Has initialization been successfully completed?",
                       ERS_EMPTY,
                       ((std::string)name))

} // namespace dunedaq

#endif // KURTTEST_SRC_RANDOMDATALISTGENERATOR_HPP_