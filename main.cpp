
//  Land pruchase using Red black tree
//  Created on 5/8/18.
//

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
using namespace std;

enum Color { RED, BLACK };  //red black tree color

							//flags
int isfound;
bool addIn;
int counter;
pair <int, int> land[1001][1001];   //keep track of land owner information per each coordinate <land owner id, land price>
									//initializations
int id;
string name;
string phone;
int xCord;
int yCord;
int membership;
int balance;
int depth;
int charging;
int charge;


// Red black tree node
struct Node {
	Node *left, *right, *parent;    //left node, right node or parent of the node
	bool color;     //color of the red black tree node
					//member informations
	int id;       //id of member
	string name;
	string phone;
	int x;
	int y;
	int membership; //general, silver, golden or diamond
	int balance;    //balance amount of the member
	vector <pair<int, int>> change;   //keep track of change in the balance <increase(1) or decrease(0), amount>
									  // Constructor
	Node(int id) {
		//initialize member informations
		this->id = id;
		this->name = " ";
		this->phone = " ";
		this->balance = 0;
		this->membership = 0;
		this->color = RED;
		left = right = parent = NULL;
	}
};

//Red black tree
class RBT {
private:
	Node *root;     //root of the tree
public:
	vector <Node*> big;                   //keep track of 5 members with the biggest amount of balance
	RBT() { root = NULL; }                // Constructor
	Node* addMember();                    //add a member to RBT
	Node* adder(Node* root, Node *pt);    //add to RBT handler
	void leftRotation(Node *&, Node *&);  //rotate to the left for balancing (restructure) the RBT
	void rightRotation(Node *&, Node *&); //rotate to the right for balancing (restructure) the RBT
	void rebalance(Node *&, Node *&);     //restore the balance (double red violation) of the Red black tree
	void swap(Node *&, Node *&);          //swap the 2 nodes
	Node* searchNode(int id);             //search a node form the RBT
	Node* searcher(Node* &root, int id);  //search from the RBT handler
	void findDepth(Node* &pt);            //find depth of a node in the RBT
	void landPurchase(int id, int x, int y, int pay);   //handle land purchase transaction
	void Ranker(Node *pt);                //keep track of top 5 biggest balance amount for each member inserted
	void checkRank(Node *root);           //keep track of top 5 biggest balance amount from the whole RBT up to date after modification
	void updateMembership(Node* &root);   //update the membership based on member's balance
	Node* getRoot();                      //obtain root node of the RBT
};

//search if there is a member with same id node
Node* RBT::searchNode(int id) {
	depth = 0;
	return searcher(root, id);  //if found, return 1
}

Node* RBT::searcher(Node* &root, int id) {
	if (root == NULL) {  //if not found member with same id
		return root;
	}
	if (root->id == id) {   //if found member with same id
		if (charging == 1) {                                  //charge money
			root->balance = root->balance + charge;         //add to the balance
			root->change.push_back({ 1, charge });            //archive the positive changes of balance
			updateMembership(root);                         //update membership
			Ranker(root);                                   //update rank
		}
		return  root;                                       //return the node
	}
	else if (root->id < id) {                     //if id is bigger
		depth++;                                //keep track the depth
		return searcher(root->right, id);   //recursively go through right branch
	}
	else {                                       //if id is smaller
		depth++;                                //keep track the depth
		return searcher(root->left, id);    //recursively go through left branch
	}
}

void RBT::swap(Node*& xNode, Node*& yNode) {                                            //recolor?
	bool color = yNode->color;
	yNode->color = xNode->color;
	xNode->color = color;
}

//obtain root of RBT
Node* RBT::getRoot() {
	return root;
}

//update the membership based on member's balance
void RBT::updateMembership(Node* &root) {
	if (root->balance < 30000) {                              //general membership
		root->membership = 0;
	}
	if (root->balance >= 30000 && root->balance < 50000) {      //silver membership
		root->membership = 1;
	}
	if (root->balance >= 50000 && root->balance < 100000) {    //gold membership
		root->membership = 2;
	}
	if (root->balance >= 100000) {                             //diamond membership
		root->membership = 3;
	}
}

