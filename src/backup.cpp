// Copyright 2025 Íthalo Júnio Medeiros de Oliveira Nóbrega

#include "../include/backup.hpp"
#include <filesystem>  // NOLINT(build/c++17)
#include <vector>
#include <string>
#include <fstream>
#include <system_error>
#include <cassert>
#include <utility>

namespace fs = std::filesystem;

/***************************************************************************
* Função: executar_backup
* Descrição:
*   Realiza o processo de backup ou restauração baseado no arquivo
*   "Backup.parm", copiando os arquivos conforme as condições da
*   tabela de decisão. Pode operar no modo backup (HD → Pen) ou
*   restauração (Pen → HD).
*
* Parâmetros:
*   backupParm - caminho do arquivo de parâmetros (Backup.parm)
*   dirHD - diretório simulando o HD local
*   dirPen - diretório simulando o Pen-drive
*   dirDestino - diretório destino de cópia (onde os arquivos serão salvos)
*   backupSolicitado - true se for backup (HD → Pen), false se for restauração
*
* Valor retornado:
*   Vetor de pares <nome do arquivo, código da ação> indicando o resultado
*   de cada operação realizada (ex: cópia, erro, nada a fazer, etc).
*
* Assertivas de entrada:
*   backupParm != ""
*   dirHD != ""
*   dirDestino != ""
*
* Assertivas de saída:
*   O vetor retornado não é nulo
*   Cada par no vetor contém o nome de um arquivo existente no Backup.parm
*   e o código correspondente à ação executada.
*
* Condições de erro tratadas:
*   - Arquivo Backup.parm inexistente → ação A6_IMPOSSIVEL
*   - Arquivos inexistentes em ambas as origens → ação A6_IMPOSSIVEL
*   - Conflito de datas entre HD e Pen → ação A5_ERRO
*
* Valor retornado:
*   Veja os códigos definidos em enum Acao:
*       A1_COPIAR_HD_PEN
*       A2_COPIAR_PEN_HD
*       A4_NADA
*       A5_ERRO
*       A6_IMPOSSIVEL
***************************************************************************/


std::vector<std::pair<std::string, int>> executar_backup(
    const std::string &backupParm,
    const std::string &dirHD,
    const std::string &dirPen,
    const std::string &dirDestino,
    bool backupSolicitado) {
    assert(!backupParm.empty());
    assert(!dirHD.empty());
    assert(!dirDestino.empty());

    std::vector<std::pair<std::string, int>> resultados;
    std::error_code ec;

    if (!fs::exists(fs::path(backupParm), ec)) {
        resultados.emplace_back(std::make_pair("Backup.parm",
            static_cast<int>(Acao::A6_IMPOSSIVEL)));
        return resultados;
    }

    std::ifstream parmFile(backupParm);  // le o arquivo .parm
    std::string nomeArquivo;

    while (std::getline(parmFile, nomeArquivo)) {
        if (nomeArquivo.empty())
            continue;

        fs::path caminhoHD = fs::path(dirHD) / nomeArquivo;
        fs::path caminhoPen = fs::path(dirPen) / nomeArquivo;
        fs::path caminhoDestino = fs::path(dirDestino) / nomeArquivo;

        bool existeHD = fs::exists(caminhoHD);
        bool existePen = fs::exists(caminhoPen);

        if (!existeHD && !existePen) {
            resultados.emplace_back(nomeArquivo, A6_IMPOSSIVEL);
            continue;
        }

        auto dataHD = existeHD ? fs::last_write_time(caminhoHD) :
        fs::file_time_type::min();
        auto dataPen = existePen ? fs::last_write_time(caminhoPen) :
        fs::file_time_type::min();

        if (backupSolicitado) {
            if (existeHD && !existePen) {
                fs::copy_file(caminhoHD, caminhoDestino,
                fs::copy_options::overwrite_existing, ec);
                resultados.emplace_back(nomeArquivo, static_cast<int>
                (Acao::A1_COPIAR_HD_PEN));
            } else if (existeHD && existePen && dataHD > dataPen) {
                fs::copy_file(caminhoHD, caminhoDestino,
                    fs::copy_options::overwrite_existing, ec);
                resultados.emplace_back(nomeArquivo, static_cast<int>
                (Acao::A1_COPIAR_HD_PEN));
            } else if (existeHD && existePen && dataPen > dataHD) {
                resultados.emplace_back(nomeArquivo, static_cast<int>
                (Acao::A5_ERRO));
            } else {
                resultados.emplace_back(nomeArquivo, static_cast<int>
                (Acao::A4_NADA));
            }
        } else {  // modo restauração
            if (!existeHD && !existePen) {
                resultados.emplace_back(nomeArquivo, static_cast<int>
                (Acao::A6_IMPOSSIVEL));
            } else if (!existeHD && existePen) {
                fs::copy_file(caminhoPen, caminhoDestino,
                fs::copy_options::overwrite_existing, ec);
                resultados.emplace_back(nomeArquivo, static_cast<int>
                (Acao::A2_COPIAR_PEN_HD));
            } else if (existeHD && existePen && dataPen > dataHD) {
                fs::copy_file(caminhoPen, caminhoDestino,
                fs::copy_options::overwrite_existing, ec);
                resultados.emplace_back(nomeArquivo, static_cast<int>
                (Acao::A2_COPIAR_PEN_HD));
            } else if (existeHD && existePen && dataPen == dataHD) {
                resultados.emplace_back(nomeArquivo, static_cast<int>
                (Acao::A4_NADA));
            } else {
                resultados.emplace_back(nomeArquivo, static_cast<int>
                (Acao::A4_NADA));
            }
        }
    }
    return resultados;
}
