#!/bin/sh

# Lê o nome do arquivo como argumento
file_name="$1"

# Lê o conteúdo do arquivo da entrada padrão (stdin) até EOF
# O corpo da solicitação HTTP é passado para o script através do stdin
content=$(cat)

# Escreve o conteúdo no arquivo
echo "$content" >> "$file_name"

echo "HTTP/1.1 204 OK"
echo ""

# Inicializa uma variável para armazenar o conteúdo
content=""

# Lê o conteúdo linha por linha até EOF
while read -r line; do
    content="${content}${line}" # Concatena a linha ao conteúdo
done

# Verifica se o conteúdo não está vazio (EOF atingido)
if [ -n "$content" ]; then
    # Escreve o conteúdo no arquivo
    echo "$content" >> "$file_name"
    echo "HTTP/1.1 204 OK"
	echo ""
else
    message="Nenhum conteúdo foi recebido."
fi