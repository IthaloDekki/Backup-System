// Copyright 2025 Íthalo Júnio Medeiros de Oliveira Nóbrega

#define CATCH_CONFIG_MAIN

// C++ system headers
#include <cassert>
#include <filesystem>  // NOLINT(build/c++17)
#include <fstream>
#include <string>
#include <system_error>
#include <vector>

// Other headers
#include "../src/catch_amalgamated.hpp"

namespace fs = std::filesystem;

static constexpr int A6 = 6;

static std::vector<std::pair<std::string, int>> run_proc(
    const fs::path &parm,
    const fs::path &hd,
    const fs::path &pen,
    const fs::path &dest,
    bool backupSolicitado);

TEST_CASE("Caso 1 - Sem Backup.parm => Impossivel (A6)", "[C1]") {
    fs::path base = fs::path("tests") / "tmp_case_1";
    fs::remove_all(base);
    fs::create_directories(base / "hd");
    fs::create_directories(base / "pen");
    fs::create_directories("backup-destino");

    fs::path parm = base / "Backup.parm";

    auto res = run_proc(parm, base / "hd", base / "pen",
    "backup-destino", true);

    bool found = false;
    for (auto &p : res) if (p.second == A6) found = true;
    REQUIRE(found == true);
}

TEST_CASE("Caso 2 arquivo só no HD => Copiar HD->Pen",
    "[C2]") {
    namespace fs = std::filesystem;

    fs::path base = fs::path("tests") / "tmp_case_2";
    fs::remove_all(base);
    fs::create_directories(base / "hd");
    fs::create_directories(base / "pen");
    fs::create_directories("backup-destino");

    // Cria o arquivo Backup.parm com um nome de arquivo a ser copiado
    fs::path parm = base / "Backup.parm";
    std::ofstream(parm) << "documento.txt" << std::endl;

    // Cria o arquivo no HD (origem)
    fs::path arquivoHD = base / "hd" / "documento.txt";
    std::ofstream(arquivoHD) << "conteudo do arquivo HD";

    // Executa o backup
    auto res = run_proc(parm, base / "hd", base / "pen",
    "backup-destino", true);

    bool found = false;
    for (auto &p : res)
        if (p.first == "documento.txt" && p.second == 1)  // A1 = Copiar HD→Pen
            found = true;

    // O arquivo deve ter sido copiado para o destino (pen)
    REQUIRE(found == true);
    REQUIRE(fs::exists("backup-destino/documento.txt"));
}

TEST_CASE("Caso 3 arquivo no Pen e não no HD => Copiar Pen->HD",
    "[C3]") {
    namespace fs = std::filesystem;

    fs::path base = fs::path("tests") / "tmp_case_3";
    fs::remove_all(base);
    fs::create_directories(base / "hd");
    fs::create_directories(base / "pen");
    fs::create_directories("backup-destino");

    // Cria Backup.parm com o nome do arquivo
    fs::path parm = base / "Backup.parm";
    std::ofstream(parm) << "relatorio.txt" << std::endl;

    // Cria arquivo apenas no Pen (origem da restauração)
    fs::path arquivoPen = base / "pen" / "relatorio.txt";
    std::ofstream(arquivoPen) << "conteudo do pen";

    // Executa restauração (backupSolicitado = false)
    auto res = run_proc(parm, base / "hd", base / "pen",
    "backup-destino", false);

    bool found = false;
    for (auto &p : res)
        if (p.first == "relatorio.txt" && p.second == 2)  // A2 = Copiar Pen->HD
            found = true;

    // Verifica se o arquivo foi copiado para o destino
    REQUIRE(found == true);
    REQUIRE(fs::exists("backup-destino/relatorio.txt"));
}

