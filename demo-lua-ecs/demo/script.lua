e = {}

e.score = 0

function e:OnCreate()
	print("I'm created!")
end

function e:OnUpdate()
	self.score = self.score + 1
	print("My score is " .. self.score)
end

return e
