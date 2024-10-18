#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <new>
#include <utility>

namespace Impl {
// c++23 std::start_lifetime_as_array
template <typename T>
auto start_lifetime_as_array(void* p, size_t) noexcept -> T* {
  // undefind behavior
  return static_cast<T*>(p);
}
auto Panic(char const* msg) noexcept -> void {
  fprintf(stderr, "panic: %s\n", msg);
  exit(1);
}
auto NextId() noexcept -> size_t {
  static size_t id = 0;
  id++;
  return id;
}
}

using Entity = uint32_t;

auto NextEntity() noexcept -> Entity {
  static uint32_t counter = 0;
  counter++;
  return counter;
}

// a unique id for each type
template <typename>
auto TypeId() noexcept -> size_t {
  static const size_t id = Impl::NextId();
  return id;
}

struct SparseSet {
  SparseSet() noexcept {
    sparse =
      Impl::start_lifetime_as_array<uint32_t>(
        malloc(sizeof(uint32_t) * MaxId), MaxId);
    dense =
      Impl::start_lifetime_as_array<uint32_t>(
        malloc(sizeof(uint32_t) * MaxId), MaxId);
    if (sparse == nullptr || dense == nullptr) {
      Impl::Panic("error: out of memory");
    }
  }
  ~SparseSet() {
    free(sparse);
    free(dense);
  }

  auto Emplace() noexcept -> Entity {
    auto entity = NextEntity();
    Insert(entity);
    return entity;
  }

  auto Insert(Entity entity) noexcept -> void {
    if (counter >= MaxId || entity >= MaxId) {
      Impl::Panic("error: out of space");
    }
    sparse[entity] = counter;
    dense[counter] = entity;
    counter++;
  }

  auto Has(Entity entity) const noexcept -> bool {
    return (entity <= MaxId)
        && (sparse[entity] < counter)
        && (dense[sparse[entity]] == entity);
  }

  auto Delete(Entity entity) noexcept -> void {
    if (Size() == 0) {
      Impl::Panic("error: nothing to delete");
    }
    auto e2 = dense[counter - 1];
    dense[sparse[entity]] = e2;
    sparse[e2] = sparse[entity];
    counter--;
  }

  auto Size() const noexcept -> size_t {
    return counter;
  }

  static constexpr uint32_t MaxId = 20000;
  uint32_t counter = 0;

  // entity -> internal index
  uint32_t* sparse;
  // internal index -> entity
  uint32_t* dense;
};

struct StorageBase {
  StorageBase(size_t id) noexcept : type_id{id} {}
  size_t const type_id;
  void* data = nullptr;
  SparseSet set;
};

template <typename T>
struct Storage : StorageBase {
  Storage() noexcept : StorageBase{TypeId<T>()} {
    data = malloc(sizeof(T) * MaxId);
    if (!data) {
      Impl::Panic("error: out of memory");
    }
  }
  template <typename... Args>
  auto Emplace(Entity entity, Args&&... args) noexcept -> void {
    set.Insert(entity);
    // this is terrible
    auto real_data = reinterpret_cast<T*>(data);
    auto ptr = real_data + set.sparse[entity];
    new(ptr) T(std::forward<Args>(args)...);
  }
  auto Get(Entity entity) noexcept -> T& {
    auto real_data = reinterpret_cast<T*>(data);
    return real_data[set.sparse[entity]];
  }

  static constexpr uint32_t MaxId = 20000;
};

struct Registry {
  auto CreateEntity() noexcept -> Entity {
    return entities.Emplace();
  }
  
  template <typename T, typename... Args>
  auto Emplace(Entity entity, Args&&... args) noexcept -> void {
    auto offset = FindStorage<T>();
    Storage<T>* ptr = nullptr;
    if (offset != -1) {
      ptr = reinterpret_cast<Storage<T>*>(storage + offset);
    } else {
      auto p = storage + type_count * sizeof(Storage<int>);
      ptr = new(p) Storage<T>();
      type_count++;
    }
    ptr->Emplace(entity, std::forward<Args>(args)...);
  }

  // return offset in storage[]
  template <typename T>
  auto FindStorage() const noexcept -> size_t {
    auto id = TypeId<T>();
    for (int i = 0; i < type_count; i++) {
      // terrible hack
      auto ptr = reinterpret_cast<StorageBase const*>(storage + i * sizeof(Storage<int>));
      if (ptr->type_id == id) {
        return i * sizeof(Storage<int>);
      }
    }
    return -1;
  }

  template <typename T>
  auto HasComponent(Entity entity) const noexcept -> bool {
    auto offset = FindStorage<T>();
    if (offset == -1) {
      return false;
    }
    auto ptr = reinterpret_cast<Storage<T> const*>(storage + offset);
    if (ptr->type_id != TypeId<T>()) {
      Impl::Panic("internal error");
    }
    return ptr->set.Has(entity);
  }

  template <typename T>
  auto GetComponent(Entity entity) noexcept -> T* {
    auto offset = FindStorage<T>();
    if (offset == -1) {
      return nullptr;
    }
    auto ptr = reinterpret_cast<Storage<T>*>(storage + offset);
    if (ptr->type_id != TypeId<T>()) {
      Impl::Panic("internal error");
    }
    if (!ptr->set.Has(entity)) {
      return nullptr;
    }
    return &ptr->Get(entity);
  }

  SparseSet entities;
  
  static constexpr size_t MaxComponents = 10;
  // terrible hack
  // should use `DenseMap`
  char storage[MaxComponents * sizeof(Storage<int>)];
  size_t type_count = 0;
};

struct Position {
  float x;
  float y;
};

struct Velocity {
  float dx;
  float dy;
};

#define CHECK(expr) if (!(expr)) { Impl::Panic("error: check " #expr " failed"); }

int main() {
  SparseSet ss;
  ss.Emplace();
  ss.Emplace();
  ss.Emplace();

  CHECK(ss.Has(1) && ss.Has(2) && ss.Has(3));
  CHECK(!ss.Has(0) && !ss.Has(4));

  ss.Delete(2);
  CHECK(!ss.Has(2));
  CHECK(ss.Has(1) && !ss.Has(2) && ss.Has(3));

  Registry registry;
  for (int i = 0; i < 1000; i++) {
    auto const entity = registry.CreateEntity();
    registry.Emplace<Position>(entity, i * 1.f, i * 1.f);
    if(i % 2 == 0) {
      registry.Emplace<Velocity>(entity, i * .1f, i * .1f);
    }
  }

  for (int i = 0; i < registry.entities.counter; i++) {
    auto entity = registry.entities.dense[i];
    auto* pos = registry.GetComponent<Position>(entity);
    auto* vel = registry.GetComponent<Velocity>(entity);
    if (pos) {
      printf("info: entity %d has position { x = %.4f; y = %.4f }\n", entity, pos->x, pos->y);
    }
    if (vel) {
      printf("info: entity %d has velocity { dx = %.4f; dy = %.4f }\n", entity, vel->dx, vel->dy);
    }
  }

  return 0;
}
