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

#define PORT 8005
int SEED_BLOCK_NUMBER = 101;


GF256::Element calculate_mac(char block[], GF256::Element key,int M);
void Tag_gen(const char* filePath, vector<GF256::Element> &key,int M,int Rows);

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
	char buffer[1024];

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
		cout << buffer << endl;
		cout << endl << "--------------------------------------------------------------------------------------------------------" << endl;
		cout << endl;
		//sendto(server_fd, response_str.c_str(), strlen(response_str.c_str()), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
	}

	close(server_fd);
}


int main() {
	/*string file;
	cout << "Enter file name : ";
	cin >> file;
	const char* filePath = file.c_str();
	std::ifstream readFile(filePath, std::ios::binary);
	
	if (!readFile.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return 0;
    }
    readFile.seekg(0, std::ios::end);
    streampos fileSize = readFile.tellg();
    
    int M;
    cout << "Enter sector size : ";
    cin >> M;
    long long fs = static_cast<long long>(fileSize);
    int Rows = ceil(( fs / (M + 0.0))); 
    cout << "File Size = " << fs << " Total Number of Block = " << Rows << endl;
    
    readFile.close();
    vector<GF256::Element>key = {123,200,15}; // ------ alph key ---------------------------------------------------------------------------- //
	Tag_gen(filePath,key,M,Rows);
	*/
	Start_Server();
	return 0;

}


//LAB-ASSIGNMENT-2
GF256::Element calculate_mac(char block[], GF256::Element key,int M)
{
	//gf2256 intialization
	GF256 gf256(0x11B); //100011011 , you can also give input in HEXA form like : 283
	GF256::Element byte = key;
	//std::cout << byte << std::endl;
	GF256::Element mac = 0x00;
	
    	for (int i = 0; i < M; i++) {
	        //std::cout << "Read byte: " << static_cast<unsigned int>(static_cast<unsigned char>(byte)) << std::endl;
			GF256::Element a;
	               
	        a = gf256.Multiply(static_cast<GF256::Element>(block[i]), byte); // byte * alfa
	        mac = gf256.Add(a,mac); // a xor mac
	        byte = gf256.Multiply(byte,key);
    }
    return mac;
}

//LAB-ASSIGNMENT-3
void Tag_gen(const char* filePath, vector<GF256::Element> &key,int M,int Rows) {
	int key_len = key.size();
	
	char buffer[M];

    // Open the file in binary mode
    std::ifstream readFile(filePath, std::ios::binary);

    if (!readFile.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }
    
    std::ofstream writeFile("Tag.bin", std::ios::binary);
    if (!writeFile.is_open()) {
        std::perror("Error opening output file");
        return;
    }
    
    // Move the file pointer to the end of the file
    readFile.seekg(0, std::ios::end);
    
    // Get the current position of the file pointer, which represents the total number of bytes
    streampos fileSize = readFile.tellg();
  	readFile.seekg(0, std::ios::beg); // move beging of file
  	
  	
  	//cout << "rows " << Rows << endl;
  	while(readFile.read(buffer, M)) {
  		
    		for (int k = 0; k < key_len; k++) {
				GF256::Element result = calculate_mac(buffer,key[k],M);
				writeFile.write(reinterpret_cast<const char*>(&result), sizeof(GF256::Element));
				
				//cout << hex << setw(2) << setfill('0') << static_cast<int>(result);
				//if (key_len - k != 1) cout << "::";
			}
//			cout << endl;
			
			memset(buffer,0b00000000,sizeof(buffer));
  	}
  	
  	 if ((Rows * M) > fileSize) {
  		
  		for (int k = 0; k < key_len; k++) {
				GF256::Element result = calculate_mac(buffer,key[k],M);
				writeFile.write(reinterpret_cast<const char*>(&result), sizeof(GF256::Element));
				
				//cout << hex << setw(2) << setfill('0') << static_cast<int>(result);
				//if (key_len - k != 1) cout << "::";
			}
			//cout << endl;
  	}
  	
   
  	cout << "Tag File is generated..." << endl;
    readFile.close();
    writeFile.close();
    return;
}
