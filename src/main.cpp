
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include "utils\cli.h";
#include "utils\filesystem.h"

namespace fs = std::filesystem;
using namespace std;
using namespace Utils;

void runDifferences(const fs::path& first, const fs::path& second, const fs::path& output);
fs::path getInputPath(string_view ordinal);
fs::path getOutputPath();
istream& getPath(istream& input, fs::path& path);
bool isExactMatch(const fs::path& first, const fs::path& second);
void printLog(const string& msg, ostream* logger = nullptr);
string searchMsg(const fs::path& first, const fs::path& second);
string openErrorMsg(const fs::path& path);
string missingMsg(const fs::path& path);
string mismatchMsg(const fs::path& first, const fs::path& second);
string resultsMsg(long long count, long long missing, long long matches, long long mismatches, long long errors);

int main() {
	try {
		const auto first = getInputPath("first");
		const auto second = getInputPath("second");
		const auto output = getOutputPath();

		runDifferences(first, second, output);
	}
	catch( ... ) {
		cout << "An unknown error occurred. Exiting...";
	}
}

void runDifferences(const fs::path& first, const fs::path& second, const fs::path& output) {
	ofstream logFile;
	ofstream* logger = nullptr;
	long long missing = 0;
	long long matches = 0;
	long long mismatches = 0;
	long long errors = 0;
	long long count = 0;

	if( !output.empty() ) {
		logFile.open(output);
		logger = &logFile;
	}

	printLog(searchMsg(first, second), logger);
	printLog("\n");

	for( const auto& entry : fs::recursive_directory_iterator(first) ) {
		cout << '\b';

		try {
			const fs::path srcFile = entry.path();

			if( !is_directory(srcFile) ) {
				++count;

				const fs::path altFile = findCorrespondingFile(srcFile, first, second);

				if( !exists(altFile) ) {
					++missing;
					printLog(missingMsg(altFile), logger);
				}
				else if( !isExactMatch(srcFile, altFile) ) {
					++mismatches;
					printLog(mismatchMsg(srcFile, altFile), logger);
				}
				else {
					++matches;
				}
			}
		}
		catch( const exception& ex ) {
			++errors;
			printLog(ex.what() + '\n', logger);
		}

		cout << getHourglassCharacter();
	}
	cout << "\b ";

	printLog("\n");
	printLog(resultsMsg(count, missing, matches, mismatches, errors), logger);
}

fs::path getInputPath(string_view ordinal) {
	fs::path path;

	cout << "Enter the " << ordinal << " directory to examine: \n";
	getPath(cin, path);
	while( !exists(path) || !is_directory(path) ) {
		cout << path << (!exists(path) ? " does not exist" : " is not a directory") << ". Try again : \n";
		getPath(cin, path);
	}
	cout << endl;

	return path;
}

fs::path getOutputPath() {
	fs::path path;
	ofstream file;

	cout << "Enter the path to an output file, or enter to skip:\n";
	getPath(cin, path);
	if( !path.empty() )
		file.open(path);
	while( !path.empty() && !file ) {
		cout << "Could not open output file at " << path << ". Enter a valid path where you have write access:\n";
		getPath(cin, path);
		file.open(path);
	}
	cout << endl;

	return path;
}

istream& getPath(istream& input, fs::path& path) {
	string line;

	getline(input, line);
	path = line;

	return input;
}

bool isExactMatch(const fs::path& first, const fs::path& second) {
	auto file1 = ifstream(first, ios::binary);
	if( !file1 )
		throw runtime_error(openErrorMsg(first));

	auto file2 = ifstream(second, ios::binary);
	if( !file2 )
		throw runtime_error(openErrorMsg(second));

	return equal(
		istreambuf_iterator(file1.rdbuf()),
		{},
		istreambuf_iterator(file2.rdbuf())
	);
}

void printLog(const string& msg, ostream* logger) {
	cout << msg;
	if( logger != nullptr )
		(*logger) << msg;
}

string searchMsg(const fs::path& first, const fs::path& second) {
	return "Searching for differences between:\n"
		"  \"" + first.string() + "\"\n"
		"  \"" + second.string() + "\"\n";
}

string openErrorMsg(const fs::path& path) {
	return "Failed to open \"" + path.string() + "\".";
}

string missingMsg(const fs::path& path) {
	return "\"" + path.string() + "\" does not exist.\n";
}

string mismatchMsg(const fs::path& first, const fs::path& second) {
	return "Not a match:\n"
		"  \"" + first.string() + "\"\n"
		"  \"" + second.string() + "\"\n";
}

string resultsMsg(long long count, long long missing, long long matches, long long mismatches, long long errors) {
	ostringstream ss;

	ss
		<< "Finished with " << errors << " error" << (errors == 1 ? "" : "s") << "\n"
		<< "Examined " << count << " file" << (count == 1 ? "" : "s") << ":\n"
		<< "  " << missing << " missing" << "\n"
		<< "  " << matches << " match" << (matches == 1 ? "" : "es") << "\n"
		<< "  " << mismatches << " mismatch" << (mismatches == 1 ? "" : "es") << "\n";

	return ss.str();
}