target("MusubiBridgeDynamic")
    if is_mode("release") then
        set_runtimes("MD")
    else
        set_runtimes("MDd")
    end 

    set_kind("static")
    set_languages("c++20")
    add_rules("aux.magic")

    add_files("*.cpp")
    add_headerfiles("*.h")
    add_includedirs("$(projectdir)/src/MusubiBridge",{public=true})

    add_packages("nlohmann_json~Dynamic")
    add_packages("crossguid~Dynamic")
    add_packages("zstd~Dynamic")

target("MusubiBridgeStatic")
    if is_mode("release") then
        set_runtimes("MT")
    else
        set_runtimes("MTd")
    end 

    set_kind("static")
    set_languages("c++20")
    add_rules("aux.magic")

    add_files("*.cpp")
    add_headerfiles("*.h")
    add_includedirs("$(projectdir)/src/MusubiBridge",{public=true})

    add_packages("nlohmann_json~Static")
    add_packages("crossguid~Static")
    add_packages("zstd~Static")