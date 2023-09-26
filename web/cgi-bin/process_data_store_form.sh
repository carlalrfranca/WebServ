#!/bin/sh

echo "oi" > "$1"
# Lê o nome do arquivo como argumento
file_name="$1"

# Lê o conteúdo do arquivo da entrada padrão (stdin) até EOF
# O corpo da solicitação HTTP é passado para o script através do stdin
content=$(cat)

# Escreve o conteúdo no arquivo
echo "$content" >> "$file_name"

echo "HTTP/1.1 204 OK"
echo ""
