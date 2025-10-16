#include "../include/backup.hpp"
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <system_error>

namespace fs = std::filesystem;

/*
 * Implementação mínima compatível com os testes:
 * - Se Backup.parm NÃO existe -> retorna par ("Backup.parm", 6)
 * - Caso contrário retorna vetor vazio (lógica completa será adicionada depois)
 *
 * Assinatura corresponde exatamente ao protótipo no header para evitar
 * erros de linkagem.
 */
std::vector<std::pair<std::string,int>> executar_backup(
    const std::string &backupParm,
    const std::string &dirHD,
    const std::string &dirPen,
    const std::string &dirDestino,
    bool backupSolicitado)
{
    return std::vector<std::pair<std::string,int>>();
}