Node* RBT::addMember()
{
	Node *node = new Node(id);  //create a node
								//member initializations
	node->id = id;
	node->name = name;
	node->phone = phone;
	node->x = xCord;
	node->y = yCord;
	node->membership = membership;
	node->balance = balance;
	Node *nodecopy = node;      //copy of the node just in case changed during findDepth()
	root = adder(root, node);   //add to the RBT
	if (isfound == 1) {          //if same id already exists
		return root;            //just return the node
	}
	Ranker(node);               //update top 5 balance
	rebalance(root, node);          //rebalance RBT
	if (isfound == 2 && addIn == 1) {                 //if no same id found & successfully added a new member
		depth = 0;
		findDepth(nodecopy);                            //find the depth
	}
	if (land[nodecopy->x][nodecopy->y].first == 0) {          //if the land doesn's have any owner
		land[nodecopy->x][nodecopy->y].first = nodecopy->id;    //register as land owner
	}
	return nodecopy;
}

Node* RBT::adder(Node* root, Node *node) {  //add a member to the right location in the RBT
	if (root == NULL) {  //if same id is not found, insert new node
		isfound = 2;    //updat flag
		return node;
	}
	if (node->id > root->id) {                //if id is bigger
		depth++;                                //update the depth
		root->right = adder(root->right, node);   //right branch recursively
		root->right->parent = root;             //update the link direction

	}
	else if (node->id < root->id) {                 //if id is smaller
		depth++;                            //update the depth
		root->left = adder(root->left, node); //left branch recursively
		root->left->parent = root;          //update the link direction

	}
	else                                        //if same id found, return the node
		isfound = 1;                            //updat flag
	return root;
}

//rotate to the left for balancing (restructure) the RBT
void RBT::leftRotation(Node *&root, Node *&node) {
	Node *rightNode = node->right;  //right node of the node
	node->right = rightNode->left;  //
	if (node->right != NULL)
		node->right->parent = node;
	rightNode->parent = node->parent;
	if (node->parent == NULL)
		root = rightNode;
	else if (node == node->parent->left)
		node->parent->left = rightNode;
	else
		node->parent->right = rightNode;
	rightNode->left = node;
	node->parent = rightNode;
}

//rotate to the right for balancing (restructure) the RBT
void RBT::rightRotation(Node *&root, Node *&node) {
	Node *leftNode = node->left;    //left node of the node
	node->left = leftNode->right;
	if (node->left != NULL)
		node->left->parent = node;
	leftNode->parent = node->parent;
	if (node->parent == NULL)
		root = leftNode;
	else if (node == node->parent->left)
		node->parent->left = leftNode;
	else
		node->parent->right = leftNode;
	leftNode->right = node;
	node->parent = leftNode;
}

//find depth of the node in the RBT
void RBT::findDepth(Node *&node) {
	if (node == root)             //if it is the root
		return;
	else {
		depth++;                //update depth
		findDepth(node->parent);  //traverse bottom up
	}
}

//restore the balance (double red violation) of the Red black tree
void RBT::rebalance(Node *&root, Node *&node) {
	Node *grandpaNode = NULL;
	Node *parentNode = NULL;
	while ((node != root) && (node->parent->color == RED) && (node->color == RED)) {  //as long as double red violation happens and nor the root
		grandpaNode = node->parent->parent;     //update the grandpa node
		parentNode = node->parent;              //update the parent node
		if (parentNode == grandpaNode->right) {               //if parent is the right child of granpa
			Node *uncleNode = grandpaNode->left;    //update uncle node
			if ((uncleNode != NULL) && (uncleNode->color == RED)) {  //if uncle is red, then recolor
				uncleNode->color = BLACK;       //make uncle black
				parentNode->color = BLACK;      //make parent black
				grandpaNode->color = RED;       //make grandpa red
				node = grandpaNode;
			}
			else {       //if uncle is not red, then restructure
				if (node == parentNode->left) {        //if the node is parent's left child
					rightRotation(root, parentNode);  //rotate to the right
					node = parentNode;                //update the relations
					parentNode = node->parent;
				}
				leftRotation(root, grandpaNode);    //rotate to left, if right child or after right rotation of left child
				swap(parentNode, grandpaNode);      //switch colors
				node = parentNode;
			}
		}
		if (parentNode == grandpaNode->left) {   //if the parent node is on the left side grandpa
			Node *uncleNode = grandpaNode->right;
			if (uncleNode != NULL && uncleNode->color == RED) { //if uncle node is red and not null, then recolor
				uncleNode->color = BLACK;       //make uncle black
				parentNode->color = BLACK;      //make parent also black
				grandpaNode->color = RED;       //make grandpa red
				node = grandpaNode;
			}
			else {      //if uncle is not red, then restructure
				if (node == parentNode->right) {     //if node is the right child
					leftRotation(root, parentNode); //rotate to left
					node = parentNode;              //update the relations
					parentNode = node->parent;
				}
				rightRotation(root, grandpaNode);   //rotate to right if left child or after left rotaion of the right child
				swap(parentNode, grandpaNode);      //switch colors
				node = parentNode;
			}
		}
	}
	root->color = BLACK;                            //finally set the root's colod black
}

