# Parte 1 – Validações e Interação

Esta parte do projeto foca-se na **validação de dados** e na **interação com o utilizador**, permitindo iniciar o processo de compra de bilhetes de voo.

---

## Funcionalidades

* Verificação da existência dos ficheiros:

  * `voos.txt`
  * `passageiros.txt`
* Pesquisa de voos por **origem ou destino**
* Listagem de voos disponíveis
* Seleção de voo pelo utilizador
* Validação do ID do passageiro
* Validação da senha

---

## Como Executar

1. Garantir que os ficheiros necessários existem:

   * `voos.txt`
   * `passageiros.txt`

2. Dar permissões ao script:

```bash
chmod +x compra_bilhete.sh
```

3. Executar:

```bash
./compra_bilhete.sh
```

---

## Observações

* O script utiliza comandos como `grep`, `awk` e `read` para processar dados.
* Inclui validações detalhadas em cada etapa.
* O código está comentado para facilitar a compreensão.
