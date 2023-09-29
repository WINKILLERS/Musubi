if is_mode("release") then
    set_runtimes("MD")
else
    set_runtimes("MDd")
end 

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

    add_packages("spdlog~Dynamic")
    add_packages("utfcpp~Dynamic")
    add_packages("zlib~Dynamic")
    add_packages("libcurl~Dynamic")
    add_packages("openssl~Dynamic")
    add_packages("jwt-cpp~Dynamic")
    add_packages("nlohmann_json~Dynamic")
    add_packages("magic_enum~Dynamic")
    add_packages("fmt~Dynamic")
    add_packages("cpr~Dynamic")

    add_packages("qt6core~Dynamic")
    add_packages("qt6gui~Dynamic")
    add_packages("qt6network~Dynamic")
    add_packages("qt6widgets~Dynamic")
    
    add_frameworks("QtNetwork","QtGui","QtCore")

    add_deps("MusubiBridgeDynamic","MusubiActivator")