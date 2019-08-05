
#include<iostream>
#include<fstream>
using namespace std;

class Node {

public:

	int jobId;
	int jobTime;

	int kc;

	Node* next;

public:

	Node() {};
	Node(int id, int time) {

		jobId = id;

		jobTime = time;
	}

	void printNode()
	{
		cout << "( " << jobId << "," << jobTime << " )";
	}
};

class LinkedList {

public:

	Node* head;

	int count = 0;

	int* kidPoint;
public:

	LinkedList()
	{
		head = new Node(999999,0);
	}

	void insertOneNode(Node* lhead, Node* tempNode)
	{

		Node* spot = findSpot(lhead, tempNode);

		tempNode->next = spot->next;

		spot->next = tempNode;

		count++;
	}

	Node* findSpot(Node* lhead, Node* nnode)
	{
		Node* tempSpot = lhead;

		while (tempSpot->next != NULL&&tempSpot->next->kc>nnode->kc)
		{
			tempSpot = tempSpot->next;
		}

		return tempSpot;
	}

	Node* removeHead()
	{
		Node* tempNode = head;

		if (isEmpty())
		{

			cout << "OPEN is empty";

			exit(0);
		}

		if (count == 1)
		{
			tempNode = head->next;

			head->next = NULL;

		}

		if (count > 1)
		{

			tempNode = head->next;

			head->next = head->next->next;
		}

		count--;

		return tempNode;
	}

	void printList(ofstream& outfile2)
	{
		Node* spot = head;

		cout << "OPEN: ";
		outfile2 << "OPEN: ";
		

		if (head->next == NULL)
		{

			cout << "( " << head->jobId << "," << head->jobTime << " )";
			outfile2 << "( " << head->jobId << "," << head->jobTime << " )";

		}
		else
		{
			
			while (spot->next != NULL)
			{
				cout << "( " << spot->jobId << "," << spot->jobTime << " )";
				outfile2 << "( " << spot->jobId << "," << spot->jobTime << " )";

				spot = spot->next;
			}

			cout << "( " << spot->jobId << "," << spot->jobTime << " )";
			outfile2 << "( " << spot->jobId << "," << spot->jobTime << " )";
		}
	}

	bool isEmpty()
	{
		if (count == 0)return true;
		else
			return false;
	}
	
};

class Scheduling {

public:
	
	int numNodes;
	int totalJobTime;
	int progiven;
	int **adjacentMatirx, **scheduleTable;
	int *jobTimeArr, *processJob, *processTime, *parentCount, *kidCount, *jobDone, *jobMarked;

	LinkedList* open;

public:

	Scheduling() {

		open = new LinkedList();
	}

	void loadMatrix(ifstream& infile)
	{
		
		adjacentMatirx = new int*[numNodes + 1];
		for (int i = 0; i < numNodes + 1; i++)
		{
			adjacentMatirx[i] = new int[numNodes + 1];
		}

		for (int i = 0; i < numNodes+1; i++)
		{
			for (int j = 0; j < numNodes+1; j++)
			{
				adjacentMatirx[i][j] = 0;
			}
		}

		int holdx, holdy, holdsum;
		while (infile >> holdx >> holdy)
		{
			adjacentMatirx[holdx][holdy] = 1;
		}

		
		for (int i = 0; i < numNodes + 1; i++)
		{
			for (int j = 0; j < numNodes + 1; j++)
			{
				parentCount[i] = parentCount[i] + adjacentMatirx[j][i];
				kidCount[i] += adjacentMatirx[i][j];
			}
		}
		
	}
	
	int computeTotalJobTimes(ifstream& infile)
	{
		int holdx, holdy, sumy = 0;
		infile >> holdx;

		jobTimeArr = new int[numNodes + 1];

		jobTimeArr[0] = 0;
		while (infile >> holdx >> holdy)
		{
			sumy += holdy;
			jobTimeArr[holdx] = holdy;
		}
		return sumy;


	}

	int getUnMarkedOrphen(int i)
	{

		if (jobMarked[i] == 0 && parentCount[i] == 0)
			
			return i;

		else
			return -1;

	}

	void insert2Open(Node* lhead, Node* node)
	{
		open->insertOneNode(lhead, node);
	}

	void printList(LinkedList* list, ofstream& outfile2)
	{
		list->printList(outfile2);
	}

	void printTable(ofstream& outfile2)
	{

		for (int i = 1; i <= progiven; i++)
		{
			for (int j = 0; j <= totalJobTime; j++)
			{
				cout << scheduleTable[i][j];
				outfile2 << scheduleTable[i][j];
			}
			cout << endl;
			outfile2 << endl;
		}

	}

