add_requires("spdlog","zlib","utfcpp","vcpkg::libsigcpp","vcpkg::infoware","libcurl","openssl","nlohmann_json","magic_enum","fmt","cpr")

target("MusubiNativeClient")
    set_kind("binary")
    set_pcxxheader("AApch.h")
    set_languages("c++20")

    add_files("MusubiNativeClient.cpp")
    add_files("**/*.cpp")
    add_headerfiles("**/*.h")

    add_includedirs("$(projectdir)/src/MusubiNativeClient")

    add_packages("spdlog","zlib","utfcpp","vcpkg::libsigcpp","vcpkg::infoware","libcurl","openssl","nlohmann_json","magic_enum","fmt","cpr")
    add_deps("MusubiBridge")
    add_syslinks("gdi32","Secur32.lib","wbemuuid.lib","OleAut32.lib","Shell32.lib","ntdll.lib","user32")