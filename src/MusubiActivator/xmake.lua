if is_mode("release") then
    set_runtimes("MD")
else
    set_runtimes("MDd")
end 

target("MusubiActivator")
    set_kind("binary")
    set_pcxxheader("AApch.h")
    set_languages("c++20")

    add_files("*.cpp")
    add_headerfiles("*.h")
    add_includedirs("$(projectdir)/src/MusubiActivator",{public=true})

    add_includedirs("$(projectdir)/src/MusubiActivator")

    add_packages("spdlog~Dynamic")
    add_packages("openssl~Dynamic")
    add_packages("jwt-cpp~Dynamic")
    add_packages("http_parser~Dynamic")
    add_packages("restinio~Dynamic")
    add_packages("nlohmann_json~Dynamic")
    add_packages("magic_enum~Dynamic")
    add_packages("fmt~Dynamic")
    add_packages("cpr~Dynamic")
    add_packages("asio~Dynamic")
    add_packages("argparse~Dynamic")