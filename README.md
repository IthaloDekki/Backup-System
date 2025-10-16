Trabalho Backup System – Técnicas de Programação 2
Autor: Íthalo Júnio Medeiros de Oliveira Nóbrega
Matrícula: 222008806

=====================================================
Descrição do Projeto
=====================================================
O objetivo deste trabalho é utilizar o desenvolvimento orientado a testes (TDD) para
resolver o problema de verificar o jogo da velha.

O jogo da velha é representado como uma matriz 3x3 de inteiros:
- O valor 0 significa que a posição está vazia.
- O valor 1 significa que a posição está com um X.
- O valor 2 significa que a posição está com um O.

A função desenvolvida deve:
- Retornar 1 se o vencedor foi o X.
- Retornar 2 se o vencedor foi o O.
- Retornar 0 se o jogo está empatado.
- Retornar -1 se o jogo está indefinido (por exemplo, apenas uma jogada feita).
- Retornar -2 se o jogo é impossível pelas regras (por exemplo, todas as posições iguais).

O projeto foi implementado em C++ seguindo a metodologia TDD, com framework de testes
Catch2, utilizando ferramentas auxiliares como cpplint, cppcheck, gcov, gdb e valgrind.

=====================================================
Instruções de Compilação e Execução
=====================================================

O projeto utiliza um Makefile para automatizar a compilação, testes,
análise estática, cobertura e verificação dinâmica de memória.

-----------------------------------------------------
1- Compilar o programa
-----------------------------------------------------
$ make compile

Isso gera o executável:
  ./testa_velha

-----------------------------------------------------
2- Executar os testes
-----------------------------------------------------
$ make test

Ou simplesmente:
$ make

-----------------------------------------------------
3- Verificação de estilo (cpplint)
-----------------------------------------------------
$ make cpplint

Verifica se o código segue o padrão de estilo Google C++.

-----------------------------------------------------
4- Verificação de cobertura (gcov)
-----------------------------------------------------
$ make gcov

Compila com suporte a cobertura, executa os testes e
gera relatório de cobertura em arquivos .gcov.

-----------------------------------------------------
5- Depuração com GDB
-----------------------------------------------------
$ make debug

Compila com símbolos de depuração e abre o GDB com o executável.

-----------------------------------------------------
6- Verificação estática (cppcheck)
-----------------------------------------------------
$ make cppcheck

Roda o analisador estático cppcheck para detectar problemas
potenciais no código.

-----------------------------------------------------
7- Verificação dinâmica (Valgrind)
-----------------------------------------------------
$ make valgrind

Executa o programa de testes com Valgrind e salva o relatório em:
  valgrind.rpt

Caso seja de interesse usando o cat valgrind.rpt a saída esperada é:

==XXXX== HEAP SUMMARY:
==XXXX==     in use at exit: 0 bytes in 0 blocks
==XXXX==   total heap usage: 234 allocs, 234 frees, 129,881 bytes allocated
==XXXX== 
==XXXX== All heap blocks were freed -- no leaks are possible

-----------------------------------------------------
8- Limpeza de arquivos gerados
-----------------------------------------------------
$ make clean

Remove arquivos objetos (.o), executáveis e arquivos temporários
de cobertura ou Valgrind.

=====================================================
Observações
=====================================================
- O compilador utilizado é o g++.
- O padrão C++ utilizado é C++14.
- Todas as dependências estão no próprio projeto (inclui Catch2).
- O programa foi testado com Valgrind e não apresenta vazamentos
  de memória ou acessos inválidos.