#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <queue>
#include <stack>

using namespace std;

// Structura unui limbaj
struct Language {
	bool isSigmaStar;
	set<string> words;
	string opType;
};

// Se imparte expresia initiala intr-un vector de token-uri
vector<string> tokenize(string expr) {
	vector<string> tokens;
	size_t i = 0;
	
	while (i < expr.length()) {
		if (expr[i] == ' ') {
			i++;
			continue;
		}

		if (expr[i] == '(' || expr[i] == ')') {
			string t;
			t.push_back(expr[i]);
			
			tokens.push_back(t);
			i++;

		} else {
			string token = "";

			while (i < expr.length() && expr[i] != ' '
				   && expr[i] != '(' && expr[i] != ')') {
				token += expr[i];
				i++;
			}

			tokens.push_back(token);
		}

	}
	return tokens;
}

// Se genereaza toate cuvintele posibile din alfabet pana la lungimea maxima folosind o coada (BFS)
set<string> getAllWords(int maxLen, string alphabet) {
	set<string> result;
	result.insert("");
	
	queue<string> q;
	q.push("");
	
	while (!q.empty()) {
		string curr = q.front();
		q.pop();
		
		if ((int)curr.length() >= maxLen) {
			continue;
		}
		
		for (int i = 0; i < (int)alphabet.length(); i++) {
			string newWord = curr + alphabet[i];
			
			if (!result.count(newWord)) {
				result.insert(newWord);

				if ((int)newWord.length() < maxLen) {
					q.push(newWord);
				}
			}
		}
	}

	return result;
}

// Se genereaza doar cuvintele de o lungime fixa pentru a optimiza verificarea echivalentei (tot cu BFS)
set<string> getWordsOfLength(int len, string alphabet) {
	set<string> result;

	if (len == 0) {
		result.insert("");
		return result;
	}
	
	queue<string> q;
	q.push("");
	
	while (!q.empty()) {
		string curr = q.front();
		q.pop();
		
		if ((int)curr.length() == len) {
			result.insert(curr);
		} else {
			for (int i = 0; i < (int)alphabet.length(); i++) {
				q.push(curr + alphabet[i]);
			}
		}
	}

	return result;
}

// Se evalueaza expresia Star-Free folosind o stiva si returneaza limbajul rezultat
Language solveLanguage(string expression, int maxLen, string alphabet) {
	vector<string> tokens = tokenize(expression);
	stack<Language> st;

	for (size_t i = 0; i < tokens.size(); i++) {
		string t = tokens[i];
		Language item;

		if (t == "(" || t == "CAT" || t == "UNION" || t == "COMP") {
			item.opType = t;
			item.isSigmaStar = false;
			st.push(item);

		} else if (t == "NIL") {
			item.opType = "";
			item.isSigmaStar = false;
			st.push(item);

		} else if (t == ")") {
			vector<Language> args;
			
			while (!st.empty() && st.top().opType == "") {
				args.push_back(st.top());
				st.pop();
			}
			
			string op = "";
			if (!st.empty()) {
				op = st.top().opType;
				st.pop();
			}

			if (!st.empty() && st.top().opType == "(") {
				st.pop();
			}

			Language res;
			res.opType = "";
			res.isSigmaStar = false;

			if (op == "COMP") {
				Language L = args[0];

				if (L.isSigmaStar) {
					res.isSigmaStar = false;
				} else if (L.words.empty()) {
					res.isSigmaStar = true;
				} else {
					set<string> allWords = getAllWords(maxLen, alphabet);

					for (string w : allWords) {
						if (!L.words.count(w)) {
							res.words.insert(w);
						}
					}
				}

			} else if (op == "UNION") {
				bool isAll = false;

				for (int k = 0; k < (int)args.size(); k++) {
					if (args[k].isSigmaStar) {
						isAll = true;
						break;
					} else {
						for (string w : args[k].words) {
							res.words.insert(w);
						}
					}
				}

				if (isAll) {
					res.isSigmaStar = true;
					res.words.clear();
				}

			} else if (op == "CAT") {
				if (args.empty()) {
					res.words.insert("");
				} else {
					bool anyVoid = false;

					for (int k = 0; k < (int)args.size(); k++) {
						if (!args[k].isSigmaStar && args[k].words.empty()) {
							anyVoid = true;
							break;
						}
					}

					if (!anyVoid) {
						bool hasSigma = false;
						bool restEpsilon = true;
						
						for (int k = 0; k < (int)args.size(); k++) {
							if (args[k].isSigmaStar) {
								hasSigma = true;
							} else {
								if (args[k].words.size() != 1
									|| !args[k].words.count("")) {
									restEpsilon = false;
								}
							}
						}

						if (hasSigma && restEpsilon) {
							res.isSigmaStar = true;
						} else {
							res.words.insert("");

							set<string> universe;
							bool universeGenerated = false;

							for (int k = (int)args.size() - 1; k >= 0; k--) {
								set<string> nextRes;
								set<string> currentPartWords;

								if (args[k].isSigmaStar) {
									if (!universeGenerated) {
										universe = getAllWords(maxLen, alphabet);
										universeGenerated = true;
									}

									currentPartWords = universe;
								} else {
									currentPartWords = args[k].words;
								}

								for (string w1 : res.words) {
									for (string w2 : currentPartWords) {
										string concat = w1 + w2;

										if ((int)concat.length() <= maxLen) {
											nextRes.insert(concat);
										}
									}
								}
								
								res.words = nextRes;
							}
						}
					}
				}
			}

			st.push(res);

		} else {
			item.opType = "";
			item.isSigmaStar = false;
			item.words.insert(t);
			st.push(item);
		}
	}

	return st.top();
}

