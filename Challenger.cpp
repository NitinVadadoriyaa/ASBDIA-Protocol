#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <cryptopp/gf256.h>
#include <unistd.h> // for sleep() method

#define PERIOD 1

using namespace CryptoPP;
using namespace std;

//int PROCESS_BLOCK_NUM = 100; // no of random block in which we only interested... TAKEN FROM USER...
long long SEED_VECTOR_VALUE = 100; // Generate vector 1 * n -values..
int TAG_SECTOR_NUM = 3; //TAG_SECTOR_NUM ---> no of sector in tag-file
int DATAFILE_SECTOR_NUM = 100000; //DATAFILE_SECTOR_NUM ---> no of sector in data-file / mue.bin file
int TAG_ROWS_COUNT = 11535; // Number of TAG_ROWS_COUNT in Tag File..
long long SEED_BLOCK_NUMBER = 101;

GF256::Element calculate_mac(vector<GF256::Element> &block, GF256::Element key,int DATAFILE_SECTOR_NUM);
bool verifyProof(int PROCESS_BLOCK_NUM);
void Send_msg(const char *server_ip);

int main() {
	Send_msg("172.16.15.6");
	return 0;
}

void Send_msg(const char *server_ip) {
	int client_fd;
	char buffer[1024];
	memset(buffer,0,sizeof(buffer));
	
	struct sockaddr_in serv_addr_prover, serv_addr_admin;
	socklen_t server_addr_len = sizeof(serv_addr_prover);
	if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		return;
	}

	serv_addr_prover.sin_family = AF_INET;
	serv_addr_prover.sin_port = htons(8011); // os automatically assign available port
	
	
	
	if (inet_pton(AF_INET, server_ip, &serv_addr_prover.sin_addr) <= 0) {
		perror("inet_pton");
		return;
	}
	
	srand(time(0));
	while(true) {
	//-------------------------------- PROCESS_BLOCK_NUM change hear ------------------------- //
		cout << "Enter percent to challange : ";
		double per;
		cin >> per;
		int PROCESS_BLOCK_NUM = ceil((per * TAG_ROWS_COUNT) / 100.0);
		cout << "Challange is sending....!" << endl;
		
		//--------make seed random--------------//
		SEED_VECTOR_VALUE = rand();
		SEED_BLOCK_NUMBER = SEED_VECTOR_VALUE + 1;
		
		string str = to_string(SEED_VECTOR_VALUE);
		
		sendto(client_fd, str.c_str(), str.length(), 0, (struct sockaddr *)&serv_addr_prover, sizeof(serv_addr_prover));
		
		str = to_string(PROCESS_BLOCK_NUM);
		sendto(client_fd, str.c_str(), str.length(), 0, (struct sockaddr *)&serv_addr_prover, sizeof(serv_addr_prover));
		
		std::ofstream outputFile("Mue.bin", std::ios::binary);
		int n;
		while (true) {
		    n = recvfrom(client_fd, (char *)buffer, sizeof(buffer), 0, (struct sockaddr*)&serv_addr_prover, &server_addr_len);
		    if (n <= 0) {
		        break;
		    }

		    outputFile.write(buffer, n);
		}
		
		outputFile.close();
		
		if (!verifyProof(PROCESS_BLOCK_NUM)) {
			// connect to admin...
			serv_addr_admin.sin_family = AF_INET;
			serv_addr_admin.sin_port = htons(8005); // os automatically assign available port
			
			const char *server_ip_admin = "172.16.15.7";
			if (inet_pton(AF_INET, server_ip_admin, &serv_addr_admin.sin_addr) <= 0) {
				perror("inet_pton");
				return;
			}
			
			char *msg = "Something Goes Wrong....!";
			sendto(client_fd, msg, strlen(msg), 0, (struct sockaddr *)&serv_addr_admin, sizeof(serv_addr_admin));
			cout << endl << "Error is reported..." << endl;
		}
		cout << endl << "--------------------------------------------------------------------------------------------" << endl << endl;
		sleep(PERIOD);
	
	}
	
	close(client_fd);
}