//update top 5 biggest balance amount, if a node is given
void RBT::Ranker(Node* pt) {
	if (big.size() >= 5) {                                            //if max 5 vector is full
		Node *minNode = big[0];
		int minId = 0;
		for (int i = 1; i < 5; i++) {
			if ((big[i]->id == pt->id) || (minNode->id == pt->id))   //if already in the vector
				return;
			//obtain the min from the vector of 5 biggest balance
			if (big[i]->balance < minNode->balance || (big[i]->balance == minNode->balance && big[i]->id > minNode->id)) {
				minNode = big[i];                                   //update min balanced member from the top 5
				minId = i;                                          //save the index
			}
		}
		//if current member has bigger balance or same but smaller id
		if (pt->balance > minNode->balance || (pt->balance == minNode->balance && pt->id < minNode->id))
			big[minId] = pt;                                        //insert to big 5 balance vector
	}
	else {
		big.push_back(pt);                                          //if max 5 vector is not full, insert to the vector
	}
}

//keep track of top 5 biggest balance amount from the whole RBT
void RBT::checkRank(Node *root)
{
	if (root == NULL)           //if empty RBT
		return;
	//recursively traverse in inorder
	checkRank(root->left);
	Ranker(root);               //make sure the top 5 biggest balance amount is up to date
	checkRank(root->right);
}

//handle land purchase transaction
void RBT::landPurchase(int id, int x, int y, int pay) {
	Node *buyer = searchNode(id);                   //buyer
	Node *seller = searchNode(land[x][y].first);    //previous owner of the land
	int landOwner = -1;                             //initialize to -1 just in case no owner at the end
	int permit = 0;                                 //permission to purchase is reset

	if (land[x][y].first == 0) {                          //if no owner exists
		if (buyer->balance >= pay) {                     //if buyer have enough balance
			landOwner = land[x][y].first = id;          //becomes land owner
			land[x][y].second = pay;                    //update land price
			buyer->balance -= pay;                      //charge from buyer's balance
			buyer->change.push_back({ 0, pay });          //update change history!!
			updateMembership(buyer);                    //update membership status
			permit = 1;                                 //allow the permission
			for (int i = 0; i < big.size(); i++) {
				if (big[i]->id == buyer->id)             //if buyer is among the top 5
					checkRank(root);                    //update the ranks of members
			}
		}
	}
	else {                                                       //if there is an owner of the land
		if (pay < land[x][y].second || buyer->balance < pay) {  //if paying less than the land price or low balance
			permit = 0;
			landOwner = land[x][y].first;                       //deny the transaction
		}
		else if (pay >= land[x][y].second && buyer->balance >= pay) {       //purchase successfully
			landOwner = land[x][y].first = id;              //becomes land owner
			land[x][y].second = pay;                        //update land price
			buyer->balance -= pay;                          //charge from buyer's balance
			buyer->change.push_back({ 0, pay });              //update change history!!
			seller->balance += pay;                         //add to seller's balance
			seller->change.push_back({ 1, pay });
			updateMembership(seller);                       //update membership status
			updateMembership(buyer);
			permit = 1;                                     //permit transaction
			for (int i = 0; i < big.size(); i++) {
				if (big[i]->id == buyer->id)                 //if among the top 5
					checkRank(root);
			}
		}
	}
	cout << permit << " " << buyer->balance << " " << landOwner << endl;     //print transaction
}

