e = entity.create("monster.lua")

transform = {
	position = { 1, 2, 3},
	rotation = { 4, 5, 6},
	scale = {7, 8, 9}
}

print("Lua entity fields:")
for k,v in pairs(e) do print("\t" .. k,v) end

print("Lua entity meta fields:")
local mt = getmetatable(e)
for k,v in pairs(mt) do print("\t" .. k,v) end

e:set("transform", transform)