TEST_CASE("Caso 4 arquivos com mesma data => Nada a fazer",
    "[C4]") {
    namespace fs = std::filesystem;

    fs::path base = fs::path("tests") / "tmp_case_4";
    fs::remove_all(base);
    fs::create_directories(base / "hd");
    fs::create_directories(base / "pen");

    // DESTINO isolado por caso
    fs::path destino = base / "backup-destino";
    fs::remove_all(destino);
    fs::create_directories(destino);

    fs::path parm = base / "Backup.parm";
    std::ofstream(parm) << "relatorio.txt" << std::endl;

    // Cria arquivo no HD e no Pen
    fs::path arquivoHD = base / "hd" / "relatorio.txt";
    fs::path arquivoPen = base / "pen" / "relatorio.txt";
    std::ofstream(arquivoHD) << "dados iguais";
    std::ofstream(arquivoPen) << "dados iguais";

    // ajusta datas iguais
    auto agora = fs::file_time_type::clock::now();
    fs::last_write_time(arquivoHD, agora);
    fs::last_write_time(arquivoPen, agora);

    // executa o backup passando o destino isolado
    auto res = run_proc(parm, base / "hd", base / "pen", destino, true);

    bool found = false;
    for (auto &p : res)
        if (p.first == "relatorio.txt" && p.second == 4)  // A4 = Nada
            found = true;

    REQUIRE(found == true);

    // O arquivo destino nao deve existir
    REQUIRE(!fs::exists(destino / "relatorio.txt"));
}

TEST_CASE("Caso 5 arquivo do HD mais novo => Copiar HD->Pen",
    "[C5]") {
    namespace fs = std::filesystem;

    fs::path base = fs::path("tests") / "tmp_case_5";
    fs::remove_all(base);
    fs::create_directories(base / "hd");
    fs::create_directories(base / "pen");
    fs::path destino = base / "backup-destino";
    fs::create_directories(destino);

    // Cria Backup.parm
    fs::path parm = base / "Backup.parm";
    std::ofstream(parm) << "dados.txt" << std::endl;

    // Cria arquivo em ambos os lugares
    fs::path arquivoHD = base / "hd" / "dados.txt";
    fs::path arquivoPen = base / "pen" / "dados.txt";
    std::ofstream(arquivoHD) << "conteudo atualizado no HD";
    std::ofstream(arquivoPen) << "conteudo antigo no Pen";

    // Ajusta as datas: HD mais novo
    auto agora = fs::file_time_type::clock::now();
    auto antes = agora - std::chrono::hours(1);
    fs::last_write_time(arquivoPen, antes);
    fs::last_write_time(arquivoHD, agora);

    // Executa o backup
    auto res = run_proc(parm, base / "hd", base / "pen", destino, true);

    bool found = false;
    for (auto &p : res)
        if (p.first == "dados.txt" && p.second == 1)  // A1 = Copiar HD→Pen
            found = true;

    REQUIRE(found == true);

    // O arquivo destino (Pen simulado) deve
    // existir e conter o conteúdo atualizado
    REQUIRE(fs::exists(destino
    / "dados.txt"));
}

TEST_CASE("Caso 6 Backup solicitado, Pen mais novo que HD => Erro",
    "[C6]") {
    namespace fs = std::filesystem;

    fs::path base = fs::path("tests") / "tmp_case_6";
    fs::remove_all(base);
    fs::create_directories(base / "hd");
    fs::create_directories(base / "pen");
    fs::path destino = base / "backup-destino";
    fs::create_directories(destino);

    // Cria Backup.parm
    fs::path parm = base / "Backup.parm";
    std::ofstream(parm) << "dados.txt" << std::endl;

    // Cria arquivo no HD e no Pen, com o Pen mais novo
    fs::path arquivoHD = base / "hd" / "dados.txt";
    fs::path arquivoPen = base / "pen" / "dados.txt";
    std::ofstream(arquivoHD) << "versao antiga";
    std::ofstream(arquivoPen) << "versao nova (mais recente)";

    // Ajusta timestamps: HD mais velho, Pen mais novo
    auto agora = fs::file_time_type::clock::now();
    fs::last_write_time(arquivoHD, agora - std::chrono::hours(1));
    fs::last_write_time(arquivoPen, agora);

    // Executa o backup (HD → Pen)
    auto res = run_proc(parm, base / "hd", base / "pen", destino, true);

    bool found = false;
    for (auto &p : res)
        if (p.first == "dados.txt" && p.second == 5)  // A5 = Erro
            found = true;

    REQUIRE(found == true);

    // O arquivo destino NÃO deve existir (pois é erro)
    REQUIRE(!fs::exists(destino
    / "dados.txt"));
}

