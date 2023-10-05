#!/bin/bash

# Função para decodificar URL
urldecode() {
    local url_encoded="${1//+/ }"
    printf '%b' "${url_encoded//%/\\x}"
}

# Verifique se foi fornecido um nome de arquivo como argumento
if [ $# -eq 0 ]; then
  echo "Por favor, forneça um nome de arquivo como argumento."
  exit 1
fi

# Nome do arquivo é o primeiro argumento
file_name="$1"

while read -r line; do
    # if [[ "$line" == *"&"* ]]; then
        # Se a linha contiver '&', quebre-a em várias linhas separadas por '\n'
        # content="${content}${line//&/$'\n'}"
    # else
        # Caso contrário, apenas concatene a linha
        content="${content}${line}"
    # fi
		# Escreve o conteúdo no arquivo
		echo "$content" >> "$file_name"
done


# Seu código para processar os parâmetros da query string aqui
# Parseia os parâmetros da query string
# IFS="&"
# for param in $QUERY_STRING; do
#   IFS="="
#   set -- $param
#   decoded_key=$(urldecode "$1")
#   decoded_value=$(urldecode "$2")
#   key=$decoded_key
#   value=$decoded_value
 	# decodedkey=$(echo "$key" | sed 's/%40/@/g')
	# decodedvalue=$(echo "$value" | sed 's/%40/@/g')
	# echo "$decodedkey | $decodedvalue" >> "$nome_do_arquivo"
# done
# Redireciona a saída para o arquivo especificado (substitui o conteúdo existente)

# faz a saída da mensagem
echo "HTTP/1.1 204 OK"
echo ""
