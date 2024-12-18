#pragma once

#include <spargel/resource/resource.h>

// Android
#include <android/asset_manager.h>

namespace spargel::resource {

    class android_asset_resource : public resource {
        friend class android_asset_resource_manager;

    public:
        void close() override;

        size_t size() override;

        void get_data(void* buf) override;

    private:
        AAsset* _asset;

        explicit android_asset_resource(AAsset* asset) : _asset(asset) {}
    };

    class android_asset_resource_manager : public resource_manager {
    public:
        explicit android_asset_resource_manager(AAssetManager* asset_manager)
            : _asset_manager(asset_manager) {}

        android_asset_resource* open(const resource_id& id) override;

    private:
        AAssetManager* _asset_manager;
    };

}  // namespace spargel::resource
