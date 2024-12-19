#include <spargel/base/logging.h>
#include <spargel/gpu/gpu3.h>

// libc
#include <stdio.h>

using namespace spargel::gpu;

int main() {
    prepared_graph graph;
    graph.nodes.push(prepared_graph::node_kind::texture, 0, "surface.0");
    graph.nodes.push(prepared_graph::node_kind::render, 0, "draw_triangle");
    graph.nodes.push(prepared_graph::node_kind::texture, 1, "surface.1");
    graph.nodes.push(prepared_graph::node_kind::present, 0, "present");
    graph.nodes.push(prepared_graph::node_kind::render, 1, "draw_debug");
    graph.nodes.push(prepared_graph::node_kind::texture, 2, "surface.2");
    // the <<draw_triangle>> node
    graph.nodes[0].outputs.push(1);
    graph.nodes[1].inputs.push(0);
    graph.nodes[1].outputs.push(2);
    graph.nodes[2].inputs.push(1);
    // the <<present>> node
    graph.nodes[2].outputs.push(3);
    graph.nodes[3].inputs.push(2);
    // the <<debug_draw>> node
    graph.nodes[2].outputs.push(4);
    graph.nodes[4].inputs.push(2);
    graph.nodes[4].outputs.push(5);
    graph.nodes[5].inputs.push(4);

    // <<present>> is what we need!
    graph.nodes[3].target = true;

    // print original graph
    printf("original graph:\n");
    for (usize i = 0; i < graph.nodes.count(); i++) {
        auto& node = graph.nodes[i];
        for (usize j = 0; j < node.outputs.count(); j++) {
            auto& other = graph.nodes[node.outputs[j]];
            printf("    <<%s>> ---> <<%s>>\n", node.name.data(), other.name.data());
        }
    }

    // fill initial ref count
    for (usize i = 0; i < graph.nodes.count(); i++) {
        auto& node = graph.nodes[i];
        node.refcount = node.outputs.count();
    }

    printf("begin culling:\n");

    u32 cull_count = 0;

    spargel::base::vector<u32> tmp;
    for (usize i = 0; i < graph.nodes.count(); i++) {
        auto& node = graph.nodes[i];
        if (node.refcount == 0 && !node.target) {
            node.culled = true;
            tmp.push(i);
            cull_count++;
            printf("    culled node <<%s>>\n", node.name.data());
        }
    }
    while (!tmp.empty()) {
        auto i = tmp[tmp.count() - 1];
        tmp.pop();
        auto& node = graph.nodes[i];
        for (usize j = 0; j < node.inputs.count(); j++) {
            auto& source = graph.nodes[node.inputs[j]];
            source.refcount--;
            if (source.refcount == 0) {
                source.culled = true;
                tmp.push(j);
                cull_count++;
                printf("    culled node <<%s>>\n", source.name.data());
            }
        }
    }

    printf("culled %d nodes\n", cull_count);

    printf("culled graph:\n");
    for (usize i = 0; i < graph.nodes.count(); i++) {
        auto& node = graph.nodes[i];
        if (node.culled) continue;
        for (usize j = 0; j < node.outputs.count(); j++) {
            auto& other = graph.nodes[node.outputs[j]];
            if (other.culled) continue;
            printf("    <<%s>> ---> <<%s>>\n", node.name.data(), other.name.data());
        }
    }

    // another approach

    printf("direct approach:\n    ");

    for (usize i = 0; i < graph.nodes.count(); i++) {
        if (graph.nodes[i].target) {
            tmp.push(i);
        }
    }
    spargel::base::vector<u32> result;
    while (!tmp.empty()) {
        auto i = tmp[tmp.count() - 1];
        tmp.pop();
        result.push(i);
        auto& node = graph.nodes[i];
        for (usize j = 0; j < node.inputs.count(); j++) {
            tmp.push(node.inputs[j]);
        }
    }

    for (auto iter = result.end() - 1; iter >= result.begin(); iter--) {
        printf("<<%s>>", graph.nodes[*iter].name.data());
        if (iter > result.begin()) {
            printf(" ---> ");
        }
    }
    printf("\n");

    return 0;
}