	void printFinalTable(ofstream& outfile1, int ct)
	{
		cout << "Current time is: " << ct << endl;
		outfile1<< "Current time is: " << ct << endl;

		cout << "    ";
		outfile1 << "    ";

		for (int i = 0; i <= totalJobTime; i++)
		{
			cout << "-" << i;
			outfile1 << "-" << i;
		}
		
		cout << endl;
		outfile1 << endl;

		for (int i = 1; i <= progiven; i++)
		{
			cout << "P(" << i << ")";
			outfile1<< "P(" << i << ")";
			for (int j = 0; j <= totalJobTime; j++)
			{
				if (scheduleTable[i][j] == 0)
				{
					cout << "|-";
					outfile1 << "|-";
				}
				else
				{
					cout << "|" << scheduleTable[i][j];
					outfile1 << "|" << scheduleTable[i][j];

				}
			}
			cout << endl << "-------------------------------------------" << endl;
			
			outfile1 << endl<< "-------------------------------------------" << endl;
		}

	}

	int findProcessor(int i)
	{
		if (processTime[i] <= 0 && !open->isEmpty())

			return i;

		else
			return -1;
	}

	void updateTable(int avaproc, Node *nj, int ct) 
	{

		for (int i = ct; i < ct + nj->jobTime; i++)
		{

			scheduleTable[avaproc][i] = nj->jobId;

		}

	}

	int checkCycle()
	{
		if (open->isEmpty() && checkGraph() && checkProcess())

			return 1;

		else

			return 0;
	}

	int findDoneJob(int i)
	{

		if (processTime[i] < 0)
		{
			processJob[i] = 0;

			return 0;
		}

		if (processTime[i] == 0)
		{
			return processJob[i];
		}
		if (processTime[i] > 0)
		{
			return 0;
		}
	}
	void deleteNode(int jobId)
	{
		jobDone[jobId] = 1;
	}
	void deleteEdge(int jobId)
	{
		for (int kidIndex = 1; kidIndex <= numNodes; kidIndex++)
		{
			if (adjacentMatirx[jobId][kidIndex] > 0)
			{
				parentCount[kidIndex]--;
			}
		}
	}

	void printDebug(ofstream& outfile, int ct, int *jm, int *pt, int *pj, int *jd )
	{
		cout << "Current Time: ";
		cout << ct << endl;

		outfile << "Current Time: ";
		outfile << ct << endl;


		cout << "-------" << endl;
		outfile << "-------" << endl;

		cout << "Job Marked: ";
		outfile << "Job Marked: ";
		for (int i = 1; i <= numNodes; i++)
		{
			cout << jm[i];
			outfile << jm[i];
		}

		cout << endl;
		cout << "-------" << endl;

		outfile << endl;
		outfile<< "-------" << endl;

		cout << "Process Time: ";
		outfile<< "Process Time: ";
		for (int i = 1; i <= progiven; i++)
		{
			cout << pt[i];
			outfile << pt[i];
		}

		cout << endl;
		cout << "------" << endl;

		outfile << endl;
		outfile<< "------" << endl;
		
		cout << "Process Job: ";
		outfile << "Process Job: ";
		for (int i = 1; i <= progiven; i++)
		{
			cout << pj[i];
			outfile << pj[i];
		}
		cout << endl;
		cout << "------" << endl;
		
		outfile << endl;
		outfile<< "------" << endl;

		cout << "Job Done: ";
		outfile<< "Job Done: ";
		for (int i = 1; i <= numNodes; i++)
		{
			cout << jd[i];
			outfile << jd[i];
		}
		cout << endl;
		cout << "------" << endl;

		outfile << endl;
		outfile << "------" << endl;

	}

	bool checkGraph()
	{
		for (int i = 1; i <= numNodes; i++)
		{
			if (jobDone[i] ==0 )return true;
		}

		return false;
	}

	bool checkProcess()
	{
		int count = 0;
		for (int i = 1; i <= progiven; i++)
		{
			if (processTime[i] <= 0)count++;
		}

		if (count == progiven)return true;

		else
			return false;
	}
};

