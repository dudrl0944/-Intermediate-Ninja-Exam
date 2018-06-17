/*원본소스
//tstbtree.cc
#include "btnode.h"
#include "btnode.tc"
#include "btree.h"
#include "btree.tc"
#include <iostream.h>

const char * keys="CSDTAMPIBWNGURKEHOLJYQZFXV";

const int BTreeSize = 4;
main (int argc, char * argv)
{
	int result, i;
	BTree <char> bt (BTreeSize);
	result = bt.Create ("testbt.dat",ios::in|ios::out);
	if (!result){cout<<"Please delete testbt.dat"<<endl;return 0;}
	for (i = 0; i<26; i++)
	{
		cout<<"Inserting "<<keys[i]<<endl;
		result = bt.Insert(keys[i],i);
		bt.Print(cout);
	}
	bt.Search(1,1);
	return 1;
}

*/

//tstbtree.cc
#include "btnode.h"
#include "btnode.cpp"
#include "btree.h"
#include "btree.cpp"
#include "person.h"
#include "recfile.h"
#include "delim.h"
#include "simpind.h"
#include "strclass.h"
#include <iostream>

const int BTreeSize = 5;
using namespace std;

int main (int argc, char * argv)
{
	int result = 0, recaddr = 0;
	char tkey[10];
	DelimFieldBuffer Buffer;
	RecordFile<Person> Recfile (Buffer);
	Recfile . Create ("record.dat",ios::in|ios::out|ios::trunc);

	BTree <char*> bt (BTreeSize);
	result = bt.Create ("testbt.dat",ios::in|ios::out);
	if (!result){cout<<"Please delete testbt.dat"<<endl;return 0;}

	Person* P[35];
	for(int i = 0; i < 35; i++)
		P[i] = new Person();

	cout << "Person의 입력" << endl;
	for (int i = 0; i < 35; i++)
	{
		cout << i+1 << "번째 Person" << endl;
		cin >> *P[i];
		fflush(stdin);
	}
	
	for(int i=0;i<35;++i)
	{
		recaddr = Recfile.Write(*P[i]);
		result = bt.Insert(P[i]->Key(),i);
		bt.TPrint(cout);
	}
	for (int i = 0; i < 25; i++){
		cout << "삭제할 Key값을 입력하시오." << endl;
		cin >> tkey;
		fflush(stdin);
		String key(tkey);
		result = bt.Remove(key);
		bt.TPrint(cout);
	}
	Recfile.Close();
	bt.Close();

	return 1;
}

