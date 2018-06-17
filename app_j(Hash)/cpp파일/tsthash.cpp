// tsthash.cpp
/* 원본소스
#include "direct.h"
#include "bucket.h"
#include "hash.h"
#include <iostream.h>

main ()
{
	int result;
	Directory Dir (4);
	result = Dir . Create ("tsthash");
	if (result == 0)
	{
		cout<<"Please delete tsthash.dir and tsthash.bkt"<<endl;
		return 0;
	}
	char * keys[]={"bill", "lee", "pauline", "alan", "julie",
		"mike", "elizabeth", "mark", "ashley", "peter",
		"joan", "john", "charles", "mary", "emily"};
	const int numkeys = 15;
	for (int i = 0; i<numkeys; i++)
	{
		cout <<keys[i]<<" "<<(void*)Hash(keys[i])
			<<" "<<(void*)MakeAddress(keys[i],16)<<endl;
		result = Dir . Insert (keys[i], 100 + i);
		if (result == 0) cout << "insert for "<<keys[i]<<" failed"<<endl;
		Dir . Print (cout);
	}
	return 1;
}
*/

// tsthash.cpp

#include "direct.h"
#include "bucket.h"
#include "hash.h"
#include "delim.h"
#include "student.h"
#include <iostream>

#define num 5

using namespace std;


int main ()
{
	int result = 0, recaddr = 0;
	DelimFieldBuffer Buffer;
	RecordFile<Student> Recfile(Buffer);
	Recfile.Create("record.dat", ios::in | ios::out | ios::trunc);

	Directory Dir (4);
	result = Dir . Create ("tsthash");

	if (result == 0)
	{
		cout<<"Please delete tsthash.dir and tsthash.bkt"<<endl;
		return 0;
	}
	
	while (1) {

		int record_choice1;

		cout << endl << "1. Student	 2. Exit  \n Enter the Number : "; cin >> record_choice1;
		if (record_choice1 == 1) {
			Student* S[num];
			for (int i = 0; i < num; i++)
				S[i] = new Student();

			cout << "Student의 입력" << endl;
			cin.ignore();
			for (int i = 0; i < num; i++)
			{
				cout << i + 1 << "번째 Student" << endl;
				cin >> *S[i];
				fflush(stdin);
			}

			cout << "########################################################" << endl;
			cout << "\tInserting Record to Directory Start" << endl;
			cout << "########################################################" << endl;

			for (int i = 0; i < num; i++)
			{
				//======================================================
				recaddr = Recfile.Write(*S[i]);
				cout << "Recording S[" << i + 1 << "] at recaddr " << recaddr << endl;
				cout << "key = " << S[i]->Key() << endl;
				//=======================================================
				cout << S[i]->Key() << " " << (void*)Hash(S[i]->Key())
					<< " " << (void*)MakeAddress(S[i]->Key(), 16) << endl;
				result = Dir.Insert(S[i]->Key(), recaddr);
				if (result == 0) cout << "insert for " << S[i]->Key() << " failed" << endl;
				//=======================================================

				cout << "########################################################" << endl;
			}

			Dir.Print(cout);

			cout << "\tInserting Record to Directory End" << endl;
			cout << "########################################################" << endl << endl << endl;



			cout << "========================================================" << endl;
			cout << "\tDeleting Record from Directory Start" << endl;
			cout << "========================================================" << endl;

			cout << "Press Any Key to Start Deleting\n";
			getchar();

			for (int i = 0; i < num; i++) {
				cout << "Recording S[" << i + 1 << "] " << "key = " << S[i]->Key() << endl;
				result = Dir.Remove(S[i]->Key());
				if (result == 0) cout << "deleting for " << S[i]->Key() << " failed" << endl;
				Dir.Print(cout);
				cout << "========================================================" << endl;
			}

			cout << "\tDeleting Record from Directory End" << endl;
			cout << "========================================================" << endl;
		}
		else if (record_choice1 == 2) {
			return -1;
		}
	}
	

	return 0;
}