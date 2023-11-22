function add_dynamic_require(name, additional_config)
    full_name = name .. "~Dynamic"
    config = {configs = {shared = true}}

    if is_mode("release") then
        config.configs.vs_runtime = "MD"
        config.configs.debug = false
    else
        config.configs.vs_runtime = "MDd"
        config.configs.debug = true
    end 

    if additional_config ~= nil then
        for key, value in pairs(additional_config) do
            config.configs[key] = value
        end
    end 

    add_requires(full_name, config)
end

function add_static_require(name, additional_config)
    full_name = name .. "~Static"
    config = {configs = {shared = false}}

    if is_mode("release") then
        config.configs.vs_runtime = "MT"
        config.configs.debug = false
    else
        config.configs.vs_runtime = "MTd"
        config.configs.debug = true
    end 

    if additional_config ~= nil then
        for key, value in pairs(additional_config) do
            config.configs[key] = value
        end
    end 
    
    add_requires(full_name, config)
end

function add_full_require(name, additional_config)
    add_dynamic_require(name, additional_config)
    add_static_require(name, additional_config)
end

add_full_require("nlohmann_json")
add_full_require("crossguid")
add_full_require("zstd")
add_full_require("spdlog", {shared = false})
add_full_require("jwt-cpp")
add_full_require("fmt")
add_full_require("asio")
add_full_require("xxhash", {shared = false})
add_full_require("zlib")
add_full_require("utfcpp")
add_full_require("infoware")
add_full_require("libcurl")
add_full_require("openssl")
add_full_require("cpr")
add_full_require("libsigcplusplus")
add_full_require("magic_enum", {shared = false, debug = false})

add_dynamic_require("catch2")

if is_plat("windows") then
    add_full_require("winreg")
end