// Se verifica apartenenta fiecarui cuvant la limbaj
void solveMembership(ifstream &fin, ofstream &fout, string alphabet) {
	string expression;
	fin.ignore();

	getline(fin, expression); 
	
	int n;
	fin >> n;
	fin.ignore(); 
	
	vector<string> words(n);
	int maxLen = 0;
	
	for (int i = 0; i < n; i++) {
		getline(fin, words[i]);

		if ((int)words[i].length() > maxLen) {
			maxLen = words[i].length();
		}
	}
	
	Language L = solveLanguage(expression, maxLen, alphabet);
	
	for (int i = 0; i < n; i++) {
		bool accepted = false;
		
		if (L.isSigmaStar) {
			accepted = true;
		} else if (L.words.count(words[i]) > 0) {
			accepted = true;
		}
		
		if (accepted) {
			fout << "YES\n";
		} else {
			fout << "NO\n";
		}
	}
}

// Se verifica daca doua limbaje sunt identice testand toate cuvintele posibile pana la o lungime limita (maxLen)
void solveEquivalence(ifstream &fin, ofstream &fout, string alphabet) {
	string expr1, expr2;
	fin.ignore();

	getline(fin, expr1);
	getline(fin, expr2); 
	
	int maxLen = 12;
	Language L1 = solveLanguage(expr1, maxLen, alphabet);
	Language L2 = solveLanguage(expr2, maxLen, alphabet);
	
	bool equivalent = false;
	string counter = "";
	
	if (L1.isSigmaStar && L2.isSigmaStar) {
		equivalent = true;
	} else if (L1.isSigmaStar || L2.isSigmaStar) {
		set<string> all = getAllWords(maxLen, alphabet);
		
		Language finiteLang;
		if (L1.isSigmaStar) {
			finiteLang = L2;
		} else {
			finiteLang = L1;
		}

		for (string w : all) {
			if (finiteLang.words.count(w) > 0) {
				counter = w; 
				break;
			}
		}
		
	} else {
		if (L1.words == L2.words) {
			equivalent = true;
		} else {
			for (int len = 0; len <= maxLen; len++) {
				if (!counter.empty()) {
					break;
				}
				
				set<string> check = getWordsOfLength(len, alphabet);
				
				for (string w : check) {
					bool in1 = (L1.words.count(w) > 0);
					bool in2 = (L2.words.count(w) > 0);
					
					if (in1 != in2) { 
						counter = w; 
						break; 
					}
				}
			}
		}
	}
	
	if (equivalent) {
		fout << "YES\n";
	} else {
		fout << "NO\n";
		fout << counter << "\n";
		
		bool acc1 = false;
		if (L1.isSigmaStar || L1.words.count(counter) > 0) {
			acc1 = true;
		}
		
		bool acc2 = false;
		if (L2.isSigmaStar || L2.words.count(counter) > 0) {
			acc2 = true;
		}
		
		if (acc1) {
			fout << "YES ";
		} else {
			fout << "NO ";
		}

		if (acc2) {
			fout << "YES\n";
		} else {
			fout << "NO\n";
		}
	}
}

int main() {
	ifstream fin("input.txt");
	ofstream fout("output.txt");
	
	string queryType, alphabet;
	fin >> queryType >> alphabet;
	
	if (queryType == "MEMBERSHIP") {
		solveMembership(fin, fout, alphabet);
	} else if (queryType == "EQUIVALENCE") {
		solveEquivalence(fin, fout, alphabet);
	}
	
	fin.close();
	fout.close();
	return 0;
}
