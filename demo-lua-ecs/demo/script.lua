local entity = {}

function entity:OnAttach()
	print("[Lua] Hello, my name is " .. (handle or "unknown") .. "!")
	for k,v in pairs(_ENV) do print(k,v) end
end

function entity:OnUpdate()
	print("[Lua] I'm (" .. (handle or "unknown") .. ") is updating!")
end

return entity;