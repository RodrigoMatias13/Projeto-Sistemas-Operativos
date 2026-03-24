#!/bin/bash
# SO_HIDE_DEBUG=1                   ## Uncomment this line to hide all @DEBUG statements
# SO_HIDE_COLOURS=1                 ## Uncomment this line to disable all escape colouring
. ./so_utils.sh                     ## This is required to activate the macros so_success, so_error, and so_debug

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2023/2024, Enunciado Versão 3+
##
## Aluno: Nº: a123294       Nome: Rodrigo Matias 
## Nome do Módulo: S4. Script: stats.sh
## Descrição/Explicação do Módulo: Neste módulo verificamos se os argumentos são validos e depois invocamos o ficheiro stats.txt criando o mesmo
##
##
###############################################################################

## Este script obtém informações sobre o sistema, afixando resultados diferentes no STDOUT consoante os argumentos passados na sua invocação. A sintaxe resumida é: ./stats.sh <passageiros>|<top <nr>>

## S4.1. Validações:
## S4.1.1. Valida os argumentos recebidos e, conforme os mesmos, o número e tipo de argumentos recebidos. Se não respeitarem a especificação, dá so_error e termina. Caso contrário, dá so_success.

#Verificar o primeiro argumento
if [[ "$1" == "passageiro" ]]; then
    so_success S4.1.1
#Verificar o primeiro e segundo argumento
elif [[ $1 == "top" ]] && [[ $2 =~ [0-9].* ]]; then
    so_success S4.1.1
else
    so_error S4.1.1
    exit
fi

## S4.2. Invocação do script:
## S4.2.1. Se receber o argumento passageiros, (i.e., ./stats.sh passageiros) cria um ficheiro stats.txt onde lista o nome de todos os utilizadores que fizeram reservas, por ordem decrescente de número de reservas efetuadas, e mostrando o seu valor total de compras. Em caso de erro (por exemplo, se não conseguir ler algum ficheiro necessário), dá so_error e termina. Caso contrário, dá so_success e cria o ficheiro. Em caso de empate no número de reservas, lista o primeiro do ficheiro. Preste atenção ao tratamento do singular e plural quando se escreve “reserva” no ficheiro).

#Verifica o primeiro argumento
if [[ "$1" == "passageiros" ]]; then
#Cria o ficheiro stats.txt
touch stats.txt
#Contagem de passageiros no ficheiro passageiros.txt, extrai o ID, conta o número de repetições do ID em "relatorio_reservas.txt", calcula o dinheiro total gasto pelo passageiro e guarda no ficheiro "stats.txt".
l=$(cat passageiros.txt | cut -d ':' -f3 | sort | uniq -c | wc -l)
d=1
while [ $d -le $l ]; do
    n=$(cat passageiros.txt | cut -d ':' -f3 | head - "$d" | tail -1)
    id=$(grep "$n" passageiros.txt | cut -d ':' -f1 | head -"$d" | tail -1)
    ocorrencias=$(grep "$id" relatorio_reservas.txt | wc -l)
    di=$(grep "$id" relatorio_reservas.txt | cut -d ':' -f5)
    dinheiro=0
    while read -r linha; do
        dinheiro=$(($dinheiro + $linha))
    done <<< "$di"
    if [ $ocorrencias -eq 1 ]; then
        echo "$n: $ocorrencias reserva; $dinheiro€" >> stats.txt
    else
        echo "$n: $ocorrencias reservas; $dinheiro€" >> stats.txt
    fi
    d=$(($d + 1))
done
so_success S4.2.1
else
so_error S4.2.1
fi

## S4.2.2. Se receber o argumento top <nr:number>, (e.g., ./stats.sh top 4), cria um ficheiro stats.txt onde lista os <nr> (no exemplo, os 4) voos mais rentáveis (que tiveram melhores receitas de vendas), por ordem decrescente. Em caso de erro (por exemplo, se não conseguir ler algum ficheiro necessário), dá so_error e termina. Caso contrário, dá so_success e cria o ficheiro. Em caso de empate, lista o primeiro do ficheiro.