//insert a member
void insert(RBT* rbt, Node* node) {
	cin >> id >> name >> phone >> xCord >> yCord;     //get information
	addIn = 1;                  //set flag
	balance = 0;                //initialize
	depth = 0;
	rbt->addMember();                //insert information
	if (isfound == 1)                 //if same id is found
		cout << depth << " " << 0 << endl;
	else if (isfound == 2)            //if not found
		cout << depth << " " << 1 << endl;
}

//retrieve member's information
void information(RBT* rbt, Node* node) {
	cin >> id;
	node = rbt->searchNode(id);         //search the id
	if (node == NULL)
		cout << "Not found!" << endl;       //not found the id
	else                                //if found id, print the information
		cout << node->name << " " << node->phone << " " << node->membership << " " << node->balance << " " << depth << endl;
}

//recharges balance
void recharge(RBT* rbt, Node* node) {
	cin >> id >> charge;
	charging = 1;
	node = rbt->searchNode(id);         //search the id
	if (node == NULL)
		cout << "Not found!" << endl;       //not found the id
	else                                //charged successfully
		cout << depth << " " << node->membership << endl;
	charging = 0;
}

//get the top 5 rank members
void topRank(RBT* rbt, Node* node) {
	if (rbt->big.size() == 0)
		cout << "Not found!" << endl;       //not found members to print
	else {
		for (int i = 0; i < rbt->big.size() - 1; i++) {
			for (int j = i + 1; j < rbt->big.size(); j++) {
				if (rbt->big[i]->balance < rbt->big[j]->balance || ((rbt->big[i]->balance == rbt->big[j]->balance) && (rbt->big[i]->id > rbt->big[j]->id)))
					swap(rbt->big[i], rbt->big[j]);
			}
		}
		for (int i = 0; i < rbt->big.size(); i++)
			cout << rbt->big[i]->id << " " << rbt->big[i]->balance << endl;
	}
}

//recent modifications
void modification(RBT* rbt, Node* node) {
	int num = 0;
	cin >> id >> num;
	node = rbt->searchNode(id);
	if (node == NULL)
		cout << "Not found!" << endl;       //not found the id
	else if (node->change.size() == 0) {
		cout << "0" << endl;                //no change history
	}
	else                                //if there are changed history
		if (node->change.size() <= num) {
			for (long int i = node->change.size() - 1; i >= 0; i--) {
				cout << node->change.at(i).first << " " << node->change.at(i).second << endl;
			}
		}
		else {
			for (long int i = node->change.size() - 1; i >= node->change.size() - num; i--) {
				cout << node->change.at(i).first << " " << node->change.at(i).second << endl;
			}
		}
}

//purchase history
void purchase(RBT* rbt, Node* node) {
	int pay = 0;
	cin >> id >> xCord >> yCord >> pay;
	rbt->landPurchase(id, xCord, yCord, pay);
}

int main()
{
	/*freopen("query50.txt", "r", stdin);
	freopen("out.txt", "w", stdout);*/
	string fileName;
	char menu;
	isfound = 0;   //flag
				   //initializations
	depth = 0;
	membership = 0;
	balance = 0;
	addIn = 0;
	RBT* rbt = new RBT();
	Node *node = NULL;

	cout << "입력파일의 이름을 입력하세요 : " << endl;
	cin >> fileName;            //get file name
	fstream infile(fileName);   //open file

	if (!infile.is_open()) {
		cout << "Unable to Open\n";
		return 0;
	}

	while (infile >> id >> name >> phone >> xCord >> yCord >> membership >> balance) {    //read from file
		rbt->addMember();                //insert to RBT
	}

	while (cin >> menu) {
		if (menu == 'q' || menu == 'Q') {  //quit program
			break;
		}
		if (menu == 'i' || menu == 'I') {  //create a new member
			insert(rbt, node);
		}
		if (menu == 'p' || menu == 'P') { //print a member's information
			information(rbt, node);
		}
		if (menu == 'a' || menu == 'A') { //a member recharges cash
			recharge(rbt, node);
		}
		if (menu == 'f' || menu == 'F') { //print 5 members with highest amount
			topRank(rbt, node);
		}
		if (menu == 'r' || menu == 'R') { //print a member's recent modifications
			modification(rbt, node);
		}
		if (menu == 'b' || menu == 'B') { //a member purchases a land
			purchase(rbt, node);
		}
	}
	infile.close();
	return 0;
}

