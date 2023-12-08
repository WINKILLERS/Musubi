target("MusubiServer")
    add_rules("qt.widgetapp")
    add_rules("config.dynamic_mode")
    set_languages("c++20")

    add_files("main.cpp")
    add_files("MusubiServer.qrc")
    add_files("**/*.cpp")
    add_files("**/*.hpp")
    add_headerfiles("**/*.hpp")
    add_files("**/*.ui")

    add_includedirs("$(projectdir)/src/MusubiServer")

    add_packages("spdlog~Dynamic")
    add_packages("magic_enum~Dynamic")
    add_packages("jwt-cpp~Dynamic")
    add_packages("nlohmann_json~Dynamic")
    add_packages("fmt~Dynamic")
    add_packages("utfcpp~Dynamic")

    add_frameworks("QtNetwork","QtGui","QtCore")

    add_deps("MusubiBridgeDynamic")

    add_defines("_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS")
