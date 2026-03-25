# Projeto de Sistemas Operativos

Este repositório contém o projeto desenvolvido no âmbito da disciplina de **Sistemas Operativos**, dividido em três partes progressivas.

O projeto consiste na implementação de um sistema de gestão de voos e reservas, utilizando scripts em **Bash**, com validações, processamento de dados e manipulação de ficheiros.

---

## Estrutura do Projeto

* `PARTE1/` – Validação de dados e interação com o utilizador
* `PARTE2/` – Processamento e manipulação de informação
* `PARTE3/` – Funcionalidades adicionais e integração final

---

## Descrição das Partes

### Parte 1 – Validações e interação

* Verificação da existência de ficheiros necessários
* Pesquisa de voos por origem/destino
* Seleção de voos disponíveis
* Autenticação de utilizadores (ID e senha)

---

### Parte 2 – Processamento de dados

* Verificação de saldo do utilizador
* Atualização de ficheiros (`passageiros.txt`, `voos.txt`)
* Subtração de saldo após compra
* Gestão de lugares disponíveis

---

### Parte 3 – Registo e funcionalidades finais

* Registo de reservas em ficheiros
* Consolidação do sistema
* Integração de todas as funcionalidades anteriores

---

## Tecnologias Utilizadas

* Bash (Shell scripting)
* Sistema Linux/Unix
* Manipulação de ficheiros (`grep`, `awk`, `sed`)

---

## Como Executar

1. Garantir que os ficheiros necessários existem:

   * `voos.txt`
   * `passageiros.txt`

2. Dar permissões de execução ao script:

```bash
chmod +x compra_bilhete.sh
```

3. Executar o script:

```bash
./compra_bilhete.sh
```

---

## Observações

* O projeto segue o enunciado fornecido na disciplina.
* O código contém comentários detalhados explicando cada etapa.
* Cada parte do projeto encontra-se organizada em pastas próprias.

---

## Autor

* Rodrigo Matias
