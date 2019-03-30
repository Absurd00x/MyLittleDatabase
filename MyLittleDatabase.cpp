#include "pch.h"
#include <algorithm>
#include <clocale>
#include <ctime>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>

using namespace std;

#define database unordered_map<int, Student>
#define function_list unordered_map<string, void(*)(database&)>

class Student {
private:
	int id;
public:
	string name, group;
	int age;
	Student(string name, string group, int age, int id) {
		this->name = name;
		this->group = group;
		this->age = age;
		this->id = id;
	}
	Student() {
		name = "";
		group = "";
		age = -1;
		id = -1;
	}
	void show_all(ostream &out) {
		const string tab = "\t\t";
		string output_name = name;
		output_name.resize(20, ' ');
		out << id << tab << output_name << tab << age << tab << group << endl;
	}
};

void save_to_file(database &db, ofstream &out) {
	for (auto &it : db)
		it.second.show_all(out);
}

bool is_number(const string &s) {
	if (s.length() == 0)
		return false;
	for (auto symbol : s)
		if (!isdigit(symbol))
			return false;
	return true;
}

string itos(int num) {
	string res;
	while (num > 0) {
		res.push_back(char(num % 10));
		num /= 10;
	}
	res.reserve();
	return res;
}

void tolower(string &str) {
	for (char &symb : str)
		symb = char(tolower(symb));
}

void getstring(string &s) {
	do {
		getline(cin, s);
	} while (!s.length());
}

void add(database &db) {
	string name, group;
	int age, id = rand() + rand() + rand();
	cout << "Enter student's name:\n";
	getstring(name);
	cout << "Enter student's age:\n";
	string input;
	bool correct;
	do {
		correct = true;
		getstring(input);
		if (!is_number(input))
			correct = false;
		else
			age = stoi(input);
	} while (!correct);
	cout << "Enter student's group:\n";
	getstring(group);
	db[id] = Student(name, group, age, id);
}

void show(database &db) {
	cout << "id\t\tName\t\t\t\tAge\t\tGroup\n";
	for (auto &it : db)
		it.second.show_all(cout);
}

void save(database &db) {
	ofstream file_out("database.txt");
	if (!file_out)
		cout << "File cannot be opened!\n";
	else {
		if (!file_out)
			save_to_file(db, file_out);
		else {
			cout << "Your file is not empty. Should I wipe it? (y/n) ";
			string response;
			do {
				getstring(response);
				tolower(response);
				if (response == "y")
					save_to_file(db, file_out);
				else if (response != "n") {
					cout << "You must answer \"y\" or \"n\"\n";
				}
			} while (!(response == "y" || response == "n"));
		}
	}
	file_out.close();
}

void load(database &db) {
	ifstream file_in("database.txt");
	string name, group;
	int age, id;
	while (file_in >> id >> name >> age >> group)
		db[id] = Student(name, group, age, id);
	file_in.close();
}

void wipe(database &db) {
	ofstream file("database.txt");
	file.clear();
	file.close();
}

void quit(database &db) {
	cout << "Bye!\n";
	Sleep(1000);
	exit(0);
}

void help(database &db) {
	cout << "add     - add a new record to database\n";
	cout << "correct - find a record by id and change one of its other fields\n";
	cout << "count   - show the number of records there are in database\n";
	cout << "delete  - delete a record from database by id\n";
	cout << "exit    - exit the program\n";
	cout << "find    - show all records in database by id, name, age or group\n";
	cout << "load    - load records from a text file\n";
	cout << "save    - save current records to a text file\n";
	cout << "show    - show all the records in database\n";
	cout << "sort    - show all records sorting by name, age or group\n";
	cout << "wipe    - clear the file data\n";
}

void count(database &db) {
	printf("There are %d records currently loaded to the database\n", db.size());
}

string get_valid_string(const string to_get) {
	cout << "Enter " << to_get << ":\n";
	string name;
	getstring(name);
	return name;
}

int get_valid_number(const string to_get) {
	cout << "Enter " << to_get << ":\n";
	string input;
	bool valid;
	int number;
	do {
		getstring(input);
		if (input.length()) {
			valid = is_number(input);
			if (valid)
				number = stoi(input);
			else
				cout << '"' << to_get << "\" must be an integer number\n";
		}
	} while (!valid);
	return number;
}

void print_found(vector<Student*> &found, const string type, const string value) {
	if (!found.empty())
		for (auto student : found)
			(*student).show_all(cout);
	else {
		cout << "There are no records with such " << type;
		cout << " -> " << value << endl;
	}
}

