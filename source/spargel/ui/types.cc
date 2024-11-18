#include "spargel/ui/types.h"

namespace spargel::ui {

bool operator==(RectSize const& lhs, RectSize const& rhs) {
  return lhs.width == rhs.width && lhs.height == rhs.height;
}

}  // namespace spargel::ui
