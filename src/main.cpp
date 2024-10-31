#include "../include/Logs.h"
#include "../include/Utils.h"
#include "../include/json.hpp"

#include <exception>
#include <spine/Animation.h>
#include <spine/Atlas.h>
#include <spine/SkeletonBinary.h>
#include <spine/SkeletonData.h>
#include <spine/Vector.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

struct SpineFiles {
    std::string atlasPath;
    std::string skelPath;
    std::string parentFolder;
};

nlohmann::json config;
std::vector<std::string> validAnimationNames;
Logs* logs = new Logs();

spine::SpineExtension* spine::getDefaultExtension() {
    return new spine::DefaultSpineExtension();
}

nlohmann::json LoadConfig() {
    try {
        std::ifstream f("spine_reader_config.json");
        nlohmann::json data = nlohmann::json::parse(f);
        return data;
    } catch (const std::exception&) {
        logs->error("Failed to load config file");
        return nullptr;
    }
}

spine::SkeletonData* LoadSpine(const char* atlasPath, const char* skelPath) {
    spine::Atlas* atlas = new spine::Atlas(atlasPath, nullptr, false);
    spine::SkeletonBinary binary(atlas);
    spine::SkeletonData* data = binary.readSkeletonDataFile(skelPath);
    if (nullptr == data) {
        logs->error("Failed to load Skeleton data from path: " + std::string(skelPath));
        logs->error(binary.getError().buffer());
        delete atlas;
        return nullptr;
    }
    delete atlas;
    logs->log("Loaded Skeleton data from path: " + std::string(skelPath));
    return data;
}

std::vector<std::string> GetInvalidAnimationNames(spine::SkeletonData* data) {
    std::vector<std::string> res;
    spine::Vector<spine::Animation*> animations = data->getAnimations();
    for (int i = 0; i < animations.size(); i++) {
        spine::Animation* anim = animations[i];
        std::string name = anim->getName().buffer();
        if (std::find(validAnimationNames.begin(), validAnimationNames.end(), name) == validAnimationNames.end()) {
            res.push_back(name);
        }
    }
    return res;
}

void findFiles(const fs::path& dir, std::vector<SpineFiles>& allSpineFiles) {
    logs->log("Searching for spine files in folder: " + dir.string());
    SpineFiles spineFiles;

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.is_directory()) {
            findFiles(entry.path(), allSpineFiles);
        } else {
            if (entry.path().extension() == ".txt" && entry.path().string().find(".atlas.txt") != std::string::npos) {
                spineFiles.atlasPath = entry.path().string();
                spineFiles.parentFolder = entry.path().parent_path().string();
            } else if (entry.path().extension() == ".skel") {
                spineFiles.skelPath = entry.path().string();
            }
        }
    }

    if (!spineFiles.atlasPath.empty() && !spineFiles.skelPath.empty()) {
        allSpineFiles.push_back(spineFiles);
    }
}

bool WriteInvalidAnimationsToFile(const std::vector<SpineFiles>& allSpineFiles) {
    std::ostringstream filename;
    filename << "invalid_animations_" << Utils::get_current_time_str() << ".txt";
    std::ofstream outFile(filename.str());
    if (!outFile) {
        logs->error("Failed to create invalid animations output file");
        return false;
    }
    for (const auto& spineFile : allSpineFiles) {
        spine::SkeletonData* data = LoadSpine(spineFile.atlasPath.c_str(), spineFile.skelPath.c_str());
        if (data != nullptr) {
            std::vector<std::string> res = GetInvalidAnimationNames(data);
            if (!res.empty()) {
                outFile << "Folder name: " << spineFile.parentFolder << std::endl;
                outFile << "Invalid animation names:" << std::endl;
                for (const auto& name : res) {
                    outFile << name << std::endl;
                }
                outFile << std::endl;
            }
            delete data;
        } else {
            outFile << "Failed to load Skeleton data from path: " << spineFile.skelPath << std::endl;
            outFile << std::endl;
        }
    }
    outFile.close();
    return true;
}

bool WriteDetailsToFile(const std::vector<SpineFiles>& allSpineFiles) {
    std::ostringstream filename;
    filename << "details_" << Utils::get_current_time_str() << ".txt";
    std::ofstream outFile(filename.str());
    if (!outFile) {
        logs->error("Failed to create details output file");
        return false;
    }
    for (const auto& spineFile : allSpineFiles) {
        spine::SkeletonData* data = LoadSpine(spineFile.atlasPath.c_str(), spineFile.skelPath.c_str());
        if (data != nullptr) {
            spine::Vector<spine::Animation*> _animations = data->getAnimations();
            outFile << "Folder name: " << spineFile.parentFolder << std::endl;
            outFile << "Animation names:" << std::endl;
            for (int i = 0; i < _animations.size(); i++) {
                spine::Animation* anim = _animations[i];
                spine::String name = anim->getName();
                outFile << name.buffer() << std::endl;
            }
            outFile << std::endl;
            delete data;
        } else {
            outFile << "Failed to load Skeleton data from path: " << spineFile.skelPath << std::endl;
            outFile << std::endl;
        }
    }
    outFile.close();
    return true;
}

int main(int argc, char** argv) {
    nlohmann::json config = LoadConfig();
    if (nullptr == config) {
        system("pause");
        return -1;
    }

    if (!config.contains("names") || !config["names"].is_array()) {
        std::cerr << "\"names\" key is missing or not an array in the config file." << std::endl;
        system("pause");
        return -1;
    }

    if (config.contains("silent") && config["silent"].get<bool>())
        logs->silent = config["silent"];
    else
        logs->silent = false;

    try {
        validAnimationNames = config["names"].get<std::vector<std::string>>();
    } catch (const nlohmann::json::exception& e) {
        logs->error("Error parsing \"names\": " + std::string(e.what()));
        system("pause");
        return -1;
    }

    std::vector<SpineFiles> allSpineFiles;
    try {
        std::string rootPath = config["rootPath"];
        fs::path paths = fs::path(rootPath);
        for (const auto& entry : fs::directory_iterator(paths)) {
            if (entry.is_directory()) {
                std::string atlasPath, skelPath, parentFolder;
                findFiles(entry.path(), allSpineFiles);
            }
        }
        bool output = WriteInvalidAnimationsToFile(allSpineFiles);
        if (!output) {
            logs->error("Failed to write invalid animations output file");
            system("pause");
            return -1;
        }
        if (config.contains("details") && config["details"].get<bool>()) {
            bool detailOutput = WriteDetailsToFile(allSpineFiles);
            if (!detailOutput) {
                logs->error("Failed to write details output file");
                system("pause");
                return -1;
            }
        }
    } catch (const fs::filesystem_error& e) {
        logs->error("Filesystem error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        logs->error("Error: " + std::string(e.what()));
    }

    if (logs->silent)
        std::cout << "[LOG " + Utils::get_current_time_str() + "] " << "finished" << std::endl;
    else
        logs->log("finished");

    if (config.contains("checkLogs") && config["checkLogs"].get<bool>()) {
        logs->save();
    }
    delete logs;

    if (config.contains("pause") && config["pause"].get<bool>()) system("pause");
    return 0;
}
