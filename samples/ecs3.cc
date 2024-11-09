template <typename T>
struct ComponentInfo;

#define COMPONENT(Type, name) \
  template <> \
  struct ComponentInfo<Type> { \
    static constexpr char const* = name; \
    static constexpr size_t size = sizeof(T); \
  };

class World {
  public:
};

int main() {
  return 0;
}
