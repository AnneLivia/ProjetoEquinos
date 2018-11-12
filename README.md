# Utilização de técnicas de processamento digital de imagens para classificação equinos

## Introdução

Projeto em desenvolvimento para classificação de equinos pelos padrões de pelagens, utilizando técnicas de processamento digital de imagens.

## Desenvolvimento
O programa no momento se encontra em três etapas:
1. Implementação de técnicas de segmentação de imagens
    Estão sendo utilizados (Non Local Means e Erosão para remoção de ruídos e limiarização para a subdivisão da imagem)
2. Implementação de técnicas para identificação da coloração presente na pelagem dos equinos
    A identificação está sendo feita utilizando o sistema HSV.
3. Implementação de métodos para classificação de pelagens
    Estão sendo analisados no momento a cor predominante no equino como um todo e em parte dos membros.
    OBS: Caso seja utilizado cavalos de pelagem composta ou conjugada, o sistema pode se equivocar na classificação.
## Pré-requisitos

O desenvolvimento está sendo realizado no ambiente de desenvolvimento CodeBlocks 17.12 e, por enquanto, é indicado a utilização de imagens de equinos com background de cores estáticas, branco ou cinza.

### Bibliotecas e Linguagens de programação
* C++
* Biblioteca OpenCV 3.4.1
