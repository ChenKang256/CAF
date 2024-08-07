// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/main/LICENSE.

#pragma once

#include "caf/test/block.hpp"

#include "caf/detail/test_export.hpp"

namespace caf::test {

/// Represents an `AND_WHEN` block.
class CAF_TEST_EXPORT and_when : public block {
public:
  using block::block;

  block_type type() const noexcept override;

  then* get_then(int id, std::string_view description,
                 const detail::source_location& loc) override;

  and_then* get_and_then(int id, std::string_view description,
                         const detail::source_location& loc) override;

  scope commit();
};

} // namespace caf::test
