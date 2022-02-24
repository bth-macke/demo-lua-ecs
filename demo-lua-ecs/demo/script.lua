score = 0
print("[entity="..me.."] Loaded!")

function OnCreate()
	print("I'm created!")
end

function OnUpdate()
	score = score + 1
	print("My score is " .. score)
end
