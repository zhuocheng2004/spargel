set_policy("build.ccache", false)

add_rules("mode.debug")

set_languages("c++23")
add_includedirs(
    "$(projectdir)/public",
    "$(projectdir)/source"
)
includes("source/spargel/base")
