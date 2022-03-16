local function myFunc(message, ...)
	local args = table.pack(...)
	for i,v in ipairs(args) do
		print(message, v)
	end
end

myFunc("What is this?", 4, "Hello", 5, "oof")