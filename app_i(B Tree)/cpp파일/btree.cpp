//btree.tc
#include "btnode.tc"
#include "indbuff.tc"
#include "btree.h"
#include <iostream.h>

const int MaxHeight = 5;
template <class keyType>
BTree<keyType>::BTree(int order, int keySize, int unique)
: Buffer (1+2*order,sizeof(int)+order*keySize+order*sizeof(int)),
	BTreeFile (Buffer), Root (order)
{
	Height = 1;
	Order = order;
	PoolSize = MaxHeight*2;
	Nodes = new BTNode * [PoolSize];
	BTNode::InitBuffer(Buffer,order);
	Nodes[0] = &Root;
}

template <class keyType>
BTree<keyType>::~BTree()
{
	Close();
	delete Nodes;
}

template <class keyType>
int BTree<keyType>::Open (char * name, int mode)
{
	int result;
	result = BTreeFile.Open(name, mode);
	if (!result) return result;
	// load root
	BTreeFile.Read(Root);
	Height = 1; // find height from BTreeFile!
	return 1;
}

template <class keyType>
int BTree<keyType>::Create (char * name, int mode)
{
	int result;
	result = BTreeFile.Create(name, mode);
	if (!result) return result;
	// append root node
	result = BTreeFile.Write(Root);
	Root.RecAddr=result;
	return result != -1;	
}

template <class keyType>
int BTree<keyType>::Close ()
{
	int result;
	result = BTreeFile.Rewind();
	if (!result) return result;
	result = BTreeFile.Write(Root);
	if (result==-1) return 0;
	return BTreeFile.Close();
}


template <class keyType>
int BTree<keyType>::Insert (const keyType key, const int recAddr)
{
	int result; int level = Height-1; 
	int newLargest=0; keyType prevKey, largestKey; 
	BTNode * thisNode, * newNode, * parentNode;
	thisNode = FindLeaf (key);
	
	//추가코드 시작
	newNode = NewNode();
	prevKey = NULL;
	//추가코드 마지막
	
	// test for special case of new largest key in tree
	/* 원본소스
	if (key > thisNode->LargestKey())
		{newLargest = 1; prevKey=thisNode->LargestKey();}
	
	result = thisNode -> Insert (key, recAddr);
	*/
	//=========================<추가코드 시작>=============================
	if (Height <= 1){
		if (recAddr == 0){
			result = thisNode->Insert(key, recAddr);
		}
	}
	if (strcmp(key, thisNode->LargestKey())>0) //키값을 string타입으로 사용하기 때문에
		//strcmp로 비교를 해야한다.
	{
		newLargest = 1; prevKey = thisNode->LargestKey();
	}
	//=========================<추가코드 마지막>=============================

	result = thisNode->Insert(key, recAddr);
	
	// handle special case of new largest key in tree
	if (newLargest)
		for (int i = 0; i<Height-1; i++) 
		{
			Nodes[i]->UpdateKey(prevKey,key);
			if (i>0) Store (Nodes[i]);
		}

	while (result==-1) // if overflow and not root
	{
		//remember the largest key
		largestKey=thisNode->LargestKey();
		// split the node
		newNode = NewNode();
		thisNode->Split(newNode);
		Store(thisNode); Store(newNode);
		level--; // go up to parent level
		if (level < 0) break;
		// insert newNode into parent of thisNode
		parentNode = Nodes[level];
		result = parentNode->UpdateKey
			(largestKey,thisNode->LargestKey());
		result = parentNode->Insert
			(newNode->LargestKey(),newNode->RecAddr);
		thisNode=parentNode;
	}
	Store(thisNode);
	if (level >= 0) return 1;// insert complete
	// else we just split the root
	int newAddr = BTreeFile.Append(Root); // put previous root into file
	// insert 2 keys in new root node
	Root.Keys[0]=thisNode->LargestKey();
	Root.RecAddrs[0]=newAddr;
	Root.Keys[1]=newNode->LargestKey();
	Root.RecAddrs[1]=newNode->RecAddr;
	Root.NumKeys=2; 
	Height++;
	return 1;	
}
/*원본소스
template <class keyType>
int BTree<keyType>::Remove (const keyType key, const int recAddr)
{
	// left for exercise
	return -1;
}
*/

