set_xmakever("2.8.2")
set_project("frozen")

set_version("1.1.0", { build = "%Y%m%d%H%M" })

option("enable_module", { defines = "EXAMPLES_USE_MODULE" })
option("enable_tests")
option("enable_benchmark")

target("frozen", function()
    set_languages("c++latest")
    if get_config("enable_module") then
        set_kind("object")
    else
        set_kind("headeronly")
    end

    add_includedirs("include", { public = true })
    add_headerfiles("include/frozen/**.hpp")
    if get_config("enable_module") then
        add_files("module/frozen.cppm", { install = true })
    end
end)

if get_config("enable_benchmark") then
    add_requires("benchmark")
    target("frozen.benchmark", function()
        set_kind("binary")
        set_languages("c++17")

        add_files("benchmarks/**.cpp")

        if not get_config("enable_benchmark_str_search") then
            remove_files("benchmarks/bench_str_search.cpp")
        end

        add_packages("benchmark")

        add_deps("frozen")
    end)
end

if get_config("enable_tests") then
    target("frozen.tests", function()
        set_kind("binary")
        set_languages("c++latest")
        add_rules("mode.coverage")
        add_files("tests/**.cpp")
        remove_files("tests/no_exceptions.cpp")

        add_cxxflags("/W3", "/WX", { tools = { "cl", "clang_cl" } })
        add_cxxflags("-Wall", "-Wextra", "-Wpedantic", "-Werror", "-Wshadow", { tools = { "cl", "clang_cl" } })

        if is_plat("windows") then
            add_cxxflags("/W3", "/WX", { tools = "icc" })
        else
            add_cxxflags("-Wall", "-Werror", { tools = "icc" })
        end

        add_deps("frozen")
    end)

    target("frozen.tests.noexcept", function()
        set_kind("binary")
        set_languages("c++latest")
        add_rules("mode.coverage")
        set_basename("test_no_exceptions")

        add_files("tests/no_exceptions.cpp")

        add_cxxflags("/W3", "/WX", { tools = { "cl", "clang_cl" } })
        add_cxxflags("-Wall", "-Wextra", "-Wpedantic", "-Werror", "-Wshadow", { tools = { "cl", "clang_cl" } })

        if is_plat("windows") then
            add_cxxflags("/W3", "/WX", { tools = { "icc" } })
        else
            add_cxxflags("-Wall", "-Werror", { tools = { "icc" } })
        end

        add_deps("frozen")
    end)

    for _, example in ipairs(os.files("examples/*.cpp")) do
        target("frozen.example." .. path.basename(example), function()
            set_kind("binary")
            set_languages("c++latest")

            add_files(example)

            add_deps("frozen")

            add_options("enable_module")

            if path.basename(example) == "html_entities_map" then
                add_cxxflags("-fconstexpr-steps=123456789", { tools = "clang" })
                add_cxxflags("-fconstexpr-ops-limit=12345678910", { tools = "gcc" })
            end
        end)
    end
end
