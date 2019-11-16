import tabula
import csv
#df = tabula.read_pdf("a.pdf", pages='78', output_format="json")

tabula.convert_into("a.pdf", "test.csv", output_format="csv", pages='78')

table = "0"


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
        if(row[0] == "Active Prefix"):
            break
        if(line_count > 1):
            inputCodes0.append(row[0])
            inputCodes1.append(row[2])
            inputCodes2.append(row[4])
            outputCodes0.append(row[1])
            outputCodes1.append(row[3])
            outputCodes2.append(row[5])
            line_count+=1
        else:
            line_count+=1
inputList = inputCodes0 + inputCodes1 + inputCodes2
outputList = outputCodes0 + outputCodes1 + outputCodes2

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
    txt = "0^{r}, 0≤r≤20"
    x = txt.replace("}", "")
    x = x.split(",")
    number = x[0].split("^{")
    minMax = x[1].split("≤r≤")
    return number[0], minMax

file = open("codeTable.c", "w+")
file.write("char codeTable" + table +"[][128] = {\r\n")
for i in inputList:
    file.write('"'+i+'"'+",")
file.write("\r\n}")
file.close()
