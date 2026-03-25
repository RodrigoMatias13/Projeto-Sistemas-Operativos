# Parte 1 – Compra de Bilhete e Validações

Esta parte do projeto implementa o processo de **compra de bilhetes de voo**, incluindo validações e interação com o utilizador.

---

## Funcionalidades

* Verificação da existência dos ficheiros:

  * `voos.txt`
  * `passageiros.txt`
* Pesquisa de voos por origem ou destino
* Listagem de voos disponíveis
* Seleção de voo
* Validação do ID do passageiro
* Validação da senha
* Verificação de saldo
* Atualização do ficheiro `passageiros.txt`
* Atualização do ficheiro `voos.txt`
* Registo da reserva em `relatorio_reservas.txt`

---

## Como Executar

```bash
chmod +x compra_bilhete.sh
./compra_bilhete.sh
```

---

## Observações

* Utiliza comandos como `grep`, `awk` e `sed`
* Realiza validações em várias etapas do processo
* Representa a primeira fase do sistema de reservas

## Observações

* O script utiliza comandos como `grep`, `awk` e `read` para processar dados.
* Inclui validações detalhadas em cada etapa.
* O código está comentado para facilitar a compreensão.
