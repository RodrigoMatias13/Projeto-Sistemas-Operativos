#!/bin/bash
# SO_HIDE_DEBUG=1                   ## Uncomment this line to hide all @DEBUG statements
# SO_HIDE_COLOURS=1                 ## Uncomment this line to disable all escape colouring
. ./so_utils.sh                     ## This is required to activate the macros so_success, so_error, and so_debug

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2023/2024, Enunciado Versão 3+
##
## Aluno: Nº: a123294       Nome: Rodrigo Matias
## Nome do Módulo: S2. Script: compra_bilhete.sh
## Descrição/Explicação do Módulo: neste módulo vemos se os pedidos de voo feito pelo passageiro são validos passando depois para a validação dos argumentos atualizando o ficheiro voos.txt e modificando o ficheiro relatorio_reservas.txt
##
##
###############################################################################

## Este script não recebe nenhum argumento, e permite que o passageiro compre um bilhete para um voo da lista de voos disponíveis. Para realizar a compra, o passageiro deve fazer login para confirmar sua identidade e saldo disponível. Os voos disponíveis estão listados no ficheiro voos.txt. Se não quiser produzir o seu próprio ficheiro, pode utilizar o ficheiro exemplo fornecido, dando o comando: cp voos-exemplo.txt voos.txt. O mesmo procedimento pode ser realizado também com o ficheiro passageiros.txt.

## S2.1. Validações e Pedido de informações interativo:
## S2.1.1. O script valida se os ficheiros voos.txt e passageiros.txt existem. Se algum não existir, dá so_error e termina. Caso contrário, dá so_success.

if [ -f voos.txt ] && [ -f passageiros.txt ]; then
	so_success S2.1.1
else
	so_error S2.1.1
	exit
fi

## S2.1.2. Na plataforma é possível consultar os voos pela sua <Origem> ou <Destino>. Pedindo "Insira a cidade de origem ou destino do voo: _". O utilizador insere a cidade Origem ou Destino do voo (o interesse é que pesquise nos 2 campos). Caso o utilizador tenha introduzido uma cidade que não exista no ficheiro voos.txt, ou se não existirem voos com lugares disponíveis com origem ou destino nessa cidade, dá so_error e termina. Caso contrário, dá so_success <Cidade>.

#Escolha da cidade
echo -n "Insira a cidade de origem ou destino do voo: _"
read Cidade
#Verificação dos voos para a cidade escolhida
if grep "$Cidade" voos.txt; then
#opcoes de voos para a cidade escolhida(percorre o ficheiro voos.txt)
	opcoes=$(grep "$Cidade" voos.txt | wc -l)
	b=1
	c=0
	while [ $b -le $opcoes ]; do
		lugares=$(grep "$Cidade" voos.txt | awk -F '[:]' '{print $8}' | head -"$b" | tail -1)
		if [ -z "$lugares" ] || [ "$lugares" -eq 0 ]; then
			c=$(($c + 1))
			fi
		b=$(($b + 1))
		done
	#Verificação de lugares disponiveis
	if [ "$c" -eq 0 ]; then
		so_success S2.1.2 $Cidade
	else
		so_error S2.1.2
	fi
else
	so_error S2.1.2
	exit
fi

## S2.1.3. O programa pede ao utilizador para inserir uma opção de voo, listando os voos que existem de acordo com a origem/destino inserida anteriormente, da seguinte forma: "Lista de voos, numerada, ou 0 para sair, Insira o voo que pretende reservar: _" O utilizador insere a opção do voo (neste exemplo, números de 1 a 3 ou 0). Se o utilizador escolheu um número de entre as opções de voos apresentadas (neste caso, entre 1 e 3), dá so_success <opção>. Caso contrário, dá so_error e termina.

#Numero de voos para a cidade escolhida; 
opcoes=$(grep "$Cidade" voos.txt | wc -l)
#voos disponiveis 
a=1
while [ $a -le $opcoes ]; do
echo ""$a".$(grep "$Cidade" voos.txt | awk -F '[:]' '{print$2}' | head -"$a" | tail -1) para $(grep "$Cidade" voos.txt | awk -F '[:]' '{print$3}' | head -"$a" | tail -1), $(grep "$Cidade" voos.txt | awk -F '[:]' '{print$4}' | head -"$a" | tail -1), Partida:$(grep "$Cidade" voos.txt | awk -F '[:]' '{print$5}' | head -"$a" | tail -1), Preço: $(grep "$Cidade" voos.txt | awk -F '[:]' '{print$6}' | head -"$a" | tail -1), Disponíveis:$(grep "$Cidade" voos.txt | awk -F '[:]' '{print$8}' | head -"$a" | tail -1) lugares"
a=$((a + 1))
done
#Escolha do voo pelo passageiro
echo "0.Sair"
read escolha
#Ver se a esolha é valida
if [[ "$escolha" =~ ^[0-9]+$ ]]; then
if [ "$escolha" -le "$opcoes" ]; then
    so_success S2.1.3
    else
    so_error S2.1.3
    fi
