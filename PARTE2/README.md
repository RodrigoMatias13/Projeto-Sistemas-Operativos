# Parte 2 – Processamento de Dados

Esta parte do projeto trata do **processamento da compra do bilhete**, incluindo validações de saldo e atualização de ficheiros.

---

## Funcionalidades

* Verificação do saldo do passageiro
* Comparação entre saldo e preço do voo
* Subtração do valor do bilhete ao saldo
* Atualização do ficheiro `passageiros.txt`
* Atualização dos lugares disponíveis no ficheiro `voos.txt`

---

## Como Funciona

1. O sistema verifica se o utilizador tem saldo suficiente
2. Caso tenha:

   * O saldo é atualizado
   * Os lugares disponíveis são decrementados
3. Caso não tenha:

   * A operação é cancelada

---

## Observações

* Utiliza ferramentas como `sed` para modificar ficheiros.
* Manipula dados diretamente a partir de ficheiros `.txt`.
* Depende da validação feita na Parte 1.
