#include <spargel/config.h>
#include <spargel/ui/ui.h>

namespace spargel::ui {

#if SPARGEL_IS_LINUX
    base::unique_ptr<platform> make_platform_xcb();
#elif SPARGEL_IS_MACOS
    base::unique_ptr<platform> make_platform_appkit();
#elif SPARGEL_IS_WINDOWS
    base::unique_ptr<platform> make_platform_win32();
#elif SPARGEL_UI_DUMMY
    base::unique_ptr<platform> make_platform_dummy();
#endif

    base::unique_ptr<platform> make_platform() {
#if SPARGEL_IS_LINUX
        return make_platform_xcb();
#elif SPARGEL_IS_MACOS
        return make_platform_appkit();
#elif SPARGEL_IS_WINDOWS
        return make_platform_win32();
#elif SPARGEL_UI_DUMMY
        return make_platform_dummy();
#else
        return nullptr;
#endif
    }

}  // namespace spargel::ui
