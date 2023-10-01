#!/bin/bash

# Verifique se foi fornecido um argumento com o caminho da imagem
if [ $# -ne 1 ]; then
  echo "Uso: $0 <caminho_da_imagem>"
  exit 1
fi

# Caminho da imagem passado como argumento
caminho_da_imagem="$1"

# Verifique se o arquivo de imagem existe
if [ ! -f "$caminho_da_imagem" ]; then
  echo "Arquivo de imagem não encontrado."
  exit 1
fi

cat "$caminho_da_imagem"