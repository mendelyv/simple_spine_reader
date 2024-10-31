#include "../include/Logs.h"
#include "../include/Utils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

void Logs::print(const std::string& msg) {
    if (!this->silent) {
        std::cout << msg << std::endl;
    }
}

void Logs::log(std::string msg) {
    msg.insert(0, "[LOG " + Utils::get_current_time_str() + "] ");
    this->datas.push_back(std::move(msg));
    this->print(this->datas.back());
}

void Logs::warn(std::string msg) {
    msg.insert(0, "[WARN " + Utils::get_current_time_str() + "] ");
    this->datas.push_back(std::move(msg));
    this->print(this->datas.back());
}

void Logs::error(std::string msg) {
    msg.insert(0, "[ERROR " + Utils::get_current_time_str() + "] ");
    this->datas.push_back(std::move(msg));
    this->print(this->datas.back());
}

bool Logs::save() {
    std::ostringstream filename;
    filename << "logs_" << Utils::get_current_time_str() << ".txt";
    std::ofstream outFile(filename.str());
    if (!outFile) {
        this->error("Failed to create logs output file");
        return false;
    }
    if (!this->datas.empty()) {
        for (const std::string& data : this->datas) {
            outFile << data << std::endl;
        }
    }
    outFile.close();
    if (!outFile) {
        this->error("Failed to write to logs output file");
        return false;
    }
    return true;
}
