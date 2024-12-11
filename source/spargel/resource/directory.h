#pragma once

#include <spargel/base/base.h>
#include <spargel/resource/resource.h>

namespace spargel::resource {
    void resource_directory_manager_init(struct resource_manager* manager,
                                              spargel::base::string base_path);
}
