#pragma once

#include <spargel/base/string.h>
#include <spargel/base/string_view.h>

namespace spargel::resource {

    class resource_id {
    public:
        static constexpr base::string_view default_namespace = "core";

        resource_id(base::string_view ns, base::string_view path) : _namespace(ns), _path(path) {}
        resource_id(base::string_view path) : resource_id(default_namespace, path) {}

        const base::string& ns() const { return _namespace; }
        const base::string& path() const { return _path; }

    private:
        base::string _namespace;
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
        resource() : _mapped(nullptr) {}

        virtual ~resource() = default;

        virtual void close();

        virtual size_t size() = 0;

        virtual void get_data(void* buf) = 0;

        virtual void* map_data();

    private:
        void* _mapped;
        size_t _mapped_size;
    };

    // The most trivial example
    class empty_resource_manager final : public resource_manager {
    public:
        resource* open(const resource_id& id) override { return nullptr; }
    };
}  // namespace spargel::resource
