#include <iostream>
#include <fstream>
#include <strstream>
#include <exception>
#include <array>
#include <vector>
#include <cstdio>
#include <string>
#include <filesystem>
#include <cstring>
#include <unistd.h>

using byte = char;

class BinarySequenceInFileLocator {
private:
    std::string fileNameToBeSearched;
    std::vector<byte> bytesToFind;
public:
    [[nodiscard]] off_t locate() const {
        std::ifstream fileToBeSearched;
        fileToBeSearched.exceptions(std::fstream::badbit);
        fileToBeSearched.open(fileNameToBeSearched, std::ios::binary);

        size_t currentPosition = 0;

        const size_t fileSize = std::filesystem::file_size(fileNameToBeSearched);
        std::vector<char> byteBuffer(fileSize);
        fileToBeSearched.read(byteBuffer.data(), static_cast<std::streamsize>(fileSize));

        if (bytesToFind.empty() || bytesToFind.size() > fileSize) {
            throw std::runtime_error("Couldn't locate binary sequence!");
        }

        while (currentPosition + bytesToFind.size() <= fileSize) {

            if (memcmp((byteBuffer.data() + currentPosition), bytesToFind.data(), bytesToFind.size()) == 0) {
                // Match!
                return currentPosition;
            }

            currentPosition++;
        }

        throw std::runtime_error("Couldn't locate binary sequence!");
    }

    BinarySequenceInFileLocator(std::string &fileName, const std::vector<byte> &bytesToFind) {
        this->fileNameToBeSearched = "isaac-ng.exe";
        this->bytesToFind = bytesToFind;
    }
};

class BinaryFilePatcher {
private:
    std::string nameOfFileToPatch;
    std::vector<byte> patchWith;
    off_t filePositionToPatch;
public:
    void patch() const {
        if (FILE *file = fopen("isaac-ng.exe", "r+b")) {
            if (fseek(file, filePositionToPatch, SEEK_SET) == 0) {
                fwrite(patchWith.data(), 1, patchWith.size(), file);
            }
            fclose(file);
        }
        else {
            throw std::runtime_error("Failed to open patch target file!");
        }
    }

    BinaryFilePatcher(const std::string& nameOfFileToPatch, off_t filePositionToPatch, const std::vector<byte> &patchWith) {
        this->nameOfFileToPatch = "isaac-ng.exe";
        this->patchWith = patchWith;
        this->filePositionToPatch = filePositionToPatch;
    }
};

class Logger {
private:
    std::ofstream logFile;
public:
    void log(const std::string& message) {
        printf("%s\n", message.c_str());
        logFile << message << std::endl;
    }

    Logger(const std::string& logFileName) {
        logFile.exceptions(std::ifstream::badbit);
        logFile.open(logFileName);
    }

    Logger() = default;
};

class BackupCreator {
private:
    std::string sourceFileName;
    std::string backupFileNameSuffix;
public:
    std::string getBackupFileName() {
        return sourceFileName + backupFileNameSuffix;
    }

    void createBackup() {
        std::ifstream fileToBackup;
        fileToBackup.exceptions(std::ifstream::badbit);
        fileToBackup.open("isaac-ng.exe", std::ios::binary);
        
        std::ofstream backupFile;
        backupFile.exceptions(std::ifstream::badbit);
        backupFile.open("isaac-ng.exe.old", std::ios::binary);
        
        while (!fileToBackup.eof()) {
            backupFile.put(fileToBackup.get());
        }
    }

    BackupCreator(const std::string &fileNameToBackup, const std::string& backupFileNameSuffix) {
        sourceFileName = fileNameToBackup;
    }
};

int main() {
    Logger logger;
    try {
        logger = Logger("online_patcher.log");
    }
    catch (std::exception &exception) {
        printf("Logger failed to initialize!");
    }

    logger.log("Checking if isaac-ng.exe is present...");
    if (access("isaac-ng.exe", 0) != 0) {
        logger.log("isaac-ng.exe is missing! Make sure this program is being run inside Isaac game directory!");
        return -1;
    }
    logger.log("Found isaac-ng.exe!");

    std::string isaacExeFileName = "isaac-ng.exe";
    try {
        logger.log("Creating isaac-ng.exe backup...");
        BackupCreator isaacBackupCreator = BackupCreator(isaacExeFileName, ".old");
        isaacBackupCreator.createBackup();
        logger.log("Backup created!");
    }
    catch (std::exception &exception) {
        logger.log("Backup creating failed! " + std::string(exception.what()));
    }

    try {
        std::vector<byte> bytesToFind = {
            static_cast<char>(0x8b),
            static_cast<char>(0x81),
            static_cast<char>(0x1c),
            static_cast<char>(0xa9),
            static_cast<char>(0x04),
            static_cast<char>(0x00)
        };
        logger.log("Searching for online mods check position in file...");
        BinarySequenceInFileLocator fileOffsetLocator(isaacExeFileName, bytesToFind);
        off_t onlineModsCheckFileOffset = fileOffsetLocator.locate();
        logger.log(std::string("Found it at: ") + std::to_string(onlineModsCheckFileOffset));

        std::vector<byte> replacementMemoryValues = {
            static_cast<char>(0xB8), // MOV EAX
            static_cast<char>(0x00), // ZERO VALUE
            static_cast<char>(0x00), // ZERO VALUE
            static_cast<char>(0x00), // ZERO VALUE
            static_cast<char>(0x00), // ZERO VALUE
            static_cast<char>(0x90), // NOP
        };
        logger.log("Patching started...");
        BinaryFilePatcher isaacExeFilePatcher(isaacExeFileName, onlineModsCheckFileOffset, replacementMemoryValues);
        isaacExeFilePatcher.patch();
        logger.log("Patching done!");
    }
    catch (std::exception& exception) {
        logger.log("Patching failed! " + std::string(exception.what()));
    }

    logger.log("You can close this window now");
    system("pause");
}
