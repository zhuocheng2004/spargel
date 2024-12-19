
#include <spargel/ui/ui_dummy.h>

namespace spargel::ui {

    base::unique_ptr<platform> make_platform_dummy() { return base::make_unique<platform_dummy>(); }

}  // namespace spargel::ui
