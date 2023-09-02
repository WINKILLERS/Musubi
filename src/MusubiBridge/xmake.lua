add_requires("nlohmann_json","crossguid","zstd")

target("MusubiBridge")
    set_kind("static")
    set_languages("c++20")

    add_files("*.cpp")
    add_headerfiles("*.h")
    add_includedirs("$(projectdir)/src/MusubiBridge",{public=true})

    add_defines(building)

    add_packages("nlohmann_json","crossguid","zstd")