else
so_error S2.1.3
exit 1
fi

## S2.1.4. O programa pede ao utilizador o seu <ID_passageiro>: "Insira o ID do seu utilizador: _" O utilizador insere o respetivo ID de passageiro (dica: UserId Linux). Se esse ID não estiver registado no ficheiro passageiros.txt, dá so_error e termina. Caso contrário, reporta so_success <ID_passageiro>.

read -p "Insira o ID do seu utilizador: " ID_passageiro
if grep -q "^$ID_passageiro" passageiros.txt; then
	so_success  S2.1.4 "$ID_passageiro"
else
	so_error S2.1.4
	exit
fi

## S2.1.5. O programa pede ao utilizador a sua <Senha>: "Insira a senha do seu utilizador: _" O utilizador insere a respetiva senha. Caso o script veja que essa senha não é a registada para esse passageiro no ficheiro passageiros.txt, dá so_error e termina. Caso contrário, reporta so_success.

read -p "Insira a senha do seu utilizador: " senha
if grep -q "^$ID_passageiro:$senha" passageiros.txt; then
	so_success S2.1.5
else
	so_error S2.1.5
	exit
fi

## S2.2. Processamento da resposta:
## S2.2.1. Valida se o passageiro possui <Saldo>, definido no ficheiro passageiros.txt, para comprar o bilhete selecionado no passo S2.1.3. Se a compra não é possível por falta de saldo, dá so_error <preço voo> <Saldo> e termina. Caso contrário, dá so_success <preço voo> <Saldo>.

#Ver se existe o ID do passageiro e a senha
if grep -q "^$ID_passageiro:$senha:[0-9]*$" passageiros.txt; then
	#Calculo do saldo e preco do voo
	saldo=$(grep "^$ID_passageiro:$senha" passageiros.txt | cut -d: -f3)
	preco_voo=$(grep -i "^$origem_destino" voos.txt | cut -d, -f3)
	#Ver se o passageiro tem saldo suficiente
	if [ "$saldo" -ge "$preco_voo" ]; then
		so_success S2.2.1 "$preco_voo $saldo"
	else
		so_error S2.2.1"$preco_voo $saldo"
	fi
else
	so_error
fi

## S2.2.2. Subtrai o valor do <preço voo> no <Saldo> do passageiro, e atualiza o ficheiro passageiros.txt. Em caso de erro (e.g., na escrita do ficheiro), dá so_error e termina. Senão, dá so_success <Saldo Atual>.

#Ver se existe o ID do passageiro e a senha
if grep -q "^$ID_passageiro:$senha:[0-9]*$" passageiros.txt; then
	#Calculo do saldo e preco do voo
	saldo=$(grep "^$ID_passageiro:$senha" passageiros.txt | cut -d: -f3)
	preco_voo=$(grep -i "^$origem_destino" voos.txt | cut -d, -f3)
	#Ver se tem saldo suficiente e depois subtrair com o preco do voo
	if [ "$saldo" -ge "$preco_voo" ]; then
		saldo_atual=$(( saldo - preco_voo ))
		sed -i "s/^$ID_passageiro:$senha:$saldo/^$ID_passageiro:$senha:$saldo_atual/" passageiros.txt
		if [ $? -eq 0 ]; then
			so_success S2.2.2"$saldo_atual"
		else
			so_error S2.2.2
	fi
fi
## S2.2.3. Decrementa uma unidade aos lugares disponíveis do voo escolhidos no passo S2.1.3, e atualiza o ficheiro voos.txt. Em caso de erro (por exemplo, na escrita do ficheiro), dá so_error e termina. Senão, dá so_success.



## S2.2.4. Regista a compra no ficheiro relatorio_reservas.txt, inserido uma nova linha no final deste ficheiro. Em caso de erro (por exemplo, na escrita do ficheiro), dá so_error e termina. Caso contrário, dá so_success.