TEST_CASE("Caso 7 Restauração: Pen mais novo que HDCopiar Pen->HD",
    "[C7]") {
    namespace fs = std::filesystem;

    fs::path base = fs::path("tests") / "tmp_case_7";
    fs::remove_all(base);
    fs::create_directories(base / "hd");
    fs::create_directories(base / "pen");
    fs::path destino = base / "backup-destino";
    fs::create_directories(destino);

    // Cria Backup.parm
    fs::path parm = base / "Backup.parm";
    std::ofstream(parm) << "dados.txt" << std::endl;

    // Cria arquivo no HD e no Pen (Pen mais novo)
    fs::path arquivoHD = base / "hd" / "dados.txt";
    fs::path arquivoPen = base / "pen" / "dados.txt";
    std::ofstream(arquivoHD) << "versao antiga do HD";
    std::ofstream(arquivoPen) << "versao nova do Pen";

    // Ajusta timestamps
    auto agora = fs::file_time_type::clock::now();
    fs::last_write_time(arquivoHD,
    agora - std::chrono::hours(1));  // HD mais velho
    fs::last_write_time(arquivoPen, agora);  // Pen mais novo

    // Executa restauração (backupSolicitado = false)
    auto res = run_proc(parm, base / "hd", base / "pen", destino, false);

    bool found = false;
    for (auto &p : res)
        if (p.first == "dados.txt" && p.second == 2)  // A2 = Copiar Pen->HD
            found = true;

    REQUIRE(found == true);
    REQUIRE(fs::exists(destino / "dados.txt"));
}

TEST_CASE("Caso 8 Restauração: arquivos iguais",
    "[C8]") {
    namespace fs = std::filesystem;
    fs::path base = fs::path("tests") / "tmp_case_8";
    fs::remove_all(base);
    fs::create_directories(base / "hd");
    fs::create_directories(base / "pen");

    // DESTINO isolado por caso (agora fica dentro de base)
    fs::path destino = base / "backup-destino";
    fs::remove_all(destino);
    fs::create_directories(destino);

    fs::path parm = base / "Backup.parm";
    std::ofstream(parm) << "relatorio.txt" << std::endl;

    fs::path arqHD = base / "hd" / "relatorio.txt";
    fs::path arqPen = base / "pen" / "relatorio.txt";
    std::ofstream(arqHD) << "mesmo conteudo";
    std::ofstream(arqPen) << "mesmo conteudo";

    // Mesma data em ambos
    auto agora = fs::file_time_type::clock::now();
    fs::last_write_time(arqHD, agora);
    fs::last_write_time(arqPen, agora);

    // backupSolicitado = false → restauração
    auto res = run_proc(parm, base / "hd", base / "pen", destino, false);

    bool found = false;
    for (auto &p : res)
        if (p.first == "relatorio.txt" && p.second == 4)  // A4 = Nada
            found = true;

    REQUIRE(found == true);
    REQUIRE(!fs::exists(destino / "relatorio.txt"));
}

TEST_CASE("Caso 9 arquivo não existe em nenhum diretório",
    "[C9]") {
    namespace fs = std::filesystem;

    // Base de teste isolada
    fs::path base = fs::path("tests") / "tmp_case_9";
    fs::remove_all(base);
    fs::create_directories(base / "hd");
    fs::create_directories(base / "pen");

    // Cria destino isolado
    fs::path destino = base / "backup-destino";
    fs::remove_all(destino);
    fs::create_directories(destino);

    // Cria arquivo Backup.parm com nome de um arquivo inexistente
    fs::path parm = base / "Backup.parm";
    std::ofstream(parm) << "fantasma.txt" << std::endl;

    // Executa restauração (backupSolicitado = false)
    auto res = run_proc(parm, base / "hd", base / "pen", destino, false);

    bool found = false;
    for (auto &p : res)
        if (p.first == "fantasma.txt" && p.second == A6)  // A6 = Impossível
            found = true;

    // Verifica se o caso foi identificado corretamente
    REQUIRE(found == true);

    // O arquivo não deve ter sido criado no destino
    REQUIRE(!fs::exists(destino / "fantasma.txt"));
}

