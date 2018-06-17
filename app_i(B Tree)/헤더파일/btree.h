// btree.h

#ifndef BTREE_H
#define BTREE_H

template <class keyType>
class BTree;

#include "btnode.h"
#include "recfile.h"
#include "fixfld.h"
#include "indbuff.h"



template <class keyType>
class BTree
{public:
	BTree(int order, int keySize = sizeof(keyType), int unique = 1);
	~BTree();
	int Open (char * name, int mode);
	int Create (char * name, int mode);
	int Close ();
	int Insert (const keyType key, const int recAddr);
	int Remove (const keyType key, const int recAddr = -1);
	int Search (const keyType key, const int recAddr = -1);
	void Print (ostream &);
	void Print (ostream &, int nodeAddr, int level);
	//=========추가 구현 시작======================
	void TPrint(ostream &);								//트리를 print하기 위해 만든 함수
	void TPrint(ostream &, int nodeAddr, int level);
	//=========추가 구현 끝======================

protected:
	typedef BTreeNode<keyType> BTNode;// useful shorthand
	BTNode * FindLeaf (const keyType key);
	// load a branch into memory down to the leaf with key
	BTNode * NewNode ();
	BTNode * Fetch(const int recaddr);
	int Store (BTNode *);
	BTNode Root;
	
	int Height; // height of tree
	int Order; // order of tree
	int PoolSize;
	BTNode ** Nodes; // pool of available nodes
	// Nodes[1] is level 1, etc. (see FindLeaf)
	// Nodes[Height-1] is leaf
	FixedFieldBuffer Buffer;
	RecordFile<BTNode> BTreeFile;	
};

#endif
