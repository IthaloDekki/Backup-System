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

    
    // le o arquivo .parm
    std::ifstream parmFile(backupParm);
    std::string nomeArquivo;

    while (std::getline(parmFile, nomeArquivo)) {
        if (nomeArquivo.empty())
            continue;

        fs::path caminhoHD = fs::path(dirHD) / nomeArquivo;
        fs::path caminhoPen = fs::path(dirPen) / nomeArquivo;
        fs::path caminhoDestino = fs::path(dirDestino) / nomeArquivo;

        
        if (backupSolicitado) {
            bool existeHD = fs::exists(caminhoHD);
            bool existePen = fs::exists(caminhoPen);

            if (existeHD && !existePen) {
                // HD → destino
                fs::copy_file(caminhoHD, caminhoDestino, fs::copy_options::overwrite_existing, ec);
                resultados.emplace_back(nomeArquivo,
                    static_cast<int>(Acao::A1_COPIAR_HD_PEN));
            } else {
                
                resultados.emplace_back(nomeArquivo,
                    static_cast<int>(Acao::A4_NADA));
            }
        }
        else {
            bool existeHD = fs::exists(caminhoHD);
            bool existePen = fs::exists(caminhoPen);

            if (!existeHD && existePen) {
                // copia arquivo Pen → destino
                fs::copy_file(caminhoPen, caminhoDestino, fs::copy_options::overwrite_existing, ec);
                resultados.emplace_back(nomeArquivo,
                    static_cast<int>(Acao::A2_COPIAR_PEN_HD));
            } else {
                resultados.emplace_back(nomeArquivo,
                    static_cast<int>(Acao::A4_NADA));
            }
        }
        
    }


    return resultados;
}