template <class keyType>
int BTree<keyType>::Remove(const keyType key, const int recAddr)
{
	int result; int level = Height - 1; // 레벨은 리프를 나타낸다.
	int merge_result = -1; int isRoot = 0;
	int temp_result = 0; int tem_rec = -1; // - tem_rec 는 merge 시 부모 노드의 언더플로우가 발생하면 처리하는 부분에서 사용.
	int upper_level = Height - 2; int a_temp;
	int isnei = -1; int isrnei = -1;
	int isLargest = 0; keyType prevKey, largestKey, nextKey;
	BTNode * thisNode, *newNode, *neighNode, *neighRNode;
	prevKey = NULL;
	largestKey = NULL;
	nextKey = NULL;
	neighNode = NULL;
	neighRNode = NULL;

	thisNode = FindLeaf(key);

	if (strcmp(key, thisNode->LargestKey()) == 0) // - 삭제하려는 키가 노드에서 가장 큰 키의 경우.
	{
		isLargest = 1;
		prevKey = key;
	}
	result = thisNode->Remove(key, recAddr); // - 노드 제거
	largestKey = thisNode->LargestKey(); // - 제거후 가장 큰 키를 저장.
	if (isLargest == 1) // - 삭제한 키가 노드에서 가장 큰 키이면.
	{
		for (int i = 0; i<level; i++)
		{
			Nodes[i]->UpdateKey(prevKey, largestKey); // - 부모 노드들의 업데이트.
			if (i>0) Store(Nodes[i]); // - 이후 저장.
		}
	}
	while (result == -1)
	{
		if (upper_level < 0 && result == -1){ isRoot = 1; result = 0; break; }
		if (result == -1) // - 언더플로우 발생시.
		{

			prevKey = Nodes[level - 1]->Findprevkey(largestKey);
			if (strcmp(prevKey, largestKey) == 0) // - 노드 내에서 가장 작은 값을 merge 할 때.
			{
				nextKey = Nodes[level - 1]->Findnextkey(largestKey);
				neighRNode = FindLeaf(nextKey);
				a_temp = neighRNode->RMerge(thisNode);
				if (a_temp == 0) // - merge 한 노드가 오버플로우일때
				{
					newNode = NewNode();

					temp_result = neighNode->FSplit(newNode); 
					neighRNode->RMerge(thisNode);
					for (int i = 0; i < level; i++)
					{
						Nodes[i]->UpdateKey(largestKey, neighRNode->LargestKey());
						if (i>0) Store(Nodes[i]);
					}
					Store(neighRNode);
					Store(newNode);
					delete newNode;
					newNode = NULL;
					break; // - 부모 노드의 언더플로우가 발생하지 않을 상황.
				}
				if (a_temp == 1) // - merge 한 노드가 언더플로우일때.
				{
					result = Nodes[upper_level]->Remove(largestKey);
					Store(Nodes[upper_level]);
					for (int i = 0; i < level; i++)
					{
						Nodes[i]->UpdateKey(largestKey, neighRNode->LargestKey());
						if (i>0) Store(Nodes[i]);
					}
					Store(neighRNode);
				}
				if (result == 1){ result = 0; break; } // - 부모 노드의 언더 플로우가 발생하지 않을 상황
				if (result == -1){ upper_level--;    level--; isrnei = 1; isnei = -1; continue; }
			} // - 노드 내에서 가장 작은 값을 merge 하는 부분은 여기까지.
			neighNode = FindLeaf(prevKey); // - 그 외의 경우.
			merge_result = neighNode->Merge(thisNode);
			if (merge_result == 0) // - merge한 노드가 오버플로우 일때.
			{
				newNode = NewNode();

				temp_result = neighNode->FSplit(newNode);
				thisNode->RMerge(newNode);

				for (int i = 0; i<level; i++)
				{
					Nodes[i]->UpdateKey(prevKey, neighNode->LargestKey());
					if (i>0) Store(Nodes[i]);
				}
				Store(neighNode);
				Store(thisNode);
				delete newNode;
				newNode = NULL;
				break; // - 부모 노드의 언더 플로우가 발생하지 않는 상황.
			}
			if (merge_result == 1) //- merge 한 노드가 오버플로우가 아닐때.
			{
				result = Nodes[upper_level]->Remove(largestKey);
				Store(Nodes[upper_level]);
				for (int i = 0; i<level; i++)
				{
					Nodes[i]->UpdateKey(prevKey, neighNode->LargestKey());
					if (i>0) Store(Nodes[i]);
				}
				Store(neighNode);
			}
			if (result == 1) { result = 0; break; }
			upper_level--;
			level--;
			isnei = 1; isrnei = -1;
		}
	}
	if (result == 1)
	{
		Store(thisNode);
	}
	if (isRoot == 1)
	{
		if (Root.NumKeys != 1)
		{
			Store(thisNode);
			return 1;
		}
		if (isnei == 1)
		{
			Root.CopyNode(neighNode);
		}
		if (isrnei == 1)
		{
			Root.CopyNode(neighRNode);
		}
		Height--;
	}
	return 1;
}


