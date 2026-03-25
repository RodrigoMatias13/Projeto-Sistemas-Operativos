# Projeto de Sistemas Operativos

Este repositório contém o projeto desenvolvido no âmbito da disciplina de **Sistemas Operativos**, implementado em **Bash**, que simula um sistema de gestão de voos.

O sistema permite realizar todo o fluxo de um passageiro:

* Compra de bilhete
* Check-in eletrónico
* Escolha de lugar

---

## Estrutura do Projeto

* `PARTE1/` – Compra de bilhetes e validações iniciais
* `PARTE2/` – Check-in eletrónico
* `PARTE3/` – Escolha de lugares no voo

---

## Descrição das Funcionalidades

### Parte 1 – Compra de Bilhete

* Validação de ficheiros (`voos.txt`, `passageiros.txt`)
* Pesquisa de voos por origem/destino
* Seleção de voo
* Autenticação do passageiro
* Atualização de saldo e lugares disponíveis
* Registo da reserva

---

### Parte 2 – Check-in Eletrónico

* Identificação do passageiro
* Validação de reservas existentes
* Realização do check-in
* Atualização da informação associada ao voo

---

### Parte 3 – Escolha de Lugar

* Apresentação de lugares disponíveis
* Seleção de assento pelo passageiro
* Validação de disponibilidade
* Atualização dos dados do voo

---

## Tecnologias Utilizadas

* Bash (Shell scripting)
* Ambiente Linux/Unix
* Ferramentas: `grep`, `awk`, `sed`

---

## Como Executar

1. Garantir que os ficheiros necessários existem:

   * `voos.txt`
   * `passageiros.txt`

2. Dar permissões de execução:

```bash
chmod +x compra_bilhete.sh
```

3. Executar:

```bash
./compra_bilhete.sh
```

---

## Observações

* O projeto segue o enunciado da disciplina
* Os scripts estão organizados por partes
* Cada parte representa uma fase do sistema de reservas

---

## Autor

* Rodrigo Matias
