function add_dynamic_require(name)
    if is_mode("release") then
        add_requires(name .. "~Dynamic", {configs = {vs_runtime = "MD", debug = false}})
    else
        add_requires(name .. "~Dynamic", {configs = {vs_runtime = "MDd", debug = true}})
    end 
end

function add_static_require(name)
    if is_mode("release") then
        add_requires(name .. "~Static", {configs = {vs_runtime = "MT", debug = false}})
    else
        add_requires(name .. "~Static", {configs = {vs_runtime = "MTd", debug = true}})
    end 
end

function add_full_require(name)
    add_dynamic_require(name)
    add_static_require(name)
end

add_full_require("nlohmann_json")
add_full_require("crossguid")
add_full_require("zstd")
add_full_require("spdlog")
add_full_require("magic_enum")

add_dynamic_require("catch2")

add_dynamic_require("qt6core","qt6gui","qt6network","qt6widgets")