//
// Created by bpage1 on 2/4/20.
//

//
// Created by Brian Page on 2019-10-22.
//
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unistd.h>


int main(int argc, char **argv) {
    std::string binName(argv[1]);

    long addr, val, flag;

    std::ifstream infile2(binName.c_str(), std::ios::in | std::ios::binary);
    if (!infile2) {
        std::cout << "FAILED TO OPEN FILE!" << std::endl;
        exit(1);
    }

    long datums;
    infile2.read((char *) &datums, sizeof(long));
    printf("Datums: %ld\n", datums);

    for (long i = 0; i < datums; i++) {
        infile2.read((char *) &addr, sizeof(long));
        infile2.read((char *) &val, sizeof(long));
        infile2.read((char *) &flag, sizeof(long));
        printf("%ld, %ld, %ld\n", addr, val, flag);

        if (addr < 1000000000 || addr > 2000000000){
            printf("ERROR: Address out of range (%ld)\n", addr);
        }
        if (val != 0 && val != 1){
            printf("ERROR: Val out of range (%ld)\n", val);
        }
        if (flag != 0 && flag != 1){
            printf("ERROR: Flag out of range (%ld)\n", flag);
        }
    }
    infile2.close();


    std::cout << "Done!" << std::endl;
    return 0;
}