TEST_CASE("Caso 10 arquivo não existe nem no HD nem no Pen",
    "[C10]") {
    namespace fs = std::filesystem;

    // Base isolada para este caso
    fs::path base = fs::path("tests") / "tmp_case_10";
    fs::remove_all(base);
    fs::create_directories(base / "hd");
    fs::create_directories(base / "pen");

    // Cria destino isolado
    fs::path destino = base / "backup-destino";
    fs::remove_all(destino);
    fs::create_directories(destino);

    // Cria Backup.parm com arquivo inexistente
    fs::path parm = base / "Backup.parm";
    std::ofstream(parm) << "arquivo_inexistente.txt" << std::endl;

    // Executa o backup (HD → Pen)
    auto res = run_proc(parm, base / "hd", base / "pen", destino, true);

    bool found = false;
    for (auto &p : res)
        if (p.first == "arquivo_inexistente.txt"
        && p.second == A6)  // A6 = Impossível
            found = true;

    REQUIRE(found == true);

    // O arquivo não deve existir no destino
    REQUIRE(!fs::exists(destino / "arquivo_inexistente.txt"));
}

/********************************************************************
* Função: executar_backup
* Descrição
* Executa o processo principal de backup e restauração, analisando o
* arquivo de parâmetros e realizando as cópias entre diretórios HD e Pen
* conforme o modo solicitado (backup ou restauração).
*
* Parâmetros
* backupParm - caminho para o arquivo de parâmetros (.parm)
* dirHD - diretório local do HD
* dirPen - diretório do Pen Drive
* dirDestino - diretório destino onde os arquivos serão copiados
* backupSolicitado - define se é operação de backup (true) ou restauração (false)
*
* Valor retornado
* Retorna um vetor de pares (string, int) onde:
*   - string representa o nome do arquivo processado
*   - int representa o código da ação realizada (A1–A6)
*
* Assertivas de entrada
* backupParm não deve ser uma string vazia
* Os diretórios informados devem existir
* O parâmetro backupSolicitado indica a direção da cópia (HD→Pen ou Pen→HD)
********************************************************************/

extern std::vector<std::pair<std::string, int>> executar_backup(
    const std::string &backupParm,
    const std::string &dirHD,
    const std::string &dirPen,
    const std::string &dirDestino,
    bool backupSolicitado);


/********************************************************************
* Função: run_proc
* Descrição
* Função auxiliar para executar o processo de backup/restauração,
* convertendo os parâmetros do tipo fs::path para string e chamando
* a função principal executar_backup.
*
* Parâmetros
* parm - caminho do arquivo de parâmetros (.parm)
* hd - diretório do HD
* pen - diretório do Pen Drive
* dest - diretório destino para cópias
* backupSolicitado - true para backup, false para restauração
*
* Valor retornado
* Retorna o mesmo vetor de pares (string, int) resultante de executar_backup,
* representando os arquivos processados e seus respectivos códigos de ação.
*
* Assertivas de entrada
* Todos os caminhos fornecidos devem existir e ser acessíveis
* A função executar_backup deve estar corretamente implementada e disponível
********************************************************************/

static std::vector<std::pair<std::string, int>> run_proc(
    const fs::path &parm,
    const fs::path &hd,
    const fs::path &pen,
    const fs::path &dest,
    bool backupSolicitado) {
    return executar_backup(
    parm.string(),
    hd.string(),
    pen.string(),
    dest.string(),
    backupSolicitado);
}
