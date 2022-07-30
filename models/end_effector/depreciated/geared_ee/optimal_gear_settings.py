tooth_range = range(30, 60)
print("Range of teeth is: ")
print(tooth_range)

# try tooth and module combos to get the best one
gearDiamA = 65.78
gearDiamB = gearDiamA / 2

#module
module_range = [1, 1.125, 1.25, 1.375, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3, 3.5, 4, 4.5, 5, 5.5, 6, 6.5, 7]

def closest(lst, K):
    return lst[min(range(len(lst)), key = lambda i : abs(lst[i] - K))]

def closestTN(K):
    return closest(tooth_range, K)

organized_list = []

# module = diameter / tooth_number
# tooth_number = diameter / module
for module in module_range:
    # Get closest tooth value for Gear A
    optimalToothNumA = gearDiamA / module
    closestToothNumA = closestTN(optimalToothNumA)
    if closestToothNumA > optimalToothNumA:
        closestToothNumA -= 1
    closestDiameterA = closestToothNumA * module
    # Get closest tooth value for Gear B
    optimalToothNumB = gearDiamB / module
    closestToothNumB = closestTN(optimalToothNumB)
    closestDiameterB = closestToothNumB * module
    if closestToothNumB > optimalToothNumB:
        closestToothNumB -= 1
    #  Calculate squared loss
    squaredLoss = (gearDiamA - closestDiameterA) ** 2 + (gearDiamB - closestDiameterB) ** 2
    # Add tuple into list
    singleDict = {
        "SquaredLoss": squaredLoss, 
        "ToothNumA": closestToothNumA, 
        "ToothNumB": closestToothNumB, 
        "Module": module
        }
    # if (closestDiameterA - gearDiamA + closestDiameterB - gearDiamB <= 0):
    organized_list.append(singleDict)

organized_list.sort(key=lambda x: x["SquaredLoss"])

for thng in organized_list:
    print (thng)
