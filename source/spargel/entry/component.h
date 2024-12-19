#pragma once

namespace spargel::entry {

    struct launch_data;

    class component {
    public:
        explicit component(launch_data* l) : _launch_data{l} {}

        virtual void on_load() = 0;

        launch_data* get_launch_data() { return _launch_data; }

    private:
        launch_data* _launch_data;
    };

}  // namespace spargel::entry
