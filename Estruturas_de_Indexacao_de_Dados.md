# Estruturas de Indexação de Dados

## Índice

Estrutura de acesso auxiliar usada para melhorar o desempenho na recuperação de registros.

Qualquer campo ou conjunto de campos pode ser usado para criar um índice, sendo que um campo pode ter mais de um índice. Vários índices podem ser definidos para um mesmo arquivo.

A pesquisa é realizada em resposta a certas condições, além de ser restringida a um subconjunto dos registros, em contrapartida à análise do conjunto completo. Ou seja, para achar dado específico, não é preciso passar por todos os outros dados. Isto é chamado de realizar **poda** nos dados.

O índice ajuda na recuperação dos dados aumentando o desempenho nas consultas, porém tem custo de manutenção.

### Índice Simples ou Linear
Chave de busca usada para indexar os registros, junto a um campo de referência (RRN ou byte offset).
Tem dois arquivos, um de dados e um de índice.

Deve ser utilizado quando o arquivo de índice cabe totalmente em RAM, por ser muito caro de ser mantido em disco devido ao grande número de acessos e alterações. Quando não cabe utilizamos outros métodos, como o índice árvore b, o índice hash, etc.

Arquivo de dados
- Campos de tamanho fixo ou variável separados por delimitador
- Registros de tamanho fixo ou variável
- Ordem que os registros estao inseriodos: aleátoria, ordem de chegada
- Tamanho do arquivo é maior, assim a página de disco armazena menos registros

Arquivo de índice
- Campos e registros de tamanho fixo, possuindo uma forma de organização mais fácil de se trabalhar
- Ordem em que os registros (entradas) estão inseridos: alfabética pela chave de busca (permite fazer busca binaria)
- Tamanho do arquivo é bem menor, assim a página de disco armazena mais registros
- Pode ser replicado em memória principal caso seja pequeno o suficiente

**A existência de índices não afeta a localização física dos registros dos arquivos de dados**

#### Pesquisa/Busca

Busca por campo indexado:

Toda pesquisa por chave começa pelo arquivo de índice, e depois vai para o de dados.

```
Busca binária no arquivo de índice até achar a chave de busca desejada
Se achou a chave de busca
	Então recupera o byte offset do registro desejado
	Acesso direto ao arquivo de dados usando o byte offser recuperado
	Recupera o registro e mostra o resultado ao usuário
Senão mostra "Registro não existente" ao usuário
```

Busca por campo não indexado:
```
Busca sequencial no arquivo de dados até achar o campo desejado
Se achou 
	Recupera o registro e mostra o resultado ao usuário
Senão mostra "Registro não existente" ao usuário
```

#### Inserção
```
Insere o novo registro no final do arquivo de dados ou em reaproveitamento de espaço, e insere a nova entrada no arquivo de índice de forma ordenada.
```

#### Remoção
Remove registros no índice devido às remoções no arquivo de dados.

```
Busca binária no arquivo de índice até achar a chave de busca desejada
Se achou a chave de busca
	Então recupera o byte offset do registro desejado
	Acesso direto ao arquivo de dados usando o byte offser recuperado
	Marca o registro do arquivo de dados como logicamente removido
	Remover a entrada do arquivo de índice
Senão mostra "Registro não existente" ao usuário
```

#### Atualização
Modifica registros no índice devido às modificações no arquivo de dados.
No registro de índices a técnica mais utilizada é a remoção seguida de inserção.
No arquivo de dados, se ao atualizar registro o tamanho utilizado acomodar o registro atualizado, apenas sobrescreve, senão marca como removido e insere no final.

```
Busca binária no arquivo de índice até achar a chave de busca desejada
Se achou a chave de busca
	Então recupera o byte offset do registro desejado
	Acesso direto ao arquivo de dados usando o byte offser recuperado
	Atualiza o registro no arquivo de dados
	Atualiza o arquivo de índice com a nova entrada
Senão mostra "Registro não existente" ao usuário
```

#### Destruição
Faz o índice desaparecer por inteiro.
Mesmo assim consigo realizar as operações anteriores, já que todos os dados estão no arquivos de dados. Apenas irá demorar mais ao fazer pesquisas por chave de busca.

#### Criação
Cria o índice, com seu registro de cabeçalho.
Caso seja baseado em um arquivo de dados já existente, é necessário percorrer e ler todo o arquivo de dados, para que as chaves sejam armazenadas e de forma ordenada.

#### Carregamento
Carrega o arquivo de índice na memória principal antes de usá-lo.

#### Reescrita
Atualiza o arquivo de índice em disco com base no arquivo de índice em memória principal, quando necessário.
Pode ter o campo *status* no registro de cabeçalho para indicar inconsistência nos índices, devido à queda de energia, travamento do programa de atualização, etc.

##### Status
Toda vez que faz um carregamento do arquivo que foi aberto para escrita, seta o status como inconsistente. Depois que todas as inserções/remoções/atualizações forem refletidas no arquivo de dados, seta o status como consistente.

```
Se status do arquivo de índice é inconsistente
	Então destrói o arquivo de índice
	Constrói o arquivo de índice
```