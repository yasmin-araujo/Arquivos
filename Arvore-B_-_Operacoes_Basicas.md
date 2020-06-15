# Árvore-B - Operações Básicas

## Páginas da Árvore-B

A estrutura de dados determina cada página de disco e pode ser implementada de diferentes formas.

A implementação adotada aqui será:
- Contador de ocupação: Número de chaves por página
- Chaves: Caracteres
- Ponteiros: Campos de referência para cada chave

## Operações Básicas: Pesquisa, Inserção e Remoção

São recursivas a partir do nó raiz e têm dois estágios de processamento, sendo o primeiro em páginas inteiras e o segundo dentro das páginas.

- [Pesquisa](./Arvore-B_-_Operacoes_Basicas.md#pesquisa)
- [Inserção]()
- [Remoção](./Arvore-B_-_Operacoes_Basicas.md#remoção)

### Pesquisa
```
Se RRN da página corrente é -1
	Então retorna que não encontrou
Senão pesquisa a página corrente procurando a chave de busca
	Se encontrou
		Então retorna a referência apropriada para o registro no arquivo de dados
	Senão procura a chave de busca no nó filho usando o RRN para a página apropriada
	Fim se
Fim se
```

### Remoção

Toda remoção começa com uma busca, e somente pode ser feita em um nó folha.

**Underflow**: Ocorre quando o número de chaves em uma página fica abaixo do número mínimo de chaves permitido pela árvore-B.

#### Algoritmo
```
1. Se a chave a ser removida não estiver em um nó folha, troque-a com sua sucessora imediata, que está em um nó folha
2. Remova a chave
3. Se o nó satisfaz o número mínimo de chaves, nenhuma ação adicional requerida
4. Caso ocorra underflow, verificar número de chaves nos nós irmão adjacentes
    a. Se algum nó irmão adjacente possui mais do que o número mínimo de chaves, aplique a redistribuição. 
    b. Senão aplique a concatenação
5. Se ocorreu concatenação, repetir passos 3 a 5 no nó pai
6. Se a últim achave da raiz for removida, a altura da árvore diminui   
```

#### Casos

1. [Remoção em nó folha sem underflow](./Arvore-B_-_Operacoes_Basicas.md#caso-1)
2. [Remoção em nó não folha](./Arvore-B_-_Operacoes_Basicas.md#caso-2)
3. [Remoção em nó folha com underflow](./Arvore-B_-_Operacoes_Basicas.md#caso-3)
4. [Remoção em nó folha com underflow e sem redistribuição](./Arvore-B_-_Operacoes_Basicas.md#caso-4)
5. [Underflow em nó pai após remoção em nó filho](./Arvore-B_-_Operacoes_Basicas.md#caso-5)
6. [Diminuição da altura da árvore](./Arvore-B_-_Operacoes_Basicas.md#caso-6)

#### Caso 1

A **remoção de uma chave em um nó folha, sem causar *underflow***, é a situação mais simples possível. É necessário apenas eliminar a chave e rearranjar as chaves remanescentes dentro da página para fechar o espaço liberado.

#### Caso 2

Na **remoção em nó não folha** a chave não pode ser removida diretamente. Assim, ela é trocada com uma chave presente no nó folha, e então retirada. Deve-se garantir a taxa de ocupação *[m/2 - 1, m - 1]*.

#### Caso 3

Na **remoção de uma chave em um nó, causando underflow,** será necessária uma redistribuição.
A **redistribuição** é feita por meio de uma procura por uma página irmã (que possui o mesmo pai) adjacente que contenha mais chaves do que o mínimo. As chaves são então redistribuídas entre as páginas, e a chave separadora é reacomodada, modificando o conteúdo do nó pai.

#### Caso 4

Na **remoção de uma chave em um nó, causando *underflow*, e a redistribuição não podendo ser aplicada**, deve ser realizada uma concatenação.
A **concatenação** forma uma nova página por meio de combinação. O conteúdo do nó que sofreu *underflow* é combinado com o conteúdo de um nó irmão adjacente e a chave separadora do pai. Caso ocorra *underflow* no nó pai, ele deve ser tratado.
Com a concatenação o número de nós da árvore sempre diminui.

#### Caso 5

No caso de ***underflow* no nó pai causado pela remoção de uma chave em um nó filho**, é necessário utilizar *redistribuição* ou *concatenação*, dependendo da quantidade de chaves que a página irmã adjacente contém.

#### Caso 6

Na **diminuição da altura da árvore** o nó raiz possui uma única chave, a qual é absorvida pela concatenação de seus nós filhos. 
Nesta situação, é necessário eliminar a raiz antiga e tornar o nó resultante da concatenação dos nós filhos, a nova raiz da árvore.
