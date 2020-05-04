# Compactação e Reuso de Espaço

- [Manipulação de Dados](https://github.com/yasmin-araujo/Arquivos/blob/master/Compactacao_e_Reuso_de_Espaco.md#manipula%C3%A7%C3%A3o-de-dados)
- [Compactação e Reuso](https://github.com/yasmin-araujo/Arquivos/blob/master/Compactacao_e_Reuso_de_Espaco.md#compacta%C3%A7%C3%A3o-e-reuso)
- [Abordagem Estática](https://github.com/yasmin-araujo/Arquivos/blob/master/Compactacao_e_Reuso_de_Espaco.md#abordagem-est%C3%A1tica)
- [Abordagem Dinâmica](https://github.com/yasmin-araujo/Arquivos/blob/master/Compactacao_e_Reuso_de_Espaco.md#abordagem-din%C3%A2mica)

## Manipulação de Dados
### Operações básicas

 **Adição de registros**: Relativamente simples
 
 **Remoção de registros**: Deve-se posteriormente reutilizar o espaço do registro removido
 
 **Atualização de registros**: Eliminação e adição de um registro

## Compactação e Reuso
### Compactação
Busca por regiões do arquivo que não contém dados e recupera os espaços perdidos.

### Reuso
Insere dados nos espaços perdidos.

## Abordagem Estática

**Vantagens**: Abordagem mais simples e fácil de ser implementada. Funciona bem independente de ser com registros de tamanho fixo ou variável.

**Desvantagens**: Alto custo de armazenamento.

A abordagem estática primeiro realiza a remoção lógica, e posteriormente, a física. Ao remover um registro gera espaço sem dados úteis, sendo necessário um programa "compactador", que reagrupa os registros e diminui o tamanho do arquivo.

A remoção lógica é utilizada para o reconhecimento de registros removidos, atribuindo um valor para um campo do registro e usando um campo extra.

Abordagem mais recomendada para quando é necessária pouca remoção de registros.

**Para chegar em determinado registro : `RNN * sizeof(registro)`**



### Busca
Buscas desconsideram os registros marcados como removidos.

```
Determinar o byteoffset = RRN * sizeof(registro)
Fazer um fseek para esse byteoffset
Se o registro existe
	Então ler o sizeof(registro)
	Se não está marcado como removido (*)
		Mostrar a resposta (o que foi lido) para o usuário
	Senão mostrar "Registro não existe"	
Senão mostrar "Registro não existe"
```


### Remoção Lógica
Registros removidos são marcados, porém não são reaproveitados.
O primeiro caracter do registro a ser removido é marcado com um *.

```
Determinar o byteoffset = RRN * sizeof(reg)
Fazer um fseek para esse byteoffset
Se o registro existe
    Então escrever * no primeiro byte
    Mostrar para o usuário "Registro excluído"
Senão mostrar "Registro nao existe"
```

### Inserção
Novas inserções de registros na abordagem estática são realizadas sempre no final do arquivo. Não são realizados testes, sendo assim a inserção uma operação muito simples.

```
Fazer um fseek para o final do arquivo
Insiro novo registro
```

### Compactação
Programa é executado para reconstruir o arquivo, descartando todos os registros removidos. Para isto, cria novo arquivo apenas com os registros que não foram marcados.
A compactação deve ser eficiente, uma vez que estamos lidando com arquivos de dados. Desta forma são utilizados 2 arquivos diferentes ao mesmo tempo.
O processo acaba realizando desfragmentação, já que coloca todos os arquivos em páginas sequenciais no disco, aumentando a eficiência de leitura desses dados posteriormente.

```
Abrir arqX para leitura
Abrir arqTemp para escrita
Enquanto não terminou arqX
	Ler um registro de arqX
	Se registro não estiver marcado como removido
		Então escrever registro no arqTemp
Fim enquanto
Fechar arqX
Remover arqX
Fechar arqTemp
Renomear arqTemp para arqX
```


## Abordagem Dinâmica
Indicada para aplicações interativas que acessam arquivos altamente voláteis.

### Registros de Tamanho Fixo
Lista encadeada de registros eliminados implementada como **pilha**.

Características:

- Lista: composta pelos RRNs dos registros marcados como logicamente removidos
- Cabeça da lista (topo da pilha): armazenada no registro de cabeçalho do arquivo
- Inserção e reuso do espaço: sempre ocorrem no inicio da pilha

#### Busca
Igual busca da [abordagem estática](https://github.com/yasmin-araujo/Arquivos/blob/master/Compactacao_e_Reuso_de_Espaco.md#busca).

#### Remoção
```
Determinar o byteoffset = RRN * sizeof(registro)
Fazer um fseek para esse byteoffset
Se o registro existe
    Então escrever * no primeiro byte
    Empilhar RRN do registro removido 
    ( Escrever o valor do topo da pilha nos próximos 4 bytes 
    e escrever no topo o valor do RRN desse registro )
    Mostrar p o usuário "Registro removido"
Senão "Registro não existe"
```

#### Inserção
```
Se a pilha estiver vazia (valor: -1)
    Então fazer um fseek para o final do arquivo
    Inserir novo registro
Senão offset = RRN do topo * sizeof(registro)
    Fazer fseek para esse byteoffset
    Desempilhar o RRN do registro que havia sido removido
    ( Atualizar o topo com o valor do encadeamento da lista e
    inserir o novo registro a partir do offset )
	
```

### Registros de Tamanho Variável
Lista encadeada de registros eliminados não tendo uma implementação específica.

Características:

- Lista: composta pelos byte offsets dos registros marcados como logicamente removidos
- Cabeça da lista (topo da lista): armazenada no registro de cabeçalho do arquivo
- Inserção: sempre ocorre no inicio da lista (como em uma pilha)

Observações:
- É necessário guardar também o tamanho do registro
- Cada registro deve ter pelo menos 9 bytes

#### Busca
A busca é feita byte a byte, contando o número de delimitadores de registro quando necessário achar algum em específico.

#### Remoção

```
1. Encontrar o registro desejado pelo byte offset 
(buscar -> deve existir uma rotina de busca)
2. Marcar o registro como logicamente removido (primeiro byte com um *)
3. Escrever o tamanho do registro nos próximos 
quatro bytes (tamanho inclui o delimitador)
4. Escrever o topo da lista nos próximos 4 bytes do registro
5. Atualizar o topo da lista com o byte offset do registro removido
```

#### Estratégias de Alocação
Como os registros tem tamanhos diferentes, não dá para facilmente reutilizar o espaço. Portanto o registro deve caber no espaço que existe disponível. Para isto temos 3 técnicas de alocação:

##### Estratégia First-Fit
Utiliza o primeiro espaço que servir.
Recomendado caso sistema esteja com muita [fragmentação interna](https://github.com/yasmin-araujo/Arquivos/blob/master/Compactacao_e_Reuso_de_Espaco.md#).

```
Se existe registro logicamente removido (topo != -1)
	Então percorrer a lista até encontrar um registro no qual o novo caiba
	Atualizar a lista com o encadeamento que está no registro
	Inserir o registro
Senão inserir o registro no final do arquivo
```

##### Estratégia Best-Fit
Escolhe o espaço mais justo possível, sobrando a menor fragmentação interna.
Recomendado caso sistema esteja com muita [fragmentação interna](https://github.com/yasmin-araujo/Arquivos/blob/master/Compactacao_e_Reuso_de_Espaco.md#).

```
Se existe registro logicamente removido (topo != -1)
	Então percorrer toda a lista para encontrar a "menor sobra"
	Atualizar lista com o encadeamento que está no registro
	Inserir o registro
Senão inserir o registro no final do arquivo
```

##### Estratégia Worst-Fit
Escolhe o maior espaço possível, sobrando a maior fragmentação interna.
Oferece maior possibilidade de tratamento de fragmentação interna (reaproveitamento de espaço), já que pode sobrar um espaço grande o suficiente para inserir um novo registro.
Recomendado caso sistema esteja com muita [fragmentação externa](https://github.com/yasmin-araujo/Arquivos/blob/master/Compactacao_e_Reuso_de_Espaco.md#).

```
Se existe registro logicamente removido (topo != -1)
	Então percorrer toda a lista para encontrar a "maior sobra"
	Atualizar a lista com o encadeamento que está no registro
	Inserir o registro
Senão inserir o registro no final do arquivo
```

#### Fragmentação Interna
Espaço que sobra dentro de um registro.
Pode ocorrer com qualquer estratégia de alocação.

**Solução**: Colocar o espaço que sobrou na lista encadeada como um registro eliminado.

#### Fragmentação Externa
Espaço que sobrou dentro de um registro, e foi colocado na lista encadeada como um registro eliminado, é tão pequeno que não pode armazenar nenhum dado.
Pode acontecer com qualquer estratégia de alocação.

**Solução**: Coalescimento - Junção de espaços vazios adjacentes, combinando 2 registros removidos e juntando em um só maior.
**Dificuldade**: Técnica só pode ser aplicada se os 2 registros estiverem dentro da mesma págica de disco.