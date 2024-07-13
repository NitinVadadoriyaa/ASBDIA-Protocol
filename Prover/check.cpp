#include <iostream>
#include <fstream>

int countSetBits(unsigned char byte) {
    int count = 0;
    while (byte) {
        count += byte & 1;
        byte >>= 1;
    }
    return count;
}

int main() {
    std::ifstream file1("datafile.bin", std::ios::binary);
    std::ifstream file2("datafile_o.bin", std::ios::binary);

    if (!file1 || !file2) {
        std::cerr << "Error opening input files.\n";
        return 1;
    }

    std::ofstream outputFile("resultXOR.bin", std::ios::binary);

    if (!outputFile) {
        std::cerr << "Error opening output file.\n";
        return 1;
    }

    int countOnes = 0;
    unsigned char byte1, byte2, resultByte;

    while (file1.read(reinterpret_cast<char*>(&byte1), sizeof(byte1)) &&
           file2.read(reinterpret_cast<char*>(&byte2), sizeof(byte2))) {
        resultByte = byte1 ^ byte2;
        countOnes += countSetBits(resultByte);
        outputFile.write(reinterpret_cast<const char*>(&resultByte), sizeof(resultByte));
    }

    file1.close();
    file2.close();
    outputFile.close();

    std::cout << "Number of ones in the XORed result: " << countOnes << std::endl;

    return 0;
}
