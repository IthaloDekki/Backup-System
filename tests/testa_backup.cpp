
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
