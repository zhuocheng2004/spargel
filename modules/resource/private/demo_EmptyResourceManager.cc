
#include <assert.h>
#include <modules/resource/public/EmptyResourceManager.h>

int main() {
    using namespace spargel::resource;

    EmptyResourceManager resourceManager;

    assert(!resourceManager.has(ResourceID("path/to/resource")));
    assert(!resourceManager.has(ResourceID("ns", "path/to/resource")));

    assert(resourceManager.open(ResourceID("path/to/resource")) == nullptr);
    assert(resourceManager.open(ResourceID("ns", "path/to/resource")) == nullptr);

    resourceManager.close();

    return 0;
}
