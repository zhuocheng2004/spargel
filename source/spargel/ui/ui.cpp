#include <spargel/config.h>
#include <spargel/ui/ui.h>

namespace spargel::ui {

    base::unique_ptr<platform> make_platform_dummy();
#if SPARGEL_IS_LINUX
    base::unique_ptr<platform> make_platform_xcb();
#elif SPARGEL_IS_MACOS
    base::unique_ptr<platform> make_platform_appkit();
#endif

    base::unique_ptr<platform> make_platform() {
#if SPARGEL_IS_LINUX
        return make_platform_xcb();
#elif SPARGEL_IS_MACOS
        return make_platform_appkit();
#elif SPARGEL_IS_WINDOWS
        return make_platform_dummy();
#endif
        return nullptr;
    }

}  // namespace spargel::ui
