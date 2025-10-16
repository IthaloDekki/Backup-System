#ifndef BACKUP_HPP
#define BACKUP_HPP

#include <string>
#include <vector>
#include <utility>

/**
 * Função usada pelos testes:
 * executar_backup(backupParm, dirHD, dirPen, dirDestino, backupSolicitado)
 *
 * Retorna vetor de pares (nomeArquivo, codigoAcao)
 * Códigos de ação:
 * 1 = A1 Copiar HD -> Destino
 * 2 = A2 Copiar Destino -> HD
 * 3 = A3 Excluir do Destino
 * 4 = A4 Nada
 * 5 = A5 Erro
 * 6 = A6 Impossível (ex.: Backup.parm ausente)
 */
std::vector<std::pair<std::string,int>> executar_backup(
    const std::string &backupParm,
    const std::string &dirHD,
    const std::string &dirPen,
    const std::string &dirDestino,
    bool backupSolicitado);

#endif // BACKUP_HPP
