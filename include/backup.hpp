#ifndef BACKUP_HPP
#define BACKUP_HPP

#include <string>
#include <vector>
#include <utility>

enum Acao {
    A1_COPIAR_HD_PEN = 1,
    A2_COPIAR_PEN_HD,
    A3_EXCLUIR,
    A4_NADA,
    A5_ERRO,
    A6_IMPOSSIVEL
};

std::vector<std::pair<std::string,int>> executar_backup(
    const std::string &backupParm,
    const std::string &dirHD,
    const std::string &dirPen,
    const std::string &dirDestino,
    bool backupSolicitado);

#endif // BACKUP_HPP
