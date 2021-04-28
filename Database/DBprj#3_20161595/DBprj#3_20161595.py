from konlpy.tag import Mecab
from pymongo import MongoClient
import pdb
stop_word=dict()
DBname="db20161595"
conn=MongoClient('localhost')
db=conn[DBname]
db.authenticate(DBname, DBname)

class tree(object):
    def __init__(self, word, count=0):
        self.word=word
        self.count=count
        self.parent=None
        self.child=dict()
        self.next=None
        self.trace=0
    def add(self, child,count):
        ret=0
        if child.word not in self.child:
            self.child[child.word]=child
            child.parent=self
            ret=1
        self.child[child.word].count+=count
        return ret
def make_stop_word():
    f=open("wordList.txt","r")
    while True:
        line=f.readline()
        if not line:
            break
        stop_word[line.strip()]=True
    f.close();

def p0():
    col1=db['news']
    col2=db['news_freq']
    
    col2.drop()

    for doc in col1.find():
        contentDic=dict()
        for key in doc.keys():
            if key !="_id":
                contentDic[key]=doc[key]
        col2.insert(contentDic)
def p1():
    for doc in db['news_freq'].find():
        doc['morph']=morphing(doc['content'])
        db['news_freq'].update({"_id":doc['_id']},doc)

def morphing(content):
    mecab=Mecab()
    morphList=[]
    for word in mecab.nouns(content):
        if word not in stop_word:
            morphList.append(word)
    return morphList
def p2():
    col1=db['news_freq']
    doc=col1.find_one()
    for i in doc['morph']:
        print(i)
def p3():
    col1=db['news_freq']
    col2=db['news_wordset']
    col2.drop()
    for doc in col1.find():
        new_doc=dict()
        new_set=set()
        for w in doc['morph']:
            new_set.add(w)
        new_doc['word_set']=list(new_set)
        new_doc['news_freq_id']=doc['_id']
        col2.insert(new_doc)

def p4():
    col1=db['news_wordset']
    doc=col1.find_one()
    for i in doc['word_set']:
        print(i)

def newitemsupporttree(transactions,mSupport):
    freitemDic=dict()
    for trans in transactions:
        for word in trans:
            if(word in freitemDic):
                freitemDic[word]=freitemDic[word]+transactions[trans];
            else:
                freitemDic[word]=transactions[trans];
    for word in list(freitemDic):
        if (freitemDic[word]<mSupport):
            del(freitemDic[word])

    freitemset=set(freitemDic.keys())

    if len(freitemset)==0:
        return None

    temptransactions=[]#######transaction, items의 mSupport를 제거한 표를 만들기 위해서
    for trans in transactions:
        temp=[]
        for word in trans:
            if word in freitemDic:
                temp.append([word,freitemDic[word],transactions[trans]])
        temptransactions.append(temp)

    transactions=[]
    for trans in temptransactions:#정렬된 transaction, items 표
        temp=sorted(trans, key=(lambda x: (x[1], x[0])), reverse=True)
        #temp=trans
        temp1=[]
        for i in temp:
            temp1.append([i[0],i[2]])
        transactions.append(temp1)

    itemsupport=dict()##item, support 표
    nextpointer=dict()
    for word in freitemDic:
        itemsupport[word]=[freitemDic[word],None]

    root=tree(None)
    for trans in transactions:##트리생성
        ptr=root
        for i in trans:#각 트랜스액션당 가장 freqeunt한것부터
            child=tree(i[0])
            if(ptr.add(child, i[1])):#ptr의 child로 child가 없었을때만 nextpointer에 이어줘야함.
                if(itemsupport[ptr.child[i[0]].word][1]==None):
                    itemsupport[ptr.child[i[0]].word][1]=ptr.child[i[0]]
                    nextpointer[i[0]]=ptr.child[i[0]]
                else:
                    nextpointer[i[0]].next=ptr.child[i[0]]
                    nextpointer[i[0]]=ptr.child[i[0]]
            ptr=ptr.child[i[0]]
    return itemsupport

def minetree(itemsupport, mSupport, prefix, freitemset, itemcount):
    sortedfreitem=sorted(itemsupport.items(), key=(lambda x:x[1][0]))##item, support 내림차순 정렬
    for i in sortedfreitem:
        newprefix=prefix.copy()
        newprefix.add(i[0])
        itemcount[frozenset(newprefix)]=itemsupport[i[0]][0]
        freitemset.append(newprefix)
        ptr=itemsupport[i[0]][1]
        patterns=dict()
        while ptr!=None:
            tempptr=ptr
            path=[]
            while ptr.word!=None:
                path.append(ptr.word)
                ptr=ptr.parent
            ptr=tempptr
            if len(path)>1:
                patterns[frozenset(path[1:])]=ptr.count
            ptr=ptr.next
        conitemsupport=newitemsupporttree(patterns, mSupport)
        if(conitemsupport!=None):
            minetree(conitemsupport, mSupport, newprefix, freitemset,itemcount)
    ###########################
