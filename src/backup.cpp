#include "../include/backup.hpp"
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <system_error>
#include <cassert>

namespace fs = std::filesystem;

/***************************************************************************
* Função: executar_backup
* Descrição:
*   Realiza o processo de backup ou restauração baseado no arquivo Backup.parm
* Parâmetros:
*   backupParm - caminho do arquivo de parâmetros (Backup.parm)
*   dirHD - diretório simulando o HD
*   dirPen - diretório simulando o Pen-drive
*   dirDestino - diretório destino de cópia
*   backupSolicitado - true se for backup (HD→Pen), false se restauração (Pen→HD)
* Valor retornado:
*   Vetor com pares (nome do arquivo, código da ação)
* Assertivas de entrada:
*   backupParm != ""
*   dirHD != ""
*   dirDestino != ""
***************************************************************************/

std::vector<std::pair<std::string,int>> executar_backup(
    const std::string &backupParm,
    const std::string &dirHD,
    const std::string &dirPen,
    const std::string &dirDestino,
    bool backupSolicitado)
{
    assert(!backupParm.empty());
    assert(!dirHD.empty());
    assert(!dirDestino.empty());

    std::vector<std::pair<std::string,int>> resultados;
    std::error_code ec;

    if (!fs::exists(fs::path(backupParm), ec)) {
        resultados.emplace_back(std::make_pair("Backup.parm",
            static_cast<int>(Acao::A6_IMPOSSIVEL)));
        return resultados;
    }

    return resultados;
}