add_requires("spdlog","qt6core","qt6network","zlib","libcurl","openssl","nlohmann_json","magic_enum","fmt","cpr")

target("MusubiServer")
    add_rules("qt.widgetapp")
    set_pcxxheader("AApch.h")
    set_languages("c++20")

    add_files("main.cpp")
    add_files("MusubiServer.qrc")
    add_files("**/*.cpp")
    add_files("**/*.h")
    add_headerfiles("**/*.h")
    add_files("**/*.ui")

    add_includedirs("$(projectdir)/src/MusubiServer")

    add_packages("spdlog","qt6core","qt6network","zlib","libcurl","openssl","nlohmann_json","magic_enum","fmt","cpr")
    add_frameworks("QtNetwork","QtGui","QtCore")
    add_deps("MusubiBridge","MusubiActivator")