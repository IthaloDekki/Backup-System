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
    std::vector<std::pair<std::string,int>> resultados;
    std::error_code ec;

    if (!fs::exists(fs::path(backupParm), ec)) {
        resultados.emplace_back(std::make_pair(std::string("Backup.parm"), 6)); // A6_IMPOSSIVEL
        return resultados;
    }

    return resultados;
}
