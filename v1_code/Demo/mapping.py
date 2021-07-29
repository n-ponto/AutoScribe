possibilities = [x for x in range(2**4)]

output = []
for x in possibilities:
    up: bool = bool(x & 0b1000)
    down: bool = bool(x & 0b0100)
    left: bool = bool(x & 0b0010)
    right: bool = bool(x & 0b0001)

    this: tuple
    if x > 12: # more than two chosen
        pass
    elif (up & down):
        pass
    elif (right & left):
        