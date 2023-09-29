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
add_full_require("libsigcplusplus")
add_full_require("infoware")
add_full_require("spdlog")
add_full_require("asio")
add_full_require("xxhash")
add_full_require("zlib")
add_full_require("utfcpp")
add_full_require("libcurl")
add_full_require("openssl")
add_full_require("magic_enum")
add_full_require("fmt")
add_full_require("cpr")
add_full_require("jwt-cpp")
add_full_require("http_parser")
add_full_require("restinio")
add_full_require("argparse")

if is_plat("windows") then
    add_full_require("winreg")
end

add_dynamic_require("qt6core","qt6gui","qt6network","qt6widgets")