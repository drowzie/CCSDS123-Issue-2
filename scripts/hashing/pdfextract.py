import tabula
import csv

#functions
# Generate consecutive zeros based on r
def generateConsZero(r):
    txt = ""
    for y in range(r):
        txt = txt + "0"
    return txt

# generate input code from low <= r <= high into rzeros + outputhex
def generate(low, high, outputhex):
    inputList = []
    outputCode = []
    for x in range(low,high+1):
        inputList.append(generateConsZero(x))
        outputCode.append(bin(outputhex + int(hex(x),16))[:1:-1])
    return inputList,outputCode

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
    x = txt.replace("}", "")
    x = x.split(",")
    number = x[0].split("^{")
    minMax = x[1].split("≤r≤")
    return number[0], minMax


#Returns bitsize and hexvalue
def extractOutputCode(outputTxt):
    print(outputTxt)
    if("'" in outputTxt):
        txt = outputTxt.split("'h")
    else:
        txt = outputTxt.split("’h")
    return txt[0], txt[1]


tabula.convert_into("a.pdf", "test.csv", output_format="csv", pages='90')

table = "10"

with open('test.csv', mode='r') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    line_count = 0
    inputCodes0 = []
    inputCodes1 = []
    inputCodes2 = []

    outputCodes0 = []
    outputCodes1 = []
    outputCodes2 = []

    for enum, row in enumerate(csv_reader):
        #if(row[0] == "Active Prefix"):
        #    break
        if(line_count > 1):
            print(row)
            inputCodes0.append(row[0])
            inputCodes1.append(row[2])
            #inputCodes2.append(row[4])
            outputCodes0.append(row[1])
            outputCodes1.append(row[3])
            #outputCodes2.append(row[5])
            line_count+=1
        else:
            line_count+=1
inputList = inputCodes0 + inputCodes1 + inputCodes2
outputList = outputCodes0 + outputCodes1 + outputCodes2

file = open("codeTable"+table+".c", "w+")
file.write("char codeTable" + table +"[][128] = {\r\n")
for i in inputList:
    #if("r" in i):

    if("^" in i):
        i = fullNumberFromString(i)
    file.write('"'+i+'"'+",")
file.write("\r\n};")

bitsizes = []
outPutValues = []
for i in outputList:
    if(not("'" in i or "’" in i)):
        continue
    bitsize, outputvalue = extractOutputCode(i)
    bitsizes.append(bitsize)
    outPutValues.append(outputvalue)
file.write("uint32_t outPutCodes" + table +"[] = {\r\n")
for i in outPutValues:
    file.write('0x'+i+",")
file.write("\r\n};")
file.write("uint8_t bitSizes" + table +"[] = {\r\n")
for i in bitsizes:
    file.write(i+", ")
file.write("\r\n};")


file.close()
