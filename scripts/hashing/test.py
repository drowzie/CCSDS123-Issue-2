import numpy as np
def generateConsZero(r):
    txt = ""
    for y in range(r):
        txt = txt + "0"
    return txt

def generate(low, high, outputhex):
    inputList = []
    outputCode = []
    for x in range(low,high+1):
        inputList.append(generateConsZero(x))
        outputCode.append(bin(outputhex + int(hex(x),16))[:1:-1])
    return inputList,outputCode
i0, O0 = generate(0,17,0x0)
i1, O1 = generate(18, 44, 0x12)


#print((bin(0x0 + int(hex(13),16))[:1:-1]) in O0 )

print(bin(0xFF) in O1)

