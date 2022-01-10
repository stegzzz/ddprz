"""DDGEM stage 2 data extraction

usage: dd/src/exe
    python dd/src/exe/getStage2DDGEM.py
    extracts stage 2 data from e178/data_TempDiscInd2_2021-07-23_16-49.csv 
    SHA1 DFFD97B33191C9198D0B4EF66F6E787B6E279AA4 saved as UTF8 from
    e178/data_TempDiscInd2_2021-07-23_16-49.xlsx SHA1 35B5D8742957B9B6CCEA3A9E8A5A8B33B239E7BF
    uses e178/prolificIDs.txt SHA1 706EE064E79E9A8052A612D54D2FD7F75686E931 cut/pasted from 
    e178/data_TempDiscInd2_2021-07-23_16-49.xlsx

    data written to e178/ddgem/stage2Data.txt
"""

import csv
from os.path import expanduser, join
home=expanduser("~")
#data sources
root="Documents/rwork/e178/"
fn=join(home,root,"data_TempDiscInd2_2021-07-23_16-49.csv")
idfn=join(home,root,"prolificIDs.txt")
ouf=join(home,root,"ddgem/stage2Data.txt")

ids=[]#get prolific participant IDs
with open(idfn,'r') as idf:
    ids=idf.read().splitlines()

print(len(ids), " prolific IDs read")
print("Parsing stage 2 data from: "+fn)
fields=[] #header row
rows=[]   #data to be parsed
with open(fn,newline='\r\n') as s2data:#NB some comment fields have \n, these were being interpreted as newline, hence '\r\n' needed here
    reader=csv.reader(s2data)
    fields=next(reader)
    rn=1
    for row in reader:
        rows.append(row)
        rn+=1
    print("%d rows read (rn=%d)"%(reader.line_num,rn))
#find the col indices of the stage 2 questions
firstTTIdx=fields.index('S117_01')
colIdx=[]
for i in range(0,48,2) :
    colIdx.append(firstTTIdx+i)

critOKidx=fields.index('IV02_01')
idIdx=fields.index('ID01s')
fields[0]='CASE' # remove unauthorised characters
caseIdx=fields.index('CASE')
#for i in colIdx:#print the field indexes and names
#    print(i,"->",fields[i])
nprolific=0
nOK=0
goodRows=[]
#check the number of prolific participants who meet the criteria for inclusion and store these rows
for j in rows:
    #print(j[idIdx]) #list the prolific IDs
    if(j[idIdx] in ids):
        nprolific+=1
        if(j[critOKidx].startswith('OK')):
            nOK+=1
            goodRows.append(j)
print(nprolific, ' prolific IDs with ', nOK, ' OK')

with open(ouf,'w') as f:
    for g in goodRows:
        f.write("%s\t"%g[caseIdx])
        for i in colIdx:
            f.write("%s\t"%g[i])
        f.write("\n")
print("data written to: ", ouf)
