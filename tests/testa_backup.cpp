
#define CATCH_CONFIG_MAIN
#include "../src/catch_amalgamated.hpp"
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <system_error>
#include <cassert>

namespace fs = std::filesystem;

static constexpr int A6 = 6;

static std::vector<std::pair<std::string,int>> run_proc(
    const fs::path &parm, const fs::path &hd, const fs::path &pen, const fs::path &dest, bool backupSolicitado);

TEST_CASE("Caso 1 - Sem Backup.parm => Impossivel (A6)", "[C1]") {

    fs::path base = fs::path("tests") / "tmp_case_1";
    fs::remove_all(base);
    fs::create_directories(base / "hd");
    fs::create_directories(base / "pen");
    fs::create_directories("backup-destino");

    fs::path parm = base / "Backup.parm";

    auto res = run_proc(parm, base / "hd", base / "pen", "backup-destino", true);

    bool found = false;
    for (auto &p : res) if (p.second == A6) found = true;
    REQUIRE(found == true);
}

TEST_CASE("Caso 2 - Backup solicitado, arquivo só no HD => Copiar HD->Pen (A1)", "[C2]") {
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
    auto res = run_proc(parm, base / "hd", base / "pen", "backup-destino", true);

    bool found = false;
    for (auto &p : res)
        if (p.first == "documento.txt" && p.second == 1) // A1 = Copiar HD→Pen
            found = true;

    // O arquivo deve ter sido copiado para o destino (pen)
    REQUIRE(found == true);
    REQUIRE(fs::exists("backup-destino/documento.txt"));
}

TEST_CASE("Caso 3 - Restauração: arquivo no Pen e não no HD => Copiar Pen->HD (A2)", "[C3]") {
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
    auto res = run_proc(parm, base / "hd", base / "pen", "backup-destino", false);

    bool found = false;
    for (auto &p : res)
        if (p.first == "relatorio.txt" && p.second == 2) // A2 = Copiar Pen->HD
            found = true;

    // Verifica se o arquivo foi copiado para o destino
    REQUIRE(found == true);
    REQUIRE(fs::exists("backup-destino/relatorio.txt"));
}

TEST_CASE("Caso 4 - Backup solicitado, arquivos com mesma data => Nada a fazer (A4)", "[C4]") {
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
        if (p.first == "relatorio.txt" && p.second == 4) // A4 = Nada
            found = true;

    REQUIRE(found == true);

    // O arquivo destino nao deve existir
    REQUIRE(!fs::exists(destino / "relatorio.txt"));
}

TEST_CASE("Caso 5 - Backup solicitado, arquivo do HD mais novo => Copiar HD->Pen (A1)", "[C5]") {
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
        if (p.first == "dados.txt" && p.second == 1) // A1 = Copiar HD→Pen
            found = true;

    REQUIRE(found == true);

    // O arquivo destino (Pen simulado) deve existir e conter o conteúdo atualizado
    REQUIRE(fs::exists(destino / "dados.txt"));
}

TEST_CASE("Caso 6 - Backup solicitado, Pen mais novo que HD => Erro (A5)", "[C6]") {
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
        if (p.first == "dados.txt" && p.second == 5) // A5 = Erro
            found = true;

    REQUIRE(found == true);

    // O arquivo destino NÃO deve existir (pois é erro)
    REQUIRE(!fs::exists(destino / "dados.txt"));
}

TEST_CASE("Caso 7 - Restauração: Pen mais novo que HD => Copiar Pen->HD (A2)", "[C7]") {
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
    fs::last_write_time(arquivoHD, agora - std::chrono::hours(1)); // HD mais velho
    fs::last_write_time(arquivoPen, agora); // Pen mais novo

    // Executa restauração (backupSolicitado = false)
    auto res = run_proc(parm, base / "hd", base / "pen", destino, false);

    bool found = false;
    for (auto &p : res)
        if (p.first == "dados.txt" && p.second == 2) // A2 = Copiar Pen->HD
            found = true;

    REQUIRE(found == true);
    REQUIRE(fs::exists(destino / "dados.txt"));
}

TEST_CASE("Caso 8 - Restauração: arquivos iguais => Nada a fazer (A4)", "[C8]") {
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
        if (p.first == "relatorio.txt" && p.second == 4) // A4 = Nada
            found = true;

    REQUIRE(found == true);
    REQUIRE(!fs::exists(destino / "relatorio.txt"));
}

TEST_CASE("Caso 9 - Restauração: arquivo não existe em nenhum diretório => Impossível restaurar (A6)", "[C9]") {
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
        if (p.first == "fantasma.txt" && p.second == A6) // A6 = Impossível
            found = true;

    // Verifica se o caso foi identificado corretamente
    REQUIRE(found == true);

    // O arquivo não deve ter sido criado no destino
    REQUIRE(!fs::exists(destino / "fantasma.txt"));
}



// link-time: chamaremos a função real depois (aqui só declaro o wrapper)
#include <utility>
extern std::vector<std::pair<std::string,int>> executar_backup(
    const std::string &backupParm,
    const std::string &dirHD,
    const std::string &dirPen,
    const std::string &dirDestino,
    bool backupSolicitado);

static std::vector<std::pair<std::string,int>> run_proc(
    const fs::path &parm, const fs::path &hd, const fs::path &pen, const fs::path &dest, bool backupSolicitado)
{
    return executar_backup(parm.string(), hd.string(), pen.string(), dest.string(), backupSolicitado);
}
