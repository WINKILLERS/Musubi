target("MusubiBridgeDynamic")
    set_kind("static")
    add_rules("aux.magic")
    add_rules("config.dynamic_mode")

    add_files("*.cpp")
    add_headerfiles("*.hpp")
    add_includedirs("$(projectdir)/src/MusubiBridge", {public = true})

    add_packages("nlohmann_json~Dynamic")
    add_packages("crossguid~Dynamic")
    add_packages("zstd~Dynamic")

target("MusubiBridgeStatic")
    set_kind("static")
    add_rules("aux.magic")
    add_rules("config.static_mode")

    add_files("*.cpp")
    add_headerfiles("*.hpp")
    add_includedirs("$(projectdir)/src/MusubiBridge", {public = true})

    add_packages("nlohmann_json~Static")
    add_packages("crossguid~Static")
    add_packages("zstd~Static")

target("MusubiBridgeTests")
    set_kind("binary")
    add_rules("config.dynamic_mode")

    set_default(false)
    add_files("tests/*.cpp")
    add_tests("test_generator", {runargs = {"[generator]"}})
    add_tests("test_handshake", {runargs = {"[handshake]"}})
    add_tests("test_header", {runargs = {"[header]"}})

    add_packages("catch2~Dynamic")

    add_deps("MusubiBridgeDynamic")
    