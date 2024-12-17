#pragma once

#include <spargel/resource/resource.h>

// libc
#include <stdio.h>

namespace spargel::resource {

    class directory_resource : public resource {
    public:
        directory_resource(FILE* fp) : _fp(fp) {}

        void close() override;

        size_t size() override;

        void get_data(void* buf) override;

    private:
        FILE* _fp;
    };

    class directory_resource_manager : public resource_manager {
    public:
        directory_resource_manager(const base::string& root_path) : _root_path(root_path) {}

        directory_resource* open(const resource_id& id) override;

    private:
        base::string _root_path;

        base::string _real_path(const resource_id& id);
    };

}  // namespace spargel::resource
