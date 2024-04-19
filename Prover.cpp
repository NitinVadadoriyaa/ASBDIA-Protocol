#include <iostream>
#include <thread>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctime>
#include <bits/stdc++.h>
#include <cryptopp/gf256.h>

using namespace std;
using namespace CryptoPP;
int DATAFILE_TOTAL_ROWS = 11535; //original total DATAFILE_TOTAL_ROWS/blocks in datafile
int DATAFILE_SECTOR_NUM =  100000; // sector size in each block
long long SEED_BLOCK_NUMBER = 101;

#define PORT 8011

void Proof_gen(char* filePath,int r,char* fileName,int seed,vector<int> &blc);
void sendFile(const char* fileName,int &server_fd,const sockaddr_in& client_addr);
void DisplayTime()
{
	std::time_t currentTime = std::time(nullptr);

    // Convert the current time to a local time structure
    std::tm* localTime = std::localtime(&currentTime);

    // Print the current local time
    std::cout << "Current local time: "
              << localTime->tm_year + 1900 << '-'
              << localTime->tm_mon + 1 << '-'
              << localTime->tm_mday << ' '
              << localTime->tm_hour << ':'
              << localTime->tm_min << ':'
              << localTime->tm_sec << std::endl;
}

void Start_Server() {
	int server_fd, addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in server_addr, client_addr;
	char buffer[10];

	if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	memset(&client_addr, 0, sizeof(client_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	cout << "Server is running... " << endl;
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	while (1) {
		memset(buffer,0,sizeof(buffer));
		ssize_t recv_len = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
		if (recv_len == -1) {

			perror("recvfrom");
			exit(EXIT_FAILURE);
		}
		DisplayTime();
		
		cout << "message coming from IP: " << inet_ntoa(client_addr.sin_addr) << " Port: " << ntohs(client_addr.sin_port) << std::endl;

		long long seed =  stoi(string(buffer));
		SEED_BLOCK_NUMBER = seed + 1;
		
		cout << "Reciev seed " << seed << endl;
		memset(buffer,0,sizeof(buffer));
		
		recv_len = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
		if (recv_len == -1) {
			perror("recvfrom");
			exit(EXIT_FAILURE);
		}
		int block_count = stoi(string(buffer));
		cout << "Reciev block_count " << block_count << endl;
		
		srand(SEED_BLOCK_NUMBER); // ------------block ind generator -------------//
		
		unordered_map<int,bool>hash;
  		vector<int>block_ind(block_count);
  		for(int i = 0; i < block_count; i++) {
  			while(true) { 		
	 	 		int r = rand() % DATAFILE_TOTAL_ROWS;
	 	 		if (hash.find(r) == hash.end()) {
	 	 			hash[r] = true;
  					block_ind[i] = r;
  					break;
	 	 		}
  			}
  		}	
		sort(block_ind.begin(),block_ind.end());
		
		Proof_gen("datafile.bin",DATAFILE_SECTOR_NUM,"Mue.bin",seed,block_ind);
		sendFile("Mue.bin",server_fd,client_addr);

		cout << endl;
		cout << "---------------------------------------------------------------------------------------------------------------" << endl;
		cout << endl;
	}

	close(server_fd);
}


int main() {
	thread server_thread(Start_Server); // thread is treager
	server_thread.join();

	return 0;
}

void Proof_gen(char* filePath,int r,char* fileName,int seed,vector<int> &block_ind) {
//r --> no of sector in one block
// Open the file in binary mode
cout << "Please Wait, Matric Multiplication is going....." << endl;
    std::ifstream readFile(filePath, std::ios::binary);
    if (!readFile.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }
    std::ofstream writeFile(fileName, std::ios::binary);
    if (!writeFile.is_open()) {
        std::perror("Error opening output file");
        return;
    }

     // Move the file pointer to the end of the file
    readFile.seekg(0, std::ios::end);

    // Get the current position of the file pointer, which represents the total number of bytes
    streampos fileSize = readFile.tellg();
    readFile.seekg(0, std::ios::beg); // move beging of file

  	vector<GF256::Element> finalRes(r,0);
  	vector<GF256::Element> buffer(r,0);
	GF256 gf256(0x11B); //100011011
	srand(seed); // -----------------vector value may REPETED......... -----------------------------------
	
	for (int b = 0; b <  block_ind.size(); b++) {
  		int val = rand() % 256;
  		readFile.seekg(block_ind[b] * DATAFILE_SECTOR_NUM, std::ios::beg);
		readFile.read(reinterpret_cast<char*>(buffer.data()), r);
  		for (int i = 0; i < r; i++) {
  			GF256::Element mul = gf256.Multiply(buffer[i], static_cast<GF256::Element>(val));
  			finalRes[i] = gf256.Add(mul,finalRes[i]);
  		}
		buffer = vector<GF256::Element> (r,0);
		
	}
	
  	

    for (int i = 0; i < r; i++) {
    	writeFile.write(reinterpret_cast<const char*>(&finalRes[i]), sizeof(GF256::Element)); // write on file
//    	cout << hex << setw(2) << setfill('0') << static_cast<int>(finalRes[i]);
//		if (r - i != 1) cout << "::"; 
    }
    readFile.close();
    writeFile.close();
	return;
}

void sendFile(const char* fileName,int &server_fd,const sockaddr_in& client_addr) {
cout << "Please wait, Mue file sending is going...." << endl;
	ifstream  file(fileName,std::ios::binary);

	if(!file.is_open()) {
		cerr << "Unable to open file: " << fileName << std::endl;
        	return;
   	 }
	 file.seekg(0, std::ios::end);
	 streampos fileSize = file.tellg();
	 file.seekg(0, std::ios::beg);
	if (static_cast<int>(fileSize) > 1024) {
		//cout << "file size " << static_cast<int>(fileSize) << endl;
		size_t numFragments = static_cast<int>(fileSize) / 1024;
		size_t lastFragmentSize = static_cast<int>(fileSize) % 1024;
		char* buffer = new char[1024];
		//cout << numFragments << endl;
		//cout << lastFragmentSize << endl;

		for (size_t i = 0; i < numFragments; ++i) {
   			file.read(buffer, 1024);

		        ssize_t sentBytes = sendto(server_fd, buffer, 1024, 0,
                                   (struct sockaddr*)&client_addr, sizeof(client_addr));
		        if (sentBytes == -1) {
		            std::cerr << "Error sending file fragment: " << strerror(errno) << std::endl;
		            delete[] buffer;
		            file.close();
		            return;
			}
	    	}
		if (lastFragmentSize > 0) {
		        file.read(buffer, lastFragmentSize);
			ssize_t sentBytes = sendto(server_fd, buffer, lastFragmentSize, 0,
                                   (struct sockaddr*)&client_addr, sizeof(client_addr));
			 if (sentBytes == -1) {
		            std::cerr << "Error sending last file fragment: " << strerror(errno) << std::endl;
            		    delete[] buffer;
		            file.close();
		            return;
		        }
		}
		ssize_t sentBytes = sendto(server_fd, buffer, 0, 0,
                                   (struct sockaddr*)&client_addr, sizeof(client_addr));
		if (sentBytes == -1) {
                            std::cerr << "Error sending last file fragment: " << strerror(errno) << std::endl;
                            delete[] buffer;
                            file.close();
                            return;
                        }

	} else {
		char buffer[static_cast<int>(fileSize)];
        	file.read(buffer,static_cast<int>(fileSize));

        	// Send ...
	        ssize_t sentBytes = sendto(server_fd, buffer, sizeof(buffer), 0,
                                   (struct sockaddr*)&client_addr, sizeof(client_addr));

        	if (sentBytes == -1) {
	            std::cerr << "Error sending file fragment: " << strerror(errno) << std::endl;
        	    delete[] buffer;  // Free the allocated memory
		    file.close();
        	    return;
	       	}
		
		sentBytes = sendto(server_fd, buffer, 0, 0,
                       (struct sockaddr*)&client_addr, sizeof(client_addr)); // buffer size == 0 , for stopping loop on Challanger side....
	}
	file.close();
	cout << fileName << " send successfully " << endl;
	return;
}
