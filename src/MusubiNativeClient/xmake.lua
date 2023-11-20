target("MusubiNativeClient")
    set_kind("binary")
    add_rules("config.static_mode")
    set_languages("c++20")

    add_files("MusubiNativeClient.cpp")
    add_files("**/*.cpp")
    add_headerfiles("**/*.hpp")

    add_includedirs("$(projectdir)/src/MusubiNativeClient")

    add_packages("spdlog~Static")
    add_packages("winreg~Static")
    add_packages("asio~Static")
    add_packages("xxhash~Static")
    add_packages("zlib~Static")
    add_packages("utfcpp~Static")
    add_packages("libsigcplusplus~Static")
    add_packages("infoware~Static")
    add_packages("libcurl~Static")
    add_packages("openssl~Static")
    add_packages("nlohmann_json~Static")
    add_packages("magic_enum~Static")
    add_packages("fmt~Static")
    add_packages("cpr~Static")
    
    add_deps("MusubiBridgeStatic")

    add_syslinks("gdi32.lib","Secur32.lib","wbemuuid.lib","OleAut32.lib","Shell32.lib","ntdll.lib","user32.lib","Advapi32.lib")