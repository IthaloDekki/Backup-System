Trabalho TDD – Técnicas de Programação 2
Autor: Íthalo Júnio Medeiros de Oliveira Nóbrega
Matrícula: 222008806

=====================================================
Descrição do Projeto

O objetivo deste trabalho é utilizar o desenvolvimento orientado a testes (TDD) para
implementar um sistema de backup que lê um arquivo de parâmetros (data/Backup.parm)
e copia os arquivos listados para um destino (simulando um pendrive).

O programa:

Lê data/Backup.parm (formato simples, veja abaixo).

Verifica o destino (pendrive): existência e permissão de escrita.

Para cada caminho listado no arquivo de parâmetros, tenta copiar para o destino,
preservando a hierarquia relativa (cria subdiretórios quando necessário).

Produz códigos de saída e mensagens claras para uso em testes automatizados.

O projeto foi implementado em C++ seguindo a metodologia TDD, com framework de testes
Catch2 (amalgamado incluído no repositório), utilizando ferramentas auxiliares como
cpplint, cppcheck, gcov, gdb, valgrind e documentação gerada por Doxygen.

=====================================================
Formato do data/Backup.parm (exemplo)

Linhas vazias e que começam com # são ignoradas.
Uma linha opcional DEST=... pode indicar o diretório destino (pendrive).
As demais linhas (não comentadas) são caminhos de arquivos a copiar.

Exemplo:

# Backup.parm - exemplo
DEST=./data/pendrive

data/exemplo1.txt
data/subdir/arquivo2.log
/home/ithalo/outro_arquivo.txt


Regras:

DEST= opcional. Se ausente, usa ./data/pendrive.

Caminhos podem ser relativos ou absolutos.

Se um arquivo não existir, o programa reporta o erro (e continua ou falha parcialmente, conforme o teste/implementação).

Mensagens de log devem ser claras para permitir assertivas nos testes.

=====================================================
Códigos de saída sugeridos

0 — sucesso total (todos os arquivos copiados).

1 — pendrive ausente (destino não existe).

2 — algum(s) arquivo(s) listado(s) não encontrado(s).

3 — erro de permissão / falha durante cópia.

=====================================================
Instruções de Compilação e Execução

O projeto utiliza um Makefile para automatizar compilação, testes, análise e geração de docs.

1- Compilar o programa

$ make compile

Isso gera o executável de testes:
./testa_backup

(Obs.: make por padrão também compila e executa o binário.)

2- Executar os testes

$ make test

Ou simplesmente:
$ make

3- Verificação de estilo (cpplint)

$ make cpplint

Roda o cpplint via python3 -m cpplint para verificar conformidade ao estilo Google C++.

4- Verificação de cobertura (gcov)

$ make gcov

Compila com suporte a cobertura, executa os testes e gera arquivos .gcov para inspeção.

5- Depuração com GDB

$ make debug

Compila com símbolos de depuração e abre o GDB com o executável de teste.

6- Verificação estática (cppcheck)

$ make cppcheck

Roda o analisador estático cppcheck no código do projeto (com supressões para Catch).

7- Verificação dinâmica (Valgrind)

$ make valgrind

Executa o binário de teste sob Valgrind e salva o relatório em:
valgrind.rpt

Saída esperada (trecho exemplo em caso de sem leaks):

==XXXX== HEAP SUMMARY:
==XXXX==     in use at exit: 0 bytes in 0 blocks
==XXXX==   total heap usage: 234 allocs, 234 frees, 129,881 bytes allocated
==XXXX== 
==XXXX== All heap blocks were freed -- no leaks are possible

8- Gerar documentação (Doxygen)

$ make docs

Gera Doxyfile (se não existir) e executa doxygen. Saída em:
./docs/html/index.html

9- Limpeza de arquivos gerados

$ make clean

Remove arquivos objetos (.o), executáveis, arquivos temporários de cobertura, valgrind e docs.

=====================================================
Como simular o "pendrive" (recomendado para testes)

Método simples (recomendado):

# criar diretório que representa o pendrive
mkdir -p ./data/pendrive
chmod u+rwx ./data/pendrive


Testar pendrive ausente:

rm -rf ./data/pendrive


Testar pendrive sem permissão de escrita:

mkdir -p ./data/pendrive
chmod -w ./data/pendrive
# restaurar:
chmod u+rwx ./data/pendrive


(Não é obrigatório montar um dispositivo real; um diretório simulado é suficiente para a validação e para TDD.)

=====================================================
Sugestão de casos de teste (mapeados por TDD)

Cada coluna da tabela de decisão deve ser um TEST_CASE:

Pendrive presente e todos os arquivos existem → sucesso (0).

Pendrive ausente → erro pendrive (1).

Arquivo listado inexistente → erro parcial (2).

Pendrive sem permissão de escrita → erro permissão (3).

DEST inválido ou formato de Backup.parm incorreto → erro de configuração.

Nos testes unitários, prefira chamar funções internas (run_backup(parm_path)) em vez de executar o binário via system(), para melhor performance e cobertura.

=====================================================
Observações

O compilador utilizado é o g++.

O padrão C++ utilizado é C++14.

O projeto contém o Catch2 em formato amalgamado (não é necessário instalar Catch2 separadamente).

Dependências para análise e testes:

make, g++, cppcheck, valgrind, gdb, doxygen, gcov

cpplint (Python) — pip3 install --user cpplint ou pipx install cpplint

Documente as assertivas de entrada/saída nas funções (cabeçalhos com descrição, parâmetros, retorno, assertivas), conforme exigido no enunciado.

O repositório deve ser enviado compactado com o diretório .git incluído, para comprovar o histórico de commits do TDD.

=====================================================
Resumo curto para o topo do README

Backup-System — Implementação em C++ de um sistema de backup que lê data/Backup.parm e copia os arquivos listados para um destino (pendrive simulado). Desenvolvido com metodologia TDD (Catch2), análise estática (cppcheck, cpplint), verificação de memória (valgrind), cobertura (gcov) e documentação (Doxygen).