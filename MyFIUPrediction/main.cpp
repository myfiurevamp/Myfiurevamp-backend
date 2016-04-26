/*
 
 THINGS TO CHANGE:
 
 -define macros to have the correct parameters to login to database
 -course table name (i.e., taken_course -> courses)
 -ignored column nums to have indexes of columns whose values are to be ignored (i.e. possibly NULL values such as grade)
 -ignore attributes to match Alastair's schema
 
 */

#include "stdafx.h"
#include <tr1/memory>
#include <iostream>
#include "include/my_fiu_revamp_pg_parser.h"

#define DBNAME "myfiurevamp" //CHANGE THESE MACROS TO HAVE THE RIGHT LOGIN PARAMETERS
#define USER "postgres"
#define PASSWORD "JAA~cen4010"
#define HOSTADDR "162.243.21.173"
#define PORT "5432"

std::string conn_string;

int main(int argc, char* argv[])
{
	using namespace std;

	vector<pair <string, string> > conn_parameters;
	
	conn_parameters.push_back(pair<string, string>("dbname", DBNAME));
	conn_parameters.push_back(pair<string, string>("user", USER));
	conn_parameters.push_back(pair<string, string>("password", PASSWORD));
	conn_parameters.push_back(pair<string, string>("hostaddr", HOSTADDR));
	conn_parameters.push_back(pair<string, string>("port", PORT));
	
	for(pair<string, string> parameter_pair : conn_parameters)
	  {
	    string parameter_string = parameter_pair.first + "=" +parameter_pair.second + " ";
	    conn_string += parameter_string;
          }
	
	conn_string.pop_back();

	/* Open and parse file with parser object, and get relationObj */
	MyFIUPgParser* parser_obj = new MyFIUPgParser();
	parser_obj->setCourseTableName("taken_course"); // CHANGE "taken_course" TO "course"
	int open_file_error_num = parser_obj->openFile(conn_string);

	parser_obj->setTypes(vector<string>({"numeric", "string", "string", "string", "nominal", "string", "numeric", "string"})); //CHANGE TO "numeric", "string", "string", "string", "string", "string", "numeric"
	parser_obj->setIgnoredColumnNums({5,6,7}); // CHANGE "5,6,7" TO ONLY "5"

	cout << "Parsing Student Data...\n";
	int parse_file_error_num = parser_obj->parseFile();

	parser_obj->closeFile(); //redundant due to delete parse_obj call; delete SHOULD close file automatically if parser object is implemented correctly
	cout << "Done Parsing Student Data...\n";
	cout << "Extracting Relation Object...\n";
	relationObj relation_obj = parser_obj->getRelationObj();
	cout << "Deleting Parsing Object...\n";
	delete parser_obj;

	/* Create classifier object (classification algorithm), and build model based off of relation and class_label */
	string class_label = "grade_received";
	cout << "Initializing Decision Tree...\n";
	J48DecisionTree* classifier = new J48DecisionTree(relation_obj, class_label); 
	string def = classifier->relation_obj.getTable().getColumn(relation_obj.getIndexOfAttribute(class_label)).getMajorityValue();
	classifier->setDefaultClassLabel(def);
	// vector<string> ignored_attributes = { "transcript_id", "course_name", "tc_credits" /*"semester_taken",*/ /*"course_num"*/}; // CHANGE TO MATCH ALASTAIR'S SCHEMA

	// //cout << "Building Decision Tree...\n";
	// //classifier->buildDecisionTree(); // predict should automatically call buildDecisionTree()
	// //cout << "Done building Decision Tree...\n";

	// /* Predict "grade_received" class label for record 
	// (8, COP 5725, Fall 2016, ? , ? , Principles of Database Management Systems, Computer Programming)
	// */
	// vector<string> record_values = 
	//   {"8", "COP 5727", "Fall 2016", "?", "?", "Principles of Database Management Systems", "3", "Computer Programming"};

	// tableRow record = tableRow(record_values.begin(), record_values.end());
	// string predicted_class_label = classifier->predictIgnore(record, ignored_attributes.begin(), ignored_attributes.end());

	cout << "Full Decision Tree:\n" << *classifier;

	// /* Print record */
	// cout << "\nPredicted class label of record (";
	// int i = 0;
	// for (string value : record_values)
	// {
	// if (i == 0)
	// cout << value;
	// else
	// cout << "," << value;

	// i++;
	// }
	// cout <<  "): " << predicted_class_label << endl;
	

	/* Predict "PlayTennis" class label for record (14,Rain,Mild,High,Strong,No) */
	//vector<string> record_values = {"15", "Rain", "Hot", "High", "Weak", "No"};
	//tableRow record = tableRow(record_values.begin(), record_values.end());
	//string predicted_class_label = classifier->predict(record);

	/* Print record */
	/*cout << "\nPredicted class label of record (";
	int i = 0;
	for (string value : record_values)
	{
		if (i == 0)
			cout << value;
		else
			cout << "," << value;

		i++;
	}
	cout <<  "): " << predicted_class_label << endl;
*/
	//delete classifier;


    return 0;
}
