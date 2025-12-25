#pragma once

#include <vector>
#include <optional>

namespace service {

// Generic service interface
template <typename T, typename ID>
class Service {
public:
  virtual ~Service() = default;

  virtual T create(const T& item) = 0;
  virtual std::optional<T> getById(const ID& id) = 0;
  virtual std::vector<T> getAll() = 0;
  virtual T update(const T& item) = 0;
  virtual void deleteById(const ID& id) = 0;
};

}  // namespace service
