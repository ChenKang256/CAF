// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/main/LICENSE.

#pragma once

#include "caf/config.hpp"

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <list>
#include <mutex>
#include <thread>
#include <queue>
#include <utility>

namespace caf::detail {

template <typename T>
struct Compare {
    bool operator()(const std::pair<T, uint8_t>& a, const std::pair<T, uint8_t>& b) const {
        return a.second > b.second;
    }
};

/*
 * A thread-safe, priority-queue for work-priority.
 */
template <class T>
class priority_queue {
public:
  using value_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  // -- for the owner ----------------------------------------------------------

  void prepend(pointer ptr, uint8_t prio = 127) {
    CAF_ASSERT(ptr != nullptr);
    std::unique_lock guard{mtx_};
    pq_.emplace(ptr, prio);
  }

  pointer try_take_head() {
    std::unique_lock guard{mtx_};
    if (!pq_.empty()) {
      auto result = pq_.top();
      pq_.pop();
      return result.first;
    }
    return nullptr;
  }

  template <class Duration>
  pointer try_take_head(Duration rel_timeout) {
    auto abs_timeout = std::chrono::system_clock::now() + rel_timeout;
    std::unique_lock guard{mtx_};
    while (pq_.empty()) {
      if (cv_.wait_until(guard, abs_timeout) == std::cv_status::timeout) {
        return nullptr;
      }
    }
    auto result = pq_.top();
    pq_.pop();
    return result.first;
  }

  pointer take_head() {
    std::unique_lock guard{mtx_};
    while (pq_.empty()) {
      cv_.wait(guard);
    }
    auto* result = pq_.top();
    pq_.pop();
    return result.first;
  }

  // Unsafe, since it does not wake up a currently sleeping worker.
  void unsafe_append(pointer ptr, uint8_t prio = 127) {
    std::unique_lock guard{mtx_};
    pq_.emplace(ptr, prio);
  }

  // -- for others -------------------------------------------------------------

  void append(pointer ptr, uint8_t prio = 127) {
    bool do_notify = false;
    {
      std::unique_lock guard{mtx_};
      do_notify = pq_.empty();
      pq_.emplace(ptr, prio);
    }
    if (do_notify) {
      cv_.notify_one();
    }
  }

  /// @brief 只能取top的, 因为是优先队列, 没有back
  pointer try_take_tail() {
    std::unique_lock guard{mtx_};
    if (!pq_.empty()) {
      auto result = pq_.top();
      pq_.pop();
      return result.first;
    }
    return nullptr;
  }

private:
  std::mutex mtx_;
  std::condition_variable cv_;
  std::priority_queue<std::pair<pointer, uint8_t>, std::vector<std::pair<pointer, uint8_t>>, Compare<pointer>> pq_;
};

} // namespace caf::detail
