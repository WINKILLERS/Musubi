rule("aux.magic")
    before_build(function (target)
        import("core.project.depend")
        import("core.cache.memcache")

        function setMagic(target)
            import("core.cache.memcache")

            math.randomseed(os.time())
            local magic = math.random(0xFFFFFFFF)
            memcache.set("aux.magic", "magic", magic)
        end

        local sources = {}
        local magic = memcache.get("aux.magic", "magic")

        for _, sourcebatch in pairs(target:sourcebatches()) do
            for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
                table.insert(sources, sourcefile)
            end
        end

        depend.on_changed(function()
            setMagic(target)

            print("rule.aux.magic: file changed, re-generating magic")
        end, {files = sources})

        if magic == nil then
            setMagic(target)
            
            print("rule.aux.magic: magic is nil, re-generating magic")
        end 
        
        magic = memcache.get("aux.magic", "magic")

        target:add("defines", "MAGIC="..magic)
    end)