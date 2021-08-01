// 0853337_曹議濃_HW4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

typedef struct Arc{
public:
	double tail;
	double head;
	double capacity;
};

typedef struct Node{
public:
	double index;
	double min_arc_index;
	double indegree;

	double distance;
	double precedence;
	double s;
};

typedef struct Topological{
public:
	Node node;
	double ordering;
};

typedef struct Path{
public:
	Node precedence;
	Node current;
};

// function declaration
void swap(int &pos_x, int &pos_y);
void printArray(vector<Arc> &A);
void maxHeapify(vector<Arc> &A, int root, int length);
void build(vector<Arc> &A);
void heapSort(vector<Arc> &A);

vector<Node> createNodeVector(vector<Arc> &A);
void bfs(vector<Node> &node_vector, vector<Arc> &A, int source_node, int end_node);
int dequeue_implementation(vector<Node> &node_vector, vector<Arc> &A, int source_node, int end_node);
void augementing_path_algorithm(vector<Node> &node_vector, vector<Arc> &A, int source_node, int end_node);


int _tmain(int argc, _TCHAR* argv[])
{
	// read forward star representation matrix
	vector<Arc> forward_star_matrix;
	string line, data;
	fstream file;
	file.open("4.csv");
	
	while(getline(file, line, '\n')){
		istringstream templine(line);
		int counter = 0;
		
		Arc arc;
		while(getline(templine, data, ',')){
			if (atof(data.c_str()) && counter == 0){
				arc.tail = atof(data.c_str());
				counter++;
			}else if (atof(data.c_str()) && counter == 1){
				arc.head = atof(data.c_str());
				counter++;
			}else if (atof(data.c_str()) && counter == 2){
				arc.capacity = atof(data.c_str());
				counter++;
			}
		}
		if (counter != 0){
			forward_star_matrix.push_back(arc);

			// residual network
			Arc residual;
			residual.tail = arc.head;
			residual.head = arc.tail;
			residual.capacity = 0;

			forward_star_matrix.push_back(residual);
		}
	}
	file.close();
	printf("FINISH READ RAW DATA\n\n");
	
	// show the forward star matrix
	printf("BEFORE SORTING THE MATRIX\n");
	printArray(forward_star_matrix);
	printf("\n");
	
	// sorting the forward star matrix
	// using heapsort to ordering my raw data, order by head
	printf("AFTER SORTING THE MATRIX\n");
	heapSort(forward_star_matrix);
	
	// show the forward star matrix
	printArray(forward_star_matrix);
	
	// create the point vector
	vector<Node> node_vector = createNodeVector(forward_star_matrix);

	// show the point vector
	printf("\nPOINT VECTOR\n");
	for (int i = 0; i < node_vector.size(); i++) printf("node%d = %f\n",i+1, node_vector[i].min_arc_index);

	// augementing path algorithm
	int source_node_index, end_node_index;
	printf("key the source node index is =");
	scanf("%d", &source_node_index);
	printf("key the end node index is =");
	scanf("%d", &end_node_index);
	augementing_path_algorithm(node_vector, forward_star_matrix, source_node_index-1, end_node_index-1);



	// arc based algorithm


	
	printf("\nFINISH !!!\n");

	system("pause");
	return 0;
}

void swap(int &pos_x, int &pos_y){
	int temp = pos_x;
	pos_x = pos_y;
	pos_y = pos_x;
}

void printArray(vector<Arc> &A){
	for (int i = 0; i < A.size(); i++) printf("(%f, %f)\n", A[i].tail, A[i].head);
}

void maxHeapify(vector<Arc> &A, int root, int length){
	int left = 2 * root;
	int right = 2 * root + 1;
	int largest;
	
	if (left <= length && A[left].tail > A[root].tail) largest = left;	
	else largest = root;
	
	if (right <= length && A[right].tail > A[largest].tail)	largest = right;

	/*printf("%s", "root\n");
	printf("%s,%s,%s\n", left, right, largest);
	printArray(A);*/
	
	if (largest != root){
		swap(A[largest], A[root]);
		
		/*printf("%s", "swap");
		printArray(A);*/
		maxHeapify(A, largest, length);
	}
}

void build(vector<Arc> &A){
	for (int i = (int)A.size()/2; i>=1; i--) maxHeapify(A, i, (int)A.size()-1);
}

void heapSort(vector<Arc> &A){
	Arc new_arc;
	new_arc.tail = 0;
	new_arc.head = 0;
	
	A.insert(A.begin(), new_arc);
	build(A);
	/*printf("%s", "FINISH BUILD");*/
	
	int size = (int)A.size()-1;
	for (int i = (int)A.size()-1; i >= 2; i--){
		swap(A[1], A[i]);
		size--;
		maxHeapify(A, 1, size);
	}
	
	A.erase(A.begin());
}