void find_by_name(database &db) {
	string name = get_valid_string("name");
	vector<Student*> result;
	for (auto &student : db)
		if (student.second.name == name)
			result.push_back(&student.second);
	print_found(result, "name", name);
}

void find_by_group(database &db) {
	string group = get_valid_string("group");
	vector<Student*> result;
	for (auto &student : db)
		if (student.second.group == group)
			result.push_back(&student.second);
	print_found(result, "group", group);
}

void find_by_age(database &db) {
	int age = get_valid_number("age");
	vector<Student*> result;
	for (auto &student : db)
		if (student.second.age == age)
			result.push_back(&student.second);
	print_found(result, "age", itos(age));
}

void find_by_id(database &db) {
	int id = get_valid_number("id");
	if (db.count(id))
		db[id].show_all(cout);
	else
		cout << "There is no record with such id -> " << id << endl;
}

void find(database &db) {
	cout << "Select either \"id\", \"name\", \"age\" or \"group\"";
	cout << " to find particular records\n";
	const set<string> answers = { "id", "name", "age", "group" };
	function_list find_by = { {"id", &find_by_id}, {"name", &find_by_name},
		{"age", &find_by_age}, {"group", &find_by_group} };
	string input;
	do {
		getstring(input);
		tolower(input);
		if (answers.count(input))
			find_by[input](db);
		else if (input == "discard")
			break;
		else {
			cout << "You must choose between the following parametrs:\n";
			cout << "\"id\", \"name\", \"age\", \"group\"\n";
			cout << "Type \"discard\" to exit instruction\n";
		}
	} while (!answers.count(input));
}

void erase(database &db) {
	const int id = get_valid_number("id");
	db.erase(id);
	cout << "Record with id " << id << " was deleted\n";
}

void correct(database &db) {
	int id = get_valid_number("id");
	if (db.count(id)) {
		cout << "Which field do you want to correct?\n";
		string field;
		getstring(field);
		tolower(field);
		const set<string> answers = { "name", "age", "group" };
		do {
			if (answers.count(field)) {

			} else if (field == "id")
				cout << "You cannot change id of the record\n";
			else if (field == "discard")
				break;
			else {
				cout << "You can change the record either by ";
				cout << "its name, age or group\n";
				cout << "Type \"discard\" to exit instruction\n";
			}
		} while (!answers.count(field));
	} else
		cout << "There is no record with such id -> " << id << endl;
}

void show_sorted(database &db) {
	cout << "Choose by which field do you want to sort the records\n";
	const set<string> answers = { "id", "name", "age", "group" };
	string field;
	do {
		getstring(field);
		tolower(field);
		if (answers.count(field)) {
			if (field == "age" || field == "id") {
				vector<pair<int, Student*>> to_sort;
				if (field == "age")
					for (auto it = db.begin(); it != db.end(); it++)
						to_sort.push_back({ (*it).second.age, &(*it).second });
				else
					for (auto it = db.begin(); it != db.end(); it++)
						to_sort.push_back({ (*it).first, &(*it).second });
				sort(to_sort.begin(), to_sort.end());
				cout << "id\t\tName\t\t\t\tAge\t\tGroup\n";
				for (auto &sorted : to_sort)
					sorted.second->show_all(cout);
			} else {
				vector<pair<string, Student*>> to_sort;
				if (field == "name")
					for (auto it = db.begin(); it != db.end(); it++)
						to_sort.push_back({ (*it).second.name, &(*it).second });
				else
					for (auto it = db.begin(); it != db.end(); it++)
						to_sort.push_back({ (*it).second.group, &(*it).second });
				sort(to_sort.begin(), to_sort.end());
				cout << "id\t\tName\t\t\t\tAge\t\tGroup\n";
				for (auto &sorted : to_sort)
					sorted.second->show_all(cout);
			}
		} else {
			cout << "You must type either \"id\", \"name\" ";
			cout << "\"age\" or \"group\"\n";
		}
	} while (!answers.count(field));
}

int main() {
	srand(unsigned int(time(NULL)));
	database db;
	function_list commands = { {"add", &add}, {"show", &show}, {"save", &save},
		{"load", &load}, {"wipe", &wipe}, {"exit", &quit}, {"help", &help},
		{"find", &find}, {"delete", &erase}, {"correct", &correct},
		{"sort", &show_sorted} };
	string query = "None";
	cout << "Print \"quit\" to terminate the program\n";
	while (true) {
		printf("Enter your command:\n");
		getstring(query);
		tolower(query);
		if (commands.count(query))
			commands[query](db);
		else {
			cout << "Invalid command\n";
			cout << "Type \"help\" to see the list of available commands\n";
		}
	}
}

