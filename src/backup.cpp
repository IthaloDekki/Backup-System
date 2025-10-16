#include "../include/backup.hpp"
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <system_error>

namespace fs = std::filesystem;

std::vector<std::pair<std::string,int>> executar_backup(
    const std::string &backupParm,
    const std::string &dirHD,
    const std::string &dirPen,
    const std::string &dirDestino,
    bool backupSolicitado)
{
    return std::vector<std::pair<std::string,int>>();
}