template <class keyType>
int BTree<keyType>::Search (const keyType key, const int recAddr)
{
	BTNode * leafNode;
	leafNode = FindLeaf (key); 
	return leafNode -> Search (key, recAddr);
}

template <class keyType>
void BTree<keyType>::Print (ostream & stream) 
{
	stream << "BTree of height "<<Height<<" is "<<endl;
	Root.Print(stream);
	if (Height>1)
		for (int i = 0; i<Root.numKeys(); i++)
		{
			Print(stream, Root.RecAddrs[i], 2);
		}
	stream <<"end of BTree"<<endl;
}

template <class keyType>
void BTree<keyType>::Print 
	(ostream & stream, int nodeAddr, int level) 
{
	BTNode * thisNode = Fetch(nodeAddr);
	stream<<"Node at level "<<level<<" address "<<nodeAddr<<' ';
	thisNode -> Print(stream);
	if (Height>level)
	{
		level++;
		for (int i = 0; i<thisNode->numKeys(); i++)
		{
			Print(stream, thisNode->RecAddrs[i], level);
		}
		stream <<"end of level "<<level<<endl;
	}
}
//===========================<추가코드 시작>=================================
template <class keyType>
void BTree<keyType>::TPrint(ostream &stream) {
	stream << "BTree of height " << Height << " is " << std::endl << std::endl;
	int Tlevel = 2;
	Root.TPrint(stream);
	stream << std::endl;

	if (Height > 1) {
		for (int i = 0; i < Root.numKeys(); i++) {
			stream << std::endl;
			TPrint(stream, Root.RecAddrs[i], Tlevel);
		}
	}

	if (Height == 1) {
		return;
	}

	stream << endl;
}

template <class keyType>
void BTree<keyType>::TPrint(std::ostream &stream, int nodeAddr, int level) {
	int btem_level = level;
	BTNode* thisNode = Fetch(nodeAddr);

	for (int i = 0; i < level; ++i)
	{
		stream << '\t';
	}

	thisNode->TPrint(stream);
	stream << std::endl;

	if (Height > btem_level) {
		stream << std::endl;
		btem_level++;

		for (int i = 0; i < thisNode->numKeys(); i++) {
			TPrint(stream, thisNode->RecAddrs[i], btem_level);
		}
	}

	delete thisNode;
	thisNode = NULL;
}

//=============================<추가코드 마지막>=============================
template <class keyType>
BTreeNode<keyType> * BTree<keyType>::FindLeaf (const keyType key)
// load a branch into memory down to the leaf with key
{
	int recAddr, level;
	for (level = 1; level < Height; level++)
	{
		recAddr = Nodes[level-1]->Search(key,-1,0);//inexact search
		Nodes[level]=Fetch(recAddr);
	}
	return Nodes[level-1];
}

template <class keyType>
BTreeNode<keyType> * BTree<keyType>::NewNode ()
{// create a fresh node, insert into tree and set RecAddr member
	BTNode * newNode = new BTNode(Order);
	int recAddr = BTreeFile . Append(*newNode);
	newNode -> RecAddr = recAddr;
	return newNode;
}

template <class keyType>
BTreeNode<keyType> * BTree<keyType>::Fetch(const int recaddr)
{// load this node from File into a new BTreeNode
	int result;
	BTNode * newNode = new BTNode(Order);
	result = BTreeFile.Read (*newNode, recaddr);
	if (result == -1) return NULL;
	newNode -> RecAddr = result;
	return newNode;
}

template <class keyType>
int BTree<keyType>::Store (BTreeNode<keyType> * thisNode)
{
	return BTreeFile.Write(*thisNode, thisNode->RecAddr);
}