vector<Node> createNodeVector(vector<Arc> &A){
	// total nodes in the graph
	vector<int> node_index; 
	for (int i = 0; i < A.size(); i++){
		if (find(node_index.begin(), node_index.end(), A[i].head) == node_index.end()){
			node_index.push_back(A[i].head);
		}
		
		if (find(node_index.begin(), node_index.end(), A[i].tail) == node_index.end()){
			node_index.push_back(A[i].tail);
		}
	}
	sort(node_index.begin(), node_index.end());
	
	// create node_vector by head column
	vector<Node> node_vector;
	Node node;
	int num_node = 1;
	int num_arcs = 0;

	//
	node.index = 1;
	node.min_arc_index = 1;
	node_vector.push_back(node);

	while (num_node != node_index.size()+1){
		num_arcs = 0;
		for (int i = 0; i < A.size(); i++){
			if (A[i].tail == node_index[num_node-1]){
				num_arcs++;
			}
		}
		node.index = num_node + 1;
		node.min_arc_index = node_vector[num_node-1].min_arc_index + num_arcs;
		node_vector.push_back(node);
		num_node++;
	}

	return node_vector;
}

void bfs(vector<Node> &node_vector, vector<Arc> &A, int source_node, int end_node){
vector<int> temp_list;
	vector<int> history;
	vector<Path> result_path;
	Path path;
	
	// source node
	temp_list.push_back(A[source_node].tail);
	
	// other node in the path
	while(temp_list.size() != 0){
		for (int i = node_vector[temp_list[0]-1].min_arc_index; i < node_vector[temp_list[0]].min_arc_index; i++){
			if (find(temp_list.begin(), temp_list.end(), A[i-1].head) == temp_list.end() && find(history.begin(), history.end(), A[i-1].head) == history.end()){
				temp_list.push_back(A[i-1].head);
				path.current.index = A[i-1].head;
				path.precedence.index = temp_list[0];
				result_path.push_back(path);
			}
		}
		history.push_back(temp_list[0]);
		temp_list.erase(temp_list.begin());
	}

	// show the path found by bfs
	printf("\nbfs tree arcs:\n");
	for (int i = 0; i < result_path.size(); i++){
		printf("node %f, pre %f\n", result_path[i].current.index, result_path[i].precedence.index);		
	}
}

