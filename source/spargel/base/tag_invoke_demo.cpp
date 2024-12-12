#include <spargel/base/logging.h>
#include <spargel/base/tag_invoke.h>

namespace A {
    namespace __foo {
        struct foo {
            template <typename T>
            constexpr auto operator()(T t) const {
                spargel::base::tag_invoke(foo{}, t);
            }
        };
        template <typename T>
        void tag_invoke(foo, T t) {
            spargel_log_info("default impl");
        }
    }  // namespace __foo
    inline constexpr __foo::foo foo{};
}  // namespace A

namespace B {
    struct bar {};
    void tag_invoke(spargel::base::tag<A::foo>, bar b) { spargel_log_info("special impl for bar"); }
}  // namespace B

int main() {
    A::foo(1);
    A::foo(B::bar{});
    return 0;
}