def p5(length):
    freitemDic=dict()
    col1=db['news_wordset']
    mSupport=0.04*col1.find().count()

    for doc in col1.find():#기사가 하나씩 doc으로 들어감
        for word in doc['word_set']:###doc의 형태소들을 하나하나 확인
            if word in freitemDic:
                freitemDic[word]=freitemDic[word]+1
            else:
                freitemDic[word]=1;
    for word in list(freitemDic):
        if(freitemDic[word]<mSupport):
            del(freitemDic[word])
            
    temptransactions=[]#######transaction, items의 정렬된  표를 만들기 위해서
    for doc in col1.find():
        temp=[]
        for word in doc['word_set']:
            if word in freitemDic:
                temp.append([word, freitemDic[word]])
        temptransactions.append(temp)
    
    sortedfreitem=sorted(freitemDic.items(), key=(lambda x:x[1]), reverse=True)##item, support 내림차순 정렬 

    transactions=[]
    for trans in temptransactions:#정렬된 transaction, items 표
        temp=sorted(trans, key=(lambda x: x[1]), reverse=True)
        temp1=[]
        for i in temp:
            temp1.append(i[0])
        transactions.append(temp1)
   
    itemsupport=dict()##item, support 표
    nextpointer=dict()
    for word in sortedfreitem:
        itemsupport[word[0]]=[word[1],None]

    ##################tree 생성
    root=tree(None)
    for trans in transactions:##트리생성
        ptr=root
        for i in trans:#각 트랜스액션당 가장 freqeunt한것부터
            child=tree(i)
            if(ptr.add(child,1)):#ptr의 child로 child가 없었을때만 nextpointer에 이어줘야함.
                if(itemsupport[ptr.child[i].word][1]==None):
                    itemsupport[ptr.child[i].word][1]=ptr.child[i]
                    nextpointer[i]=ptr.child[i]
                else:
                    nextpointer[i].next=ptr.child[i]
                    nextpointer[i]=ptr.child[i]
            ptr=ptr.child[i]
    freitemset=[]
    freitemcount=dict()
    minetree(itemsupport, mSupport, set([]),freitemset,freitemcount)
    #print(freitemset)
    #print(freitemcount)
    
    if(length==1):
        col2=db['candidate_L1']
        col2.drop()
        for i in freitemcount:
            if(len(i)==1):
                col2.insert({"item_set":list(i), "support":freitemcount[i]})
    if (length==2):
        col2=db['candidate_L2']
        col2.drop()
        for i in freitemcount:
            if(len(i)==2):
                col2.insert({"item_set":list(i), "support":freitemcount[i]})
    if (length ==3):
        col2=db['candidate_L3']
        col2.drop()
        for i in freitemcount:
            if(len(i)==3):
                #print(list(i), freitemcount[i])
                col2.insert({"item_set":list(i), "support":freitemcount[i]})

def p6(length):
    minconf = 0.8
    col1 = db['candidate_L1']
    col2 = db['candidate_L2']
    col3 = db['candidate_L3']
    if (length==2):
        for doc2 in col2.find():
            itemset=doc2['item_set']
            support=doc2['support']
            #print(itemset)
            #pdb.set_trace()
            for doc1 in col1.find():
                item=doc1['item_set']
             #   print(item)
                if(item[0] == itemset[0]):
                    if(support/doc1['support']>=minconf):
                        print(item[0]," => ",itemset[1],'   ',support/doc1['support'])
                if(item[0] == itemset[1]):
                    if(support/doc1['support']>=minconf):
                        print(item[0]," => ",itemset[0],'   ',support/doc1['support'])
    if (length==3):
        for doc3 in col3.find():
            itemset=doc3['item_set']
            support=doc3['support']
            #print(itemset)
            for doc1 in col1.find():
                item=doc1['item_set']
                if(item[0] == itemset[0]):
                    if(support/doc1['support']>=minconf):
                        print(item[0]," => ", itemset[1],',' ,itemset[2], '     ', support/doc1['support'])
                if(item[0] == itemset[1]):
                    if(support/doc1['support']>=minconf):
                        print(item[0]," => ", itemset[0],',', itemset[2], '     ', support/doc1['support'])
                if(item[0] == itemset[2]):
                    if(support/doc1['support']>=minconf):
                        print(item[0]," => ", itemset[0],',',itemset[1], '     ', support/doc1['support'])
            for doc2 in col2.find():
                item=doc2['item_set']
                if(((item[0]==itemset[0]) and (item[1]==itemset[1]))or((item[0]==itemset[1]) and (item[1]==itemset[0]))):
                    if(support/doc2['support']>=minconf):
                        print(item[0],',',item[1]," => ",itemset[2],'     ', support/doc2['support'])
                if(((item[0]==itemset[1]) and (item[1]==itemset[2]))or((item[0]==itemset[2]) and (item[1]==itemset[1]))):
                    if(support/doc2['support']>=minconf):
                        print(item[0],',',item[1]," => ",itemset[0],'     ', support/doc2['support'])
                if(((item[0]==itemset[0]) and (item[1]==itemset[2]))or((item[0]==itemset[2]) and (item[1]==itemset[0]))):
                    if(support/doc2['support']>=minconf):
                        print(item[0],',',item[1]," => ",itemset[1],'     ', support/doc2['support'])


def printMenu():
    print("0. CopyData")
    print("1. Morph")
    print("2. print morphs")
    print("3. print wordset")
    print("4. frequent item set")
    print("5. association rule")

if __name__=="__main__":
    make_stop_word()
    printMenu()
    selector=int(input())
    if selector==0:
        p0()
    elif selector==1:
        p1()
        p3()
    elif selector==2:
        p2()
    elif selector==3:
        p4()
    elif selector==4:
        print("input length of the frequent item:")
        length=int(input())
        p5(length)
    elif selector==5:
        print("input length of the frequent item:")
        length=int(input())
        p6(length)


