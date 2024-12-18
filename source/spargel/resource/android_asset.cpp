
#include <spargel/resource/android_asset.h>

namespace spargel::resource {

    void android_asset_resource::close() {
        AAsset_close(_asset);
        resource::close();
    }

    usize android_asset_resource::size() { return AAsset_getLength(_asset); }

    void android_asset_resource::get_data(void* buf) { AAsset_read(_asset, buf, size()); }

    android_asset_resource* android_asset_resource_manager::open(
        const spargel::resource::resource_id& id) {
        AAsset* asset = AAssetManager_open(_asset_manager, id.path().data(), AASSET_MODE_BUFFER);
        return asset ? new android_asset_resource(asset) : nullptr;
    }
}  // namespace spargel::resource
