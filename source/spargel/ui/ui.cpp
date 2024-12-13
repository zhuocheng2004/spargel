#include <spargel/config.h>
#include <spargel/ui/ui.h>

#if SPARGEL_IS_MACOS
namespace spargel::ui {
    base::unique_ptr<platform> make_platform_appkit();
}
#endif

namespace spargel::ui {

    base::unique_ptr<platform> make_platform() {
#if SPARGEL_IS_MACOS
        return make_platform_appkit();
#endif
        return nullptr;
    }

}  // namespace spargel::ui
