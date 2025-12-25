#pragma once

#include <vector>
#include <optional>

namespace repository {

// Generic repository interface for CRUD operations.
template <typename T, typename U>
class Repository {
public:
  Repository() = default;
  virtual ~Repository() = default;

  // Create a new item
  virtual T create(const T& item) = 0;

  // Read an item by its identifier
  virtual std::optional<T> read(const U& id) = 0;

  // Read all items
  virtual std::vector<T> readAll() = 0;

  // Update an existing item
  virtual T update(const T& item) = 0;

  // Delete an item by its identifier
  virtual void deleteItem(const U& id) = 0;
};

}  // namespace repository
