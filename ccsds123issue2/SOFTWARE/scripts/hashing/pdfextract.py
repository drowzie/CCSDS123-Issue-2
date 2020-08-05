#functions
# Generate consecutive zeros based on r
def generateConsZero(r):
    txt = ""
    for y in range(r):
        txt = txt + "0"
    return txt

# generate input code from low <= r <= high into rzeros + outputhex
#Return inputlist, outputcodes, and their bitsizes
def generate(low, high, inputSuffix, outputhex, bitsize, multiple):
    inputList = []
    outputCode = []
    bitSizes = []
    reverse = []
    for x in range(low,high+1):
        inputList.append(generateConsZero(x) + inputSuffix)
        outputCode.append(hex(outputhex + (multiple * int(hex(x),16))))
        bitSizes.append(bitsize)
        reverse.append(1)
    return inputList,outputCode, bitSizes, reverse

def reverseHex(number):
    x = bin(int(number))[:1:-1]
    value = int(x, 2)
    return hex(value)

# Turn Eg. 0^{2}2324X to 0002324X
# Check for ^ in text
def fullNumberFromString(txt):
    txt = txt.split("}")
    reps = txt[0].split("^{")
    returnString = ""
    for x in range(int(reps[1])):
        returnString = returnString + reps[0]
    return returnString + txt[1]

# Check for r in text
def extractRFromString(txt):
    x = txt.split(",")
    lastWord = x[0].replace("0^{r}", "")
    minMax = x[1].split("≤r≤")
    return lastWord, [int(minMax[0]), int(minMax[1])]

#Returns bitsize and hexvalue
def extractOutputCode(outputTxt):
    if("'" in outputTxt):
        txt = outputTxt.split("'h")
    else:
        txt = outputTxt.split("’h")
    return txt[0], hex(int(txt[1], 16))

# Extract hex and multiple value from eg. FFB from (FFB + 2 * r)
def extractOutputHex(txt):
    txt = txt.split("+")
    txt[0] = txt[0].replace("(", "")
    multiple = 1
    if("2" in txt[1]):
        multiple = 2
    return int(txt[0],16), multiple


# Extract numbits + outputhex for functoin generate
def extractOutputIfReverse(outputTxt):
    outputTxt = outputTxt.replace("<", "")
    outputTxt = outputTxt.replace(">", "")
    if("'" in outputTxt):
        txt = outputTxt.split("'h")
    else:
        txt = outputTxt.split("’h")
    return txt[0], extractOutputHex(txt[1])

test = "0^{r}X, 0≤r≤1"
test2 = "<12'h(FFA+r)>"

# input of format "0^{r}X, 0≤r≤1", output: "<12'h(FFA+r)>", remember to check for <>
def generateInputOutputR(input, output):
    suffixCode, rRange = extractRFromString(input)
    bitsize, outputHex = extractOutputIfReverse(output)
    return generate(rRange[0], rRange[1], suffixCode, outputHex[0], bitsize, outputHex[1])

file2 = open("codeTable.c", "w+")

for j in range(0, 16):
    if j < 10:
        table = "0" + str(j)
    else:
        table = str(j)
    file = open ("code_" + table + ".txt")

    inputCodes0 = []
    inputCodes1 = []
    inputCodes2 = []

    outputCodes0 = []
    outputCodes1 = []
    outputCodes2 = []


    for line in file:
        split = line.split(", ")
        inputCodes0.append(split[0])
        outputCodes0.append(split[1])

    inputList = inputCodes0 + inputCodes1 + inputCodes2
    outputList = outputCodes0 + outputCodes1 + outputCodes2

    bitsizes = []
    outPutValues = []
    inputcodes = []
    ifReverse = []
    for i,o in zip(inputList, outputList):
        if(i):
            if("^{r}" in i):
                inp, output, bitsize, reverse = generateInputOutputR(i,o)
                inputcodes.extend(inp)
                outPutValues.extend(output)
                bitsizes.extend(bitsize)
                ifReverse.extend(reverse)
            elif("^" in i):
                inCode = fullNumberFromString(i)
                bitsize, outputvalue = extractOutputCode(o)
                bitsizes.append(bitsize)
                outPutValues.append(outputvalue)
                inputcodes.append(inCode)
                ifReverse.append(0)
            else:
                bitsize, outputvalue = extractOutputCode(o)
                outPutValues.append(outputvalue)
                bitsizes.append(bitsize)
                inputcodes.append(i)
                ifReverse.append(0)


    file2.write("char codeTable"+ str(j) +"[][256] = {\n")
    for v in inputcodes:
        file2.write('"'+ v + '"' + ", ")
    file2.write("\n};\n")

    file2.write("uint32_t outputCodes" + str(j) + "[] = {\n")
    for v in outPutValues:
        file2.write(str(v)+", ")
    file2.write("\n};\n")

    file2.write("uint8_t bitSizes"+str(j)+"[] = {\n")
    for v in bitsizes:
        file2.write(v + ", ")
    file2.write("\n};\n")


file.close()
