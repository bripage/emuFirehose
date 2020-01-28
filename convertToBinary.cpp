//
// Created by Brian Page on 2019-10-22.
//
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unistd.h>


int main(int argc, char **argv) {
    std::string matName(argv[1]);
    std::string binMatName(argv[2]);
    long file_count = std::atoi(argv[3]);
    long file_length = std::atoi(argv[4]);
    long last_file_length = std::atoi(argv[5]);
    long total_packets = std::atoi(argv[6]);

    long datums = 100;
    long address, val, flag;
    long* buffer;

    for (long iteration = 0; iteration < file_count; iteration++) {
        std::string matPart = matName + ".part" + std::to_string(iteration);

        if (iteration == file_count-1){
            file_length = last_file_length;
        }

        // Read in sparse matrix saved in Matrix Market Format
        std::ifstream infile(matPart.c_str());
        if (!infile) {
            std::cout << "FAILED TO OPEN FILE!" << std::endl;
            exit(1);
        }

        buffer = (long *) malloc(((datums) * 3) * sizeof(long));

        std::cout << "mat file opened" << std::endl;
        std::string line;
        int i = 0, j = 0;

        while (std::getline(infile, line)) {
            size_t pos = 0;
            std::string token;
            i = 0;
            //std::cout << "0" << std::endl;
            pos = line.find(',');
            if (pos != std::string::npos) {
                token = line.substr(0, pos);
                line.erase(0, pos + 1);
                address = ::atol(token.c_str());

                pos = line.find(',');
                if (pos != std::string::npos) {
                    token = line.substr(0, pos);
                    line.erase(0, pos + 1);
                    val = ::atol(token.c_str());
                    flag = ::atol(line.c_str());
                }

                //printf("%ld %ld %ld\n", tempRow, tempCol, tempData);
                buffer[j] = address;
                buffer[j + 1] = val;
                buffer[j + 2] = flag;
                j += 3;
            } else {
                // do nothing
            }
        }
        infile.close();

        std::string outFile = binMatName;
        //std::ofstream fout(outFile.c_str(), std::ios::out | std::ios::binary);
        FILE *fout;
        fout = fopen(binMatName.c_str(), "ab");
	    if (iteration == 0) fwrite(&total_packets, sizeof(long), 1, fout);

        //for (i = 0; i < (file_length*3); i++) {
        for (i = 0; i < (datums*3); i++) {
            fwrite(&buffer[i], sizeof(long), 1, fout);
        }
        fclose(fout);


        std::ifstream infile2(binMatName.c_str(), std::ios::in | std::ios::binary);
        if (!infile2) {
            std::cout << "FAILED TO OPEN FILE!" << std::endl;
            exit(1);
        }

        long temp1, temp2, temp3, temp4;
        infile2.read((char *) &temp1, sizeof(long));
        infile2.read((char *) &temp2, sizeof(long));
        infile2.read((char *) &temp3, sizeof(long));
        infile2.read((char *) &temp4, sizeof(long));

        printf("%ld \n%ld %ld %ld\n", temp1, temp2, temp3, temp4);

        infile2.close();

        free(buffer);
    }
    std::cout << "Done!" << std::endl;

    return 0;
}