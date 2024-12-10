#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#define BUF_SIZE 1024

void usageError(const std::string& programName) {
    std::cerr << "Usage: " << programName << " <source-file> <destination-file>" << std::endl;
    exit(EXIT_FAILURE);
}

void errorExit(const std::string& message, const std::string& fileName) {
    std::cerr << "Error: " << message << " " << fileName << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    if (argc != 3 || std::strcmp(argv[1], "--help") == 0) {
        usageError(argv[0]);
    }

    int inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1) {
        errorExit("opening file", argv[1]);
    }

    int openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-
    int outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1) {
        close(inputFd); 
        errorExit("opening file", argv[2]);
    }

    char buf[BUF_SIZE];
    ssize_t numRead;
    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        if (write(outputFd, buf, numRead) != numRead) {
            close(inputFd);
            close(outputFd);
            errorExit("writing to file", argv[2]);
        }
    }

    if (numRead == -1) {
        close(inputFd);
        close(outputFd);
        errorExit("reading file", argv[1]);
    }

    if (close(inputFd) == -1) {
        errorExit("closing file", argv[1]);
    }

    if (close(outputFd) == -1) {
        errorExit("closing file", argv[2]);
    }

    std::cout << "File copied successfully from " << argv[1] << " to " << argv[2] << std::endl;

    return EXIT_SUCCESS;
}
