# Sistema para identificação automática de pelagens dos equinos utilizando técnicas de processamento digital de imagens e visão computacional.

## Introdução

Projeto em desenvolvimento para classificação automática de equinos através da análise dos padrões de pelagens, utilizando técnicas de processamento digital de imagens e visão computacional para o aprimoramento e extração das informações

## Desenvolvimento
O programa no momento se encontra em três etapas:
1. Implementação de técnicas de segmentação de imagens
    * Estão sendo utilizados (Non Local Means e Erosão para remoção de ruídos e limiarização para a subdivisão da imagem)
2. Implementação de técnicas para identificação da coloração presente na pelagem dos equinos
    * A identificação está sendo feita utilizando o sistema HSV.
3. Implementação de métodos para classificação de pelagens
    * Estão sendo feitos testes para classificação de duas pelagens, Alazã e Castanho
    * Estão sendo analisados no momento a cor predominante no equino como um todo e em parte dos membros.
    * OBS: Caso seja utilizado cavalos de pelagem composta ou conjugada, o sistema pode se equivocar na classificação.
## Pré-requisitos

O desenvolvimento está sendo realizado no ambiente de desenvolvimento CodeBlocks 17.12 e, por enquanto, é indicado a utilização de imagens de equinos com background de cores estáticas, branco ou cinza.

### Bibliotecas e Linguagens de programação
* C++
* Biblioteca OpenCV 4.0.1

## Desenvolvedora
Anne Livia da F. Macedo (UFPA - Castanhal)
