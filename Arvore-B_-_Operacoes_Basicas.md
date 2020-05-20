# Árvore-B - Operações Básicas

## Páginas da Árvore-B

A estrutura de dados determina cada página de disco e pode ser implementada de diferentes formas.

A implementação adotada aqui será:
- Contador de ocupação: Número de chaves por página
- Chaves: Caracteres
- Ponteiros: Campos de referência para cada chave

## Operações Básicas: Pesquisa, Inserção e Remoção

São recursivas a partir do nó raiz e têm dois estágios de processamento, sendo o primeiro em páginas inteiras e o segundo dentro das páginas.

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
