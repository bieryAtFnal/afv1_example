/**
 * @file ListReverser.hpp
 *
 * ListReverser is a simple DAQModule implementation that reads a list
 * of integers from one queue, reverses their order in the list, and pushes
 * the reversed list onto another queue.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef AFV1_EXAMPLE_SRC_LISTREVERSER_HPP_
#define AFV1_EXAMPLE_SRC_LISTREVERSER_HPP_

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"

#include <ers/Issue.h>

#include <memory>
#include <string>
#include <vector>

namespace dunedaq {
namespace afv1_example {

/**
 * @brief ListReverser reads lists of integers from one queue,
 * reverses the order of the list, and writes out the reversed list.
 */
class ListReverser : public dunedaq::appfwk::DAQModule
{
public:
  /**
   * @brief ListReverser Constructor
   * @param name Instance name for this ListReverser instance
   */
  explicit ListReverser(const std::string& name);

  ListReverser(const ListReverser&) =
    delete; ///< ListReverser is not copy-constructible
  ListReverser& operator=(const ListReverser&) =
    delete; ///< ListReverser is not copy-assignable
  ListReverser(ListReverser&&) =
    delete; ///< ListReverser is not move-constructible
  ListReverser& operator=(ListReverser&&) =
    delete; ///< ListReverser is not move-assignable

  void init() override;

private:
  // Commands
  void do_start(const std::vector<std::string>& args);
  void do_stop(const std::vector<std::string>& args);

  // Threading
  dunedaq::appfwk::ThreadHelper thread_;
  void do_work(std::atomic<bool>&);

  // Configuration
  std::unique_ptr<dunedaq::appfwk::DAQSource<std::vector<int>>> inputQueue_;
  std::unique_ptr<dunedaq::appfwk::DAQSink<std::vector<int>>> outputQueue_;
  std::chrono::milliseconds queueTimeout_;
};
} // namespace afv1_example
} // namespace dunedaq

#endif // AFV1_EXAMPLE_SRC_LISTREVERSER_HPP_
