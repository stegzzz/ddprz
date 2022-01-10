"""DDGEM stage 1 data extraction

usage: dd/src/exe
    python dd/src/exe/getStage1DDGEM.py
    extracts stage 1 data from e178/data_TempDiscInd2_2021-07-23_16-49.csv 
    SHA1 DFFD97B33191C9198D0B4EF66F6E787B6E279AA4 saved as UTF8 from
    e178/data_TempDiscInd2_2021-07-23_16-49.xlsx SHA1 35B5D8742957B9B6CCEA3A9E8A5A8B33B239E7BF
    uses e178/prolificIDs.txt SHA1 706EE064E79E9A8052A612D54D2FD7F75686E931 cut/pasted from 
    e178/data_TempDiscInd2_2021-07-23_16-49.xlsx

    data written to e178/ddgem/stage1DataMain.txt and e178/ddgem/stage1DataFiller.txt
"""

import csv
from os.path import expanduser, join
home=expanduser("~")
#data sources
root="Documents/rwork/e178/"
fn=join(home,root,"data_TempDiscInd2_2021-07-23_16-49.csv")
idfn=join(home,root,"prolificIDs.txt")
ouf_M=join(home,root,"ddgem/stage1DataMain.txt")
ouf_F=join(home,root,"ddgem/stage1DataFiller.txt")
ouf_DALL=join(home,root,"ddgem/demoAll.txt")
ouf_DOK=join(home,root,"ddgem/demoOK.txt")
idfillersfn=join(home,root,"ddgem/jb_valid_ids.txt")

ids=[]#get prolific participant IDs
with open(idfn,'r') as idf:
    ids=idf.read().splitlines()

fids=[]#get participant IDs for filler data, all those with JB OK
with open(idfillersfn,'r') as idf:
    fids=idf.read().splitlines()

print(len(ids), " prolific IDs read")
print("Parsing stage 1 data from: "+fn)
fields=[] #header row
rows=[]   #data to be parsed
with open(fn,newline='\r\n') as s1data:#NB some comment fields have \n, these were being interpreted as newline, hence '\r\n' needed here
    reader=csv.reader(s1data)
    fields=next(reader)
    rn=1
    for row in reader:
        rows.append(row)
        rn+=1
    print("%d rows read (rn=%d)"%(reader.line_num,rn))
#find the col indices of the stage 1 questions
def getIndices(start: int, end: int, skip: int) :
    result=[]
    for i in range(0,end,skip) :
        result.append(start+i)
    return result

#responses
colIdxSeq1=getIndices(fields.index('S101_01'),160,2)#seq1
colIdxFill1=getIndices(fields.index('S101_81'),38,2)#fillers1
colIdxSeq2=getIndices(fields.index('S102_01'),160,2)#seq2
colIdxFill2=getIndices(fields.index('S102_81'),38,2)#fillers2

#delays/amounts for fillers
colIdxSA1=getIndices(fields.index('IV04_01'),19,1)#sa1
colIdxSD1=getIndices(fields.index('IV06_01'),19,1)#sd1
colIdxLA1=getIndices(fields.index('IV05_01'),19,1)#la1
colIdxLD1=getIndices(fields.index('IV07_01'),19,1)#ld1

colIdxSA2=getIndices(fields.index('IV08_01'),19,1)#sa2
colIdxSD2=getIndices(fields.index('IV10_01'),19,1)#sd2
colIdxLA2=getIndices(fields.index('IV09_01'),19,1)#la2
colIdxLD2=getIndices(fields.index('IV11_01'),19,1)#ld2

ageIdx=fields.index('DQ01_01')
genderIdx=fields.index('DQ02')
critOKidx=fields.index('IV02_01')
idIdx=fields.index('ID01s')
fields[0]='CASE' # remove unauthorised characters
caseIdx=fields.index('CASE')
#for i in colIdx:#print the field indexes and names
#    print(i,"->",fields[i])
nprolific=0
nprolificS1=0
nOK=0
goodRows=[]
n=0
allRows=[]
demoAll=[]
demoOK=[]
#check the number of prolific participants who meet the criteria for inclusion and store these rows
for j in rows:
    #print(j[idIdx]) #list the prolific IDs
    if(j[idIdx] in ids):
        nprolific+=1
        if(j[critOKidx].startswith('STOP')):
            nprolificS1+=1
            demoAll.append((j[caseIdx],j[ageIdx],j[genderIdx] if j[genderIdx]=='1' else j[genderIdx] if j[genderIdx]=='2' else j[genderIdx] if j[genderIdx]=='3' else '-1'))
        if(j[critOKidx]==''):
            print(j[caseIdx], " did not complete S1")   
        if(j[caseIdx] in fids and j[critOKidx]!=''):
            n+=1
            allRows.append(j) 
        if(j[critOKidx].startswith('OK')):
            nOK+=1
            goodRows.append(j)
            demoOK.append((j[caseIdx],j[ageIdx],j[genderIdx] if j[genderIdx]=='1' else j[genderIdx] if j[genderIdx]=='2' else j[genderIdx] if j[genderIdx]=='3' else '-1'))
            demoAll.append((j[caseIdx],j[ageIdx],j[genderIdx] if j[genderIdx]=='1' else j[genderIdx] if j[genderIdx]=='2' else j[genderIdx] if j[genderIdx]=='3' else '-1'))
print(nprolific, ' prolific IDs with ', nOK, ' OK and ', nprolificS1, ' failed on critJB or critExpK')

def writeDemo(fn,data):
    with open(fn,'w') as f:
        for r in data:
            f.write("%s\t%s\t%s\n"%r)

writeDemo(ouf_DALL,demoAll)
print("all demographics written to: ", ouf_DALL)
writeDemo(ouf_DOK,demoOK)
print("OK demographics written to: ", ouf_DOK)

with open(ouf_M,'w') as f:
    for g in goodRows:
        f.write("%s\t"%g[caseIdx])
        for i in colIdxSeq1:
            f.write("%s\t"%g[i])
        for i in colIdxSeq2:
            f.write("%s\t"%g[i])
        f.write("\n")
print("main stage 1 data written to: ", ouf_M)#note delays/amounts will be assigned later (in r based on xmlTagValueMap.txt)

dmap={'1 week': 0.25, '2 weeks': 0.5, '1 month': 1, '2 months': 2, '3 months': 3, '4 months': 4, '6 months': 6, '9 months': 9, '1 year': 12, '18 months': 18, '2 years': 24, '3 years': 36}

with open(ouf_F,'w') as f:
    for g in allRows: #NB here we can choose all of those who only meet the JB criterion. jb_valid_ids.txt written by e178/ddgem/dataAnalysis.r
        f.write("%s\t"%g[caseIdx])
        for i in colIdxFill1:
            f.write("%s\t"%g[i])
        for i in colIdxSA1:
            f.write("%s\t"%g[i])
        for i in colIdxSD1:
            f.write("%s\t"%dmap[g[i]])
        for i in colIdxLA1:
            f.write("%s\t"%g[i])
        for i in colIdxLD1:
            f.write("%s\t"%dmap[g[i]])
        for i in colIdxFill2:
            f.write("%s\t"%g[i])
        for i in colIdxSA2:
            f.write("%s\t"%g[i])
        for i in colIdxSD2:
            f.write("%s\t"%dmap[g[i]])
        for i in colIdxLA2:
            f.write("%s\t"%g[i])
        for i in colIdxLD2:
            f.write("%s\t"%dmap[g[i]])
        f.write("\n")
print("%i\tcase of filler stage 1 data written to: "%n, ouf_F)
