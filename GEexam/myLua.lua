print("Hello from Lua!")

if not entity1 then
    entity1 = createEntity()
    setPosition(entity1, 0.0, 1.0, 12.0)
    setVelocity(entity1, 1.0, 0.0, 0.0)
    setColor(entity1, 1.0, 0.0, 0.0) -- Red
end

if not entity2 then
    entity2 = createEntity()
    setPosition(entity2, 3.0, 1.0, 0.0)
    setVelocity(entity2, -0.5, 0.0, 0.0)
    setColor(entity2, 0.0, 1.0, 0.0) -- Green
end

if not entity3 then
    entity3 = createEntity()
    setPosition(entity3, 20.0, 1.0, 0.5)
    setVelocity(entity3, 1.0, 0.0, 0.0)
    setColor(entity3, 0.0, 0.0, 1.0) -- Blue
end

if not entity4 then
    entity4 = createEntity()
    setPosition(entity4, 20.0, 1.0, 0.5)
    setVelocity(entity4, 1.0, 0.0, 0.0)
    setColor(entity4, 1.0, 0.0, 1.0) -- Pink
end

if not entity5 then
    entity5 = createEntity()
    setPosition(entity5, 20.0, 1.0, 0.5)
    setVelocity(entity5, 1.0, 0.0, 0.0)
    setColor(entity5, 1.0, 1.0, 1.0) -- White
end