int dequeue_implementation(vector<Node> &node_vector, vector<Arc> &A, int source_node, int end_node){
	// initialization; flag := -2, 排過隊的:= -1, otherwise := node index
	vector<Node> queue;
	for (int i = 0; i < node_vector.size(); i++){
		node_vector[i].distance = INT_MAX;
		node_vector[i].precedence = 0;
		node_vector[i].s = 0;
	}

	// source node
	double flow = INT_MAX;
	node_vector[source_node].distance = 0;
	node_vector[source_node].precedence = 0;
	node_vector[source_node].s = -2;
	queue.push_back(node_vector[source_node]);

	// table name
	/*printf("\t");
	for (int i = 0; i < node_vector.size() - 1; i++) printf("node %d\t", (int)node_vector[i].index);
	printf("\nQueue = ");
	for (int i = 0; i < queue.size(); i++) printf("%d, ", (int)queue[i].index);
	printf("\nd(i) = \t");
	for (int i = 0; i < node_vector.size() - 1; i++) printf("%d\t", (int)node_vector[i].distance);
	printf("\np(i) = \t");
	for (int i = 0; i < node_vector.size() - 1; i++) printf("%d\t", (int)node_vector[i].precedence);
	printf("\ns(i) = \t");
	for (int i = 0; i < node_vector.size() - 1; i++) printf("%d\t", (int)node_vector[i].s);
	printf("\n=============================================================================================\n");*/

	// procedure
	vector<int> record1, record2;
	int iterations = 0;
	while(queue.size()!=0){
		// update distance label from queue[0].index
		node_vector[queue[0].index-1].s = -1;
		for (int i = node_vector[queue[0].index-1].min_arc_index-1; i < node_vector[queue[0].index].min_arc_index-1;i++){
			if (node_vector[A[i].head-1].distance > 1 + node_vector[queue[0].index-1].distance && A[i].capacity > 0){
				node_vector[A[i].head-1].distance = 1 + node_vector[queue[0].index-1].distance;
				node_vector[A[i].head-1].precedence = A[i].tail;

				// add into queue (插隊或是正常排隊)
				if (node_vector[A[i].head-1].s == -1){
					queue.insert(queue.begin()+1, 1, node_vector[A[i].head-1]);
				}
				// add head node into queue
				else if (i != node_vector[queue[0].index].min_arc_index && node_vector[A[i].head-1].s <= 0 && node_vector[A[i].head-1].s > -2){
					queue.push_back(node_vector[A[i].head-1]);
				}
				else if (node_vector[A[i].head-1].s <= 0 && node_vector[A[i].head-1].s > -2){
					node_vector[A[i].head-1].s = node_vector[A[i].head-1].index;
					queue.push_back(node_vector[A[i].head-1]);
				}
			}
		}

		// update s(i)
		Node marked = queue[0];
		node_vector[queue[0].index-1].s = -1;
		queue.erase(queue.begin());

		if (queue.size() != 0){
			node_vector[queue[queue.size()-1].index-1].s = -2;
			for (int i = 0; i < queue.size()-1; i++) node_vector[queue[i].index-1].s = queue[i+1].index;
		} 
		 
		// show output table
		/*printf("\nQueue = ");
		for (int i = 0; i < queue.size(); i++) printf("%d, ", (int)queue[i].index);
		printf("\n Marked node = %d", (int) marked.index);
		printf("\nd(i) = \t");
		for (int i = 0; i < node_vector.size() - 1; i++) printf("%d\t", (int)node_vector[i].distance);
		printf("\np(i) = \t");
		for (int i = 0; i < node_vector.size() - 1; i++) printf("%d\t", (int)node_vector[i].precedence);
		printf("\ns(i) = \t");
		for (int i = 0; i < node_vector.size() - 1; i++) printf("%d\t", (int)node_vector[i].s);
		printf("\n=============================================================================================\n");*/
		iterations++;
	}

	/*printf("total iterations = %d\n", iterations);
	printf("shortest path cost = %d\n", (int)node_vector[end_node].distance);*/
	
	// link the path
	vector<int> res_path;
	res_path.push_back(node_vector[end_node].index);

	while (node_vector[end_node].precedence != 0)
	{
		int pre = node_vector[end_node].precedence-1;
		end_node = node_vector[pre].index;
		res_path.push_back(end_node);
		end_node--;
	}
	
	// find all of used arcs to store in record
	for (int i = 0; i < res_path.size()-1; i++){
		for (int j = node_vector[res_path[i]-1].min_arc_index-1; j < node_vector[res_path[i]].min_arc_index-1; j++)
			if (A[j].head == res_path[i+1]){
				record1.push_back(j);
			}
	}

	reverse(res_path.begin(),res_path.end());

	for (int i = 0; i < res_path.size()-1; i++){
		for (int j = node_vector[res_path[i]-1].min_arc_index-1; j < node_vector[res_path[i]].min_arc_index-1; j++)
			if (A[j].head == res_path[i+1]){
				record2.push_back(j);
			}
	}

	// take minimum capacity from "record"
	for (int i = 0; i < record2.size(); i++){
		if (A[record2[i]].capacity < flow) flow = A[record2[i]].capacity;
	}

	

	for (int i = 0; i < record1.size(); i++){
		// residual network
		for (int j = node_vector[A[i].head-1].min_arc_index-1; j < node_vector[A[i].head].min_arc_index-1; j++){
			if (A[j].head == A[i].tail) A[record2[i]].capacity -= flow;
		}
	}


	// update arc capacity and build residual network
	for (int i = 0; i < record1.size(); i++){
		// residual network
		for (int j = node_vector[A[i].head-1].min_arc_index-1; j < node_vector[A[i].head].min_arc_index-1; j++){
			if (A[j].head == A[i].tail) A[j].capacity += flow;
		}
	}
	
	if (flow != INT_MAX){
		printf("-----------------------------------------------------------------------");
		printf("\npath = ");
		for (int i = 0; i < res_path.size(); i++){ 
			if (i != res_path.size()-1) printf("node %d --> ", res_path[i]);
			else printf("node %d", res_path[i]);
		}
		printf("\t flow = %f \n", flow);
	}
	
	return flow;
}

void augementing_path_algorithm(vector<Node> &node_vector, vector<Arc> &A, int source_node, int end_node){
	Path path;
	double max_flow = 0, flow;

	// find an available path from source to end
	flow = dequeue_implementation(node_vector, A, source_node, end_node);
	while(flow != INT_MAX){
		max_flow += flow;
		flow = dequeue_implementation(node_vector, A, source_node, end_node);
	}

	printf("-----------------------------------------------------------------------");
	printf("\nMAXIMUM FLOW = %f", max_flow);
}
