# Árvore-B - Definição
Método genérico para o armazenamento e a recuperação de dados voltado para arquivos volumosos, que não cabem em ram, proporcionando rápido acesso aos dados e um custo mínimo de overhead.

Em sistemas de banco de dados é uma organização padrão para indexação desde 1979, sendo amplamente utilizada e tendo variantes.

Tem como característica geral manter um índice para um arquivo de acesso aleatório altamente dinâmico, sendo que o índice possui *n* elementos *(x,a)* de tamanho fixo, sendo *x* a chave de busca e *a* o campo de referência.

Por ter um índice estremamente volumoso e apenas uma parcela dele podendo ser carregada em memória principal, são utilizadas operações baseadas em discos.
Essas operações fazem com que exista a preocupação com paginação (páginas de disco), as quais são unidades de transferência de dados entre a RAM e o disco, sendo transferidos 4K bytes.

## Características

O objetivo da árbore-B é ser totalmente projetada para acessar o menor número de páginas de disco necessário. Deste modo, ela é balanceada.

Segue a técnica de *Bottom-up* para sua criação em disco, ou seja, indo dos nós folhas para a raiz. As chaves na raiz da árvore emergem naturalmente.

O nó da árvore é igual ao tamanho da página de disco. Ou seja, dentro de uma página de disco é possível colocar inúmeras chaves de busca, tendo uma sequencia ordenada de chaves e ponteiros. Tais ponteiros são na verdade endereçamentos (byte offset/RRN).

**Ordem**: Número máximo de descendentes/filhos que pode ser armazenado em um nó, tendo um máximo de *ordem-1* chaves de busca. Em nós folhas (nível mais baixo de descendentes de um nó) não existem filhos, sendo os ponteiros nulos.

Se tamanho dos registros for 128 bytes e tamanho da página for 4K, então a cada leitura estará lendo 32 registros por página.

O número de descendentes de um nível da árvore-B é igual ao número de chaves contidas no nível em questão e em todos os níveis acima + 1.

## Estrutura Lógica de um Nó

Nó: Página de disco
Campos e registros têm tamanho fixo, portanto é utilizado o RRN.
Os campos com o RRN são os campos de endereçamento que levam para os filhos do nó.
```
  _____________________________________________________
 |     |         |     |         |     |         |     |
 | RRN | chave 1 | RRN | chave 2 | RRN | chave 3 | RRN |
 |_____|_________|_____|_________|_____|_________|_____|
    v	            v               v               v
< chave 1       > chave 1       > chave 2       > chave 3
                < chave 2       < chave 3
```

## Definição Formal

Considerando uma Árvore-B com ordem *m*:

- As chaves de busca são ordenadas dentro de cada página;
- Cada página possui um máximo de *m* descendentes;
- Cada página, exceto a raiz e as folhas, possui no mínimo *[m/2]* filhos, sendo assim a taxa de ocupação é de no mínimo 50%;
- A raiz possui pelo menos 2 filhos, a menos que seja um nó folha;
- Todas as folhas aparecem no mesmo nível (o último);
- Uma página interna com *k* filhos contém *k-1* chaves de busca;
- Uma folha possui no mínimo *[m/2]-1* chaves e no máximo *m-1* chaves (corresponde a taxa de ocupação).

## Complexidade (Pior caso)

A complexidade da árvore no pior caso acontece quando se tem a menor taxa de ocupação, e é dada em termos do número de acessos a disco, uma vez que é feito um acesso a disco por altura da árvore.

No pior caso a árvore apresentará o seguinte número de descendentes em cada nível:
- Nível 1: 2 (raiz)
- Nível 2: 2 * [m/2]
- Nível 3: 2 * [m/2] * [m/2] = 2 [m/2]<sup>2</sup>
- Nível 4: 2 * [m/2] * [m/2] * [m/2] = 2 [m/2]<sup>3</sup>
- ...
- Nível d: 2 [m/2]<sup>d-1</sup>

Assim, a complexidade fica como **N + 1 $\geq$ 2 * [m/2] <sup>d-1</sup>** e **d $\leq$ 1 + log<sub>[m/2]</sub>((N + 1)/2)**, sendo *N + 1* o número de chaves e descendentes e *d* a profundidade da árvore.
