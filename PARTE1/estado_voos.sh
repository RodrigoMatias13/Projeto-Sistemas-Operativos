#!/bin/bash
# SO_HIDE_DEBUG=1                   ## Uncomment this line to hide all @DEBUG statements
# SO_HIDE_COLOURS=1                 ## Uncomment this line to disable all escape colouring
. ./so_utils.sh                     ## This is required to activate the macros so_success, so_error, and so_debug

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2023/2024, Enunciado Versão 3+
##
## Aluno: Nº: a123294       Nome: Rodrigo Matias
## Nome do Módulo: S3. Script: estado_voos.sh
## Descrição/Explicação do Módulo: Neste módulo comecamos por validar o ficheiro voos.txt e ver se corresponde a especificação pedida e depois processamos o script 
##
##
###############################################################################

## Este script não recebe nenhum argumento, e é responsável pelo relatório do estado dos voos que pertencem à plataforma IscteFlight.

## S3.1. Validações:
## S3.1.1. O script valida se o ficheiro voos.txt existe. Se não existir, dá so_error e termina. Senão, dá so_success.

if [ -f voos.txt ]; then
    so_success S3.1.1
else
    so_error S3.1.1
    exit
fi

## S3.1.2. O script valida se os formatos de todos os campos de cada linha do ficheiro voos.txt correspondem à especificação indicada em S2, nomeadamente se respeitam os formatos de data e de hora. Se alguma linha não respeitar, dá so_error <conteúdo da linha> e termina. Caso contrário, dá so_success.

formatoNumVoo='^[[:alpha:]]{2}[0-9]{4}$'
formatoCidades='^[[:alpha:]]+$'
formatoData='^[0-9]{4}-[0-9]{2}-[0-9]{2}$'
formatoHora='^[0-9]{2}h[0-9]{2}$'
formatoNums='^[0-9]+$'

#ler linhas ficheiro voos.txt
while read -r linha; do
    numVoo=$(echo "$linha" | awk -F ":" '{print $1}')
    origem=$(echo "$linha" | awk -F ":" '{print $2}')
    destino=$(echo "$linha" | awk -F ":" '{print $3}')
    data=$(echo "$linha" | awk -F ":" '{print $4}')
    hora=$(echo "$linha" | awk -F ":" '{print $5}')
    preco=$(echo "$linha" | awk -F ":" '{print $6}')
    lotacao=$(echo "$linha" | awk -F ":" '{print $7}')
    lugaresVazios=$(echo "$linha" | awk -F ":" '{print $8}')

    #Validar se corresponde à esecificação pedida em S2
    if ! [[ $numVoo =~ $formatoNumVoo && $origem =~ $formatoCidades && $destino =~ $formatoCidades && $data =~ $formatoData && $hora =~ $formatoHora && $preco =~ $formatoNums && $lotacao =~ $formatoNums && $lugaresVazios =~ $formatoNums  ]]; then
        so_error S3.1.2 "$linha"
        exit 1
    fi

done < voos.txt

so_success S3.1.2

## S3.2. Processamento do script:
## S3.2.1. O script cria uma página em formato HTML, chamada voos_disponiveis.html, onde lista os voos com lugares disponíveis, indicando nº, origem, destino, data, hora, lotação, nº de lugares disponíveis, e nº de lugares ocupados (para isso deve calcular a diferença dos anteriores). Em caso de erro (por exemplo, se não conseguir escrever no ficheiro), dá so_error e termina. Caso contrário, dá so_success.

## S3.3. Invocação do script estado_voos.sh:
## S3.3.1. Altere o ficheiro cron.def fornecido, por forma a configurar o seu sistema para que o script seja executado de hora em hora, diariamente. Nos comentários no início do ficheiro cron.def, explique a configuração realizada, e indique qual o comando que deveria utilizar para despoletar essa configuração. O ficheiro cron.def alterado deverá ser submetido para avaliação juntamente com os outros Shell scripts.