bool verifyProof(int PROCESS_BLOCK_NUM) {
	vector<GF256::Element>key = {123,200,15};
	
	/* --------------------------------- calculate Tau1 ---------------------------------------------- */
    std::ifstream readFile("Tag.bin", std::ios::binary);
    if (!readFile.is_open()) {
        std::cerr << "Error opening file: Tag.bin"  << std::endl;
        return false;
    }
    std::ofstream writeFile("Tau1.bin", std::ios::binary);
    if (!writeFile.is_open()) {
        std::perror("Error opening output file");
        return false;
    }
    readFile.seekg(0, std::ios::end);
    streampos fileSize = readFile.tellg();
    readFile.seekg(0, std::ios::beg);
  	vector<GF256::Element> finalRes(TAG_SECTOR_NUM,0);
  	vector<GF256::Element> buffer(TAG_SECTOR_NUM,0);
	GF256 gf256(0x11B); //100011011
	
	//generate random bolck number unique
  	srand(SEED_BLOCK_NUMBER); // ----------------------------------------------------- SEED_BLOCK_NUMBER ----------------------------------------------------//
  	
  	unordered_map<int,bool>hash;
  	vector<int>block_ind(PROCESS_BLOCK_NUM);
  	for(int i = 0; i < PROCESS_BLOCK_NUM; i++) {
  		while(true) {	
	  		int blockNum = rand() % TAG_ROWS_COUNT;
	  		if (hash.find(blockNum) == hash.end()) {
	  			hash[blockNum] = true;
  				block_ind[i] = blockNum;
  				break;
	  		}
  		}
  	}
  	sort(block_ind.begin(),block_ind.end());//-----11534 is last block number && after reading it pointer move to end of file that why HANDLE IT!

	//TAG_SECTOR_NUM --> no of sector in tag-file in one block
	srand(SEED_VECTOR_VALUE);
	for(int block = 0; block < PROCESS_BLOCK_NUM; block++) {
		
		readFile.seekg(TAG_SECTOR_NUM * block_ind[block], std::ios::beg); //-----------HEAR I CHANGE -------------//
		readFile.read(reinterpret_cast<char*>(buffer.data()), TAG_SECTOR_NUM);
		int val = rand() % 256;
		
		
		for (int i = 0; i < TAG_SECTOR_NUM; i++) {
  			GF256::Element mul = gf256.Multiply(buffer[i], static_cast<GF256::Element>(val));
  			finalRes[i] = gf256.Add(mul,finalRes[i]);
  		}
		buffer = vector<GF256::Element> (TAG_SECTOR_NUM,0);
	}
		

     for (int i = 0; i < TAG_SECTOR_NUM; i++) {
    	writeFile.write(reinterpret_cast<const char*>(&finalRes[i]), sizeof(GF256::Element)); // write on file
    	cout << hex << setw(2) << setfill('0') << static_cast<int>(finalRes[i]);
		if (TAG_SECTOR_NUM - i != 1) cout << "::"; 
    } 
    cout << endl;
    readFile.close();
    writeFile.close();
    
	/* --------------------------------- calculate Tau2 ---------------------------------------------- */
	
	int key_len = key.size();
	buffer.resize(DATAFILE_SECTOR_NUM,0);
    std::ifstream readFile1("Mue.bin", std::ios::binary);
    if (!readFile1.is_open()) {
        std::cerr << "Error opening file: Mue.bin" << std::endl;
        return false;
    }
    std::ofstream writeFile1("Tau2.bin", std::ios::binary);
    if (!writeFile1.is_open()) {
        std::perror("Error opening output file");
        return false;
    }
    readFile1.seekg(0, std::ios::end);
    fileSize = readFile1.tellg(); //---------use previous one-----------//
  	readFile1.seekg(0, std::ios::beg);
  	
  	bool ans = true;
  	while(readFile1.read(reinterpret_cast<char*>(buffer.data()), DATAFILE_SECTOR_NUM)) {
    		for (int k = 0; k < key_len; k++) {
				GF256::Element result = calculate_mac(buffer,key[k],DATAFILE_SECTOR_NUM);
				writeFile1.write(reinterpret_cast<const char*>(&result), sizeof(GF256::Element));
				cout << hex << setw(2) << setfill('0') << static_cast<int>(result);
				if (key_len - k != 1) cout << "::"; 
				
				if (result != finalRes[k]) {					 	
				 	ans = false; // comparing Tau1 and Tau2 by hexa decimal charactors.
				 }
				
			}
			cout << endl;
  	}
    readFile1.close();
    writeFile1.close();
    return ans;   
}

GF256::Element calculate_mac(vector<GF256::Element> &block, GF256::Element key,int DATAFILE_SECTOR_NUM)
{
	//gf2256 intialization
	GF256 gf256(0x11B); //100011011 , you can also give input in HEXA form like : 283
	GF256::Element byte = key;
	GF256::Element mac = 0x00;
	
    	for (int i = 0; i < DATAFILE_SECTOR_NUM; i++) {
			GF256::Element a;
	               
	        a = gf256.Multiply(static_cast<GF256::Element>(block[i]), byte); // byte * alfa
	        mac = gf256.Add(a,mac); // a xor mac
	        byte = gf256.Multiply(byte,key);
    }
    return mac;
}

