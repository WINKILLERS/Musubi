rule("aux.magic")
    on_build(function (target)
        import("core.project.depend")

        local sources = {}
        local magic = 0
        math.randomseed(os.time())

        for _, sourcebatch in pairs(target:sourcebatches()) do
            for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
                table.insert(sources, sourcefile)
            end
        end

        if _g.sources ~= sources then
            _g.magic = math.random(0xFFFFFFFF)
        end

        magic = _g.magic
        _g.sources = sources

        target:add("defines", "MAGIC="..magic)
    end)