int main(int agrc, char* argv[])
{
	ifstream infile1, infile2;
	ofstream outfile1, outfile2;
	infile1.open(argv[1]);
	infile2.open(argv[2]);

	outfile1.open(argv[3]);
	outfile2.open(argv[4]);

	Scheduling *myScheduling = new Scheduling();

	infile1 >> myScheduling->numNodes;

	myScheduling->parentCount = new int[myScheduling->numNodes + 1];
	myScheduling->kidCount = new int[myScheduling->numNodes + 1];
	myScheduling->jobMarked = new int[myScheduling->numNodes + 1];

	for (int i = 0; i < myScheduling->numNodes + 1; i++) {

		myScheduling->parentCount[i] = 0;
		myScheduling->kidCount[i] = 0;
		myScheduling->jobMarked[i] = 0;

	}

	myScheduling->loadMatrix(infile1);

	
	int totalJobTimes = myScheduling->computeTotalJobTimes(infile2);

	myScheduling->totalJobTime = totalJobTimes;


	int procGiven, procUsed = 0, currentTime = 0;

	cout << "Enter a number of processors: " << endl;
	cin >> procGiven;

	if (procGiven <= 0)cout << "it is an error";
	
	if (procGiven > myScheduling->numNodes)
	{
		procGiven = myScheduling->numNodes;
	}

	myScheduling->progiven = procGiven;

	//initialize arrays;
	myScheduling->scheduleTable = new int*[myScheduling->numNodes + 1];

	myScheduling->jobDone = new int[myScheduling->numNodes + 1];

	for (int i = 0; i < myScheduling->numNodes + 1; i++)
	{
		myScheduling->scheduleTable[i] = new int[myScheduling->totalJobTime + 1];

		myScheduling->jobDone[i] = 0;
		
	}

	for (int i = 0; i < myScheduling->numNodes + 1; i++)
	{
		for (int j = 0; j < myScheduling->totalJobTime + 1; j++)
		{
			myScheduling->scheduleTable[i][j] = 0;
		}
	}

	myScheduling->processTime = new int[procGiven + 1];
	myScheduling->processJob = new int[procGiven + 1];

	for (int i = 0; i <= procGiven; i++)
	{
		myScheduling->processTime[i] = 0;
		myScheduling->processJob[i] = 0;
	}

	//step1;

	int orphenNode;
	int availProc;

	Node* newJob;

	int index = 1;


	for (int i = 1; myScheduling->checkGraph(); i++) {

		
			for (int i = 1; i <= myScheduling->numNodes; i++)
			{

				orphenNode = myScheduling->getUnMarkedOrphen(i);

				if (orphenNode != -1) {

					myScheduling->jobMarked[orphenNode] = 1;

					Node* myNode = new Node(orphenNode, myScheduling->jobTimeArr[orphenNode]);

					myNode->kc = myScheduling->kidCount[orphenNode];

					myScheduling->insert2Open(myScheduling->open->head, myNode);


				}
			}

			myScheduling->printList(myScheduling->open, outfile2);


			//step4

			while (!myScheduling->open->isEmpty() && procUsed < procGiven) {
				
				for (int i = 1; i <= procGiven; i++) {

					availProc = myScheduling->findProcessor(i);

					if (availProc > 0)
					{

						procUsed++;


						newJob = new Node();

						newJob = myScheduling->open->removeHead();


						myScheduling->processJob[availProc] = newJob->jobId;

						myScheduling->processTime[availProc] = newJob->jobTime;

						
						myScheduling->updateTable(availProc, newJob, currentTime);
						

					}
				}
				
			}

			cout << endl;
			outfile2 << endl;

			if (myScheduling->checkCycle() >= 1)
			{
				cout << "Error: it is a cycle!";
				outfile1<< "Error: it is a cycle!";
				outfile2<< "Error: it is a cycle!";
				

				return 0;

			}

			myScheduling->printTable(outfile2);

			currentTime++;


			for (int i = 1; i <= procGiven; i++)
			{
				myScheduling->processTime[i] -= 1;

				if (myScheduling->processTime[i] <= 0)
				{
					procUsed--;
				}
			}

			//step10;
			
			for (int i = 1; i <= procGiven; i++)
			{

				int job = myScheduling->findDoneJob(i);

				if (job == 0)continue;

				myScheduling->deleteNode(job);
				myScheduling->deleteEdge(job);
				
			}

			myScheduling->printDebug(outfile2, currentTime, myScheduling->jobMarked, myScheduling->processTime, myScheduling->processJob, myScheduling->jobDone);

			index++;

	myScheduling->printFinalTable(outfile1, currentTime);
	}

	//cout << "fnial table:" << endl;
	//outfile1<< "fnial table:" << endl;
	//myScheduling->printFinalTable(outfile1);

		
	infile1.close();
	infile2.close();
	outfile1.close();
	outfile2.close();
    return 0;
}

