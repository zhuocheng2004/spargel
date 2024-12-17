#pragma once

#include <spargel/base/string.h>

namespace spargel::resource {

    const base::string DEFAULT_NS = base::string_from_cstr("core");

    class resource_id {
    public:
        resource_id(const base::string& ns, const base::string& path) : _ns(ns), _path(path) {}
        resource_id(const base::string& path) : resource_id(DEFAULT_NS, path) {}

        resource_id(const char* ns, const char* path)
            : resource_id(base::string_from_cstr(ns), base::string_from_cstr(path)) {}
        resource_id(const char* path) : resource_id(base::string_from_cstr(path)) {}

        const base::string& ns() const { return _ns; }
        const base::string& path() const { return _path; }

    private:
        base::string _ns;
        base::string _path;
    };

    class resource;

    class resource_manager {
    public:
        virtual ~resource_manager() = default;

        virtual void close() {}

        virtual resource* open(const resource_id& id) = 0;
    };

    class resource {
    public:
        virtual ~resource() = default;

        virtual void close() {}

        virtual size_t size() = 0;

        virtual void get_data(void* buf) = 0;

        // virtual void* map_data() = 0;
    };

    // The most trivial example
    class empty_resource_manager : public resource_manager {
        resource* open(const resource_id& id) override { return nullptr; }
    };
}  // namespace spargel::resource
