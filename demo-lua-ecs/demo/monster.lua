local monster = {}

monster.name = "Marcus"

function monster:OnStart()
	print("Monster created!")
end

function monster:OnUpdate()
	print(self.name .. " updated!")
end

return monster