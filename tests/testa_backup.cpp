// tests/testa_backup.cpp
#define CATCH_CONFIG_MAIN
#include "../src/catch_amalgamated.hpp"
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

// Assinatura esperada (será implementada depois)
// std::vector<std::pair<std::string,int>> executar_backup(
//     const std::string &backupParm,
//     const std::string &dirHD,
//     const std::string &dirPen,
//     const std::string &dirDestino,
//     bool backupSolicitado
// );

static constexpr int A6 = 6;

static std::vector<std::pair<std::string,int>> run_proc(
    const fs::path &parm, const fs::path &hd, const fs::path &pen, const fs::path &dest, bool backupSolicitado);

// Caso 1: C1 = F -> Impossível (A6)
TEST_CASE("Caso 1 - Sem Backup.parm => Impossivel (A6)", "[C1]") {
    // diretórios de teste mínimos
    fs::path base = fs::path("tests") / "tmp_case_1";
    fs::remove_all(base);
    fs::create_directories(base / "hd");
    fs::create_directories(base / "pen");
    fs::create_directories("backup-destino");

    fs::path parm = base / "Backup.parm"; // não criado -> C1 = F

    auto res = run_proc(parm, base / "hd", base / "pen", "backup-destino", true);

    bool found = false;
    for (auto &p : res) if (p.second == A6) found = true;
    REQUIRE(found == true);
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
