add_rules("mode.debug", "mode.release")

set_project("Musubi")

function add_full_require(name)
    if is_mode("release") then
        add_requires(name .. "~Static", {configs = {vs_runtime = "MT", debug = false}})
        add_requires(name .. "~Dynamic", {configs = {vs_runtime = "MD", debug = false}})
    else
        add_requires(name .. "~Static", {configs = {vs_runtime = "MTd", debug = true}})
        add_requires(name .. "~Dynamic", {configs = {vs_runtime = "MDd", debug = true}})
    end 
end

add_full_require("nlohmann_json")
add_full_require("crossguid")
add_full_require("zstd")
add_full_require("vcpkg::libsigcpp")
add_full_require("vcpkg::infoware")
add_full_require("spdlog")
add_full_require("winreg")
add_full_require("asio")
add_full_require("xxhash")
add_full_require("zlib")
add_full_require("utfcpp")
add_full_require("libcurl")
add_full_require("openssl")
add_full_require("nlohmann_json")
add_full_require("magic_enum")
add_full_require("fmt")
add_full_require("cpr")
add_full_require("vcpkg::jwt-cpp")
add_full_require("http_parser")
add_full_require("restinio")
add_full_require("argparse")

includes("src/**/xmake.lua")

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

