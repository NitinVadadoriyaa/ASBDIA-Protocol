#include <iostream>
#include <bits/stdc++.h>

#define SEED 100

using namespace std;
int main() {
	string fileName;
	cout << "FileName : ";
	cin >> fileName;
	const char* filePath = fileName.c_str();
	std::ifstream readFile(filePath, std::ios::binary);
	
	if (!readFile.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return 0;
    	}
    	readFile.seekg(0, std::ios::end);
	streampos fileSize = readFile.tellg();
	cout << "File size : " << static_cast<long long>(fileSize) << endl;
	readFile.close();
	
	double percent = 0;
	cout << "Enter percentage : ";
	cin >> percent;
	
	long long p_block = ceil((static_cast<long long>(fileSize) * percent) / 100.0); // * 8 --> because we want to modifiy on byte.
	cout << p_block << " bits Modification is started randomly.." << endl;
	
	srand(SEED);
	unordered_map<long long,bool>hash;
	
	fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
	//ofstream file(filePath,std::ios::binary);
	
	if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 0;
    }
	long long fsize = static_cast<long long>(fileSize);
	for(long long i = 0; i < p_block; i++) {
		long long r;
		while(true) {
			r = rand() % fsize;
			if (hash.find(r) == hash.end()) {
				hash[r] = true;
				break;
			}
		}
		char byte = 0b00000000;
		file.seekp(r);
		//file.seekp(r);
		file.read(&byte,1);
		bitset<8> bit(byte);
		int ind = rand() % 8;
		bit.flip(ind);
		//cout << r << " " << ind << endl;
		file.seekp(r);
		file.write(reinterpret_cast<char*>(&bit), 1);
	}
	
	
	file.close();
	return 0;
}
