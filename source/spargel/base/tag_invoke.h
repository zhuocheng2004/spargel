#pragma once

#include <spargel/base/meta.h>

namespace spargel::base {

    namespace __tag_invoke {
        void tag_invoke();
        struct tag_invoke_impl {
            template <typename Tag, typename... Args>
            constexpr decltype(auto) operator()(Tag&& tag, Args&&... args) const {
                return tag_invoke(forward<Tag>(tag), forward<Args>(args)...);
            }
        };
    }  // namespace __tag_invoke

    inline constexpr __tag_invoke::tag_invoke_impl tag_invoke{};

    template <auto& o>
    using tag = decay<decltype(o)>;

}  // namespace spargel::base
