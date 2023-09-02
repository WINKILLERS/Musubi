add_requires("spdlog","http_parser","restinio","nlohmann_json","magic_enum","fmt","cpr","asio","argparse","openssl")

target("MusubiActivator")
    set_kind("binary")
    set_pcxxheader("AApch.h")
    set_languages("c++20")

    add_files("*.cpp")
    add_headerfiles("*.h")
    add_includedirs("$(projectdir)/src/MusubiActivator",{public=true})

    add_includedirs("$(projectdir)/src/MusubiActivator")

    add_packages("spdlog","http_parser","restinio","nlohmann_json","magic_enum","fmt","cpr","asio","argparse","openssl")