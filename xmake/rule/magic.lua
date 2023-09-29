rule("aux.magic")
    before_build(function (target)
        import("core.project.depend")

        function setMagic(target)
            math.randomseed(os.time())
            local magic = math.random(0xFFFFFFFF)
            local path = path.join(os.tmpdir(), "magic")
            io.writefile(path, magic)
        end

        local sources = {}
        local path = path.join(os.tmpdir(), "magic")
        print("magic file: "..path)

        if os.exists(path) ~= true then
            setMagic(target)
            
            print("rule.aux.magic: magic is nil, re-generating magic")
        end

        for _, sourcebatch in pairs(target:sourcebatches()) do
            for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
                table.insert(sources, sourcefile)
            end
        end

        depend.on_changed(function()
            setMagic(target)

            print("rule.aux.magic: file changed, re-generating magic")
        end, {files = sources})
        
        local magic = io.readfile(path)

        target:add("defines", "MAGIC="..magic)
    end)