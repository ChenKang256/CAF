// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/main/LICENSE.

#pragma once

#include "caf/detail/core_export.hpp"
#include "caf/detail/unique_function.hpp"
#include "caf/fwd.hpp"

#include <string>

namespace caf {

/// Stores spawn-time settings and flags.
class CAF_CORE_EXPORT actor_config {
public:
  // -- member types -----------------------------------------------------------

  using init_fun_type = detail::unique_function<behavior(local_actor*)>;

  // -- constructors, destructors, and assignment operators --------------------

  explicit actor_config(scheduler* sched = nullptr,
                        local_actor* parent = nullptr);

  // -- member variables -------------------------------------------------------

  scheduler* sched;
  local_actor* parent;
  int flags = 0;
  int8_t core = -1; // Bound to no core
  uint8_t prio = 127; // Default priority
  detail::unique_function<behavior(local_actor*)> init_fun;
  detail::mailbox_factory* mbox_factory = nullptr;

  // -- properties -------------------------------------------------------------

  actor_config& add_flag(int x) {
    flags |= x;
    return *this;
  }

  void setCore(int8_t _core) {
    this->core = _core;
  }

  void setPrio(uint8_t _prio) {
    this->prio = _prio;
  }
};

/// @relates actor_config
CAF_CORE_EXPORT std::string to_string(const actor_config& x);

} // namespace caf
