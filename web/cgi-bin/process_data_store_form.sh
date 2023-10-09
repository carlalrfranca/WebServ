#!/bin/sh

# Lê o nome do arquivo como argumento
file_name="$1"

# Lê o conteúdo do arquivo da entrada padrão (stdin) até EOF
# O corpo da solicitação HTTP é passado para o script através do stdin
# content=$(cat)

# Escreve o conteúdo no arquivo
# echo "$content" >> "$file_name"

# echo "HTTP/1.1 204 OK"
# echo ""

# Inicializa uma variável para armazenar o conteúdo
# content=""
# cat - > "$file_name"
# Lê o conteúdo linha por linha até EOF
# while read -r line; do
#     content="${content}${line}" # Concatena a linha ao conteúdo
# done
urldecode() {
    local url_encoded="${1//+/ }"
    printf '%s' "${url_encoded//%/\\x}"
}
# Lê o conteúdo linha por linha até EOF
# while IFS= read -r 1 char; do
#     content="${content}${char}" # Concatena o caractere ao conteúdo
# done

# Verifica se o conteúdo não está vazio (EOF atingido)
# if [ -n "$content" ]; then
    # Escreve o conteúdo no arquivo sem expansão de caracteres especiais
    # printf "%s" "$content" >> "$file_name"

#se o formato não for de imagem (jpg, jpeg, )

# Verifica se a variável de ambiente MY_VARIABLE está definida
if [ -n "$CONTENT_TYPE" ]; then
  cat - > "$file_name"
  touch content.txt
  content_type="$CONTENT_TYPE"
  location="$LOCATION"
  fileName="$FILE_NAME"
  # Verifica se o valor da variável contém a palavra "image"
  if [ -n "$CONTENT_TYPE" ] && echo "$CONTENT_TYPE" | grep -q "image"; then
  		echo "$file_name" > content.txt
  		echo "<!DOCTYPE html>"
		echo "<html lang='en'>"
		echo "<head>"
		echo "    <meta charset='UTF-8'>"
		echo "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>"
		echo "    <title>Upload de um arquivo</title>"
		echo "    <link rel='stylesheet' href='/assets/styles.css'>"
		echo "</head>"
		echo "<body>"
		echo "<header>"
		echo	"<div class='navMenu'>"
		echo		"<a href='/'>home</a>"
		echo		"<a href='/tour.html'>tour</a>"
		echo		"<a href='#'>tests</a>"
		echo	"<div class='dot'></div>"
		echo	"</div>"
		echo "</header>"
		echo "    <section class='section-container'>"
		echo "        <div class='upload-container'>"
		echo "            <h1>Olá! Vamos treinar um pouco com o POST?</h1>"
		echo "            <span class='upload-span'>"
		echo "                Esse projeto exige que seja feito alguns scripts CGI."
		echo "            </span>"
		echo "            <span class='upload-span'>"
		echo "                Nesse caso, Vamos mexer um pouco com a funcionalidade de postar um arquivo de imagem"
		echo "                no nosso servidor, e exibir essa imagem aqui."
		echo "            </span>"
		echo "        </div>"
		echo "        <div class='upload-container'>"
		echo "            <h2>LEMBRE-SE</h3>"
		echo "            <span class='upload-span'>Tudo tem limite</span>"
		echo "            <span class='upload-span'>A imagem submetida precisa ter até - megas. Mais do que isso e o servidor não irá aceitar!</span>"
		echo "        </div>"
		echo "        <div id='choose-image'>"
		echo "            <div>"
		echo "                <span class='upload-span'>Vamos lá? A imagem aparecerá nesse <span class='colored'>espaço destacado</span></span>"
		echo "            </div>"
		echo "            <div id='image'>"
		echo "                <img src=\"$location/$fileName\" alt='Imagem postada no servidor'>"
		echo "            </div>"
		echo "        </div>"
		echo "    </section>"
		echo "</body>"
		echo "</html>"
	else
		echo "<!DOCTYPE html>"
		echo "<html lang='en'>"
		echo "<head>"
		echo "    <meta charset='UTF-8'>"
		echo "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>"
		echo "    <title>Upload de um arquivo</title>"
		echo "    <link rel='stylesheet' href='/assets/styles.css'>"
		echo "</head>"
		echo "<body>"
		echo "    <section class='section-container'>"
		echo "        <div class='upload-container'>"
		echo "            <h1>Olá! Vamos treinar um pouco com o POST?</h1>"
		echo "            <span class='upload-span'>"
		echo "                Esse projeto exige que seja feito alguns scripts CGI."
		echo "            </span>"
		echo "            <span class='upload-span'>"
		echo "                Nesse caso, Vamos mexer um pouco com a funcionalidade de postar um arquivo de imagem"
		echo "                no nosso servidor, e exibir essa imagem aqui."
		echo "            </span>"
		echo "        </div>"
		echo "        <div class='upload-container'>"
		echo "            <h2>LEMBRE-SE</h3>"
		echo "            <span class='upload-span'>Tudo tem limite</span>"
		echo "            <span class='upload-span'>A imagem submetida precisa ter até - megas. Mais do que isso e o servidor não irá aceitar!</span>"
		echo "        </div>"
		echo "        <div id='choose-image'>"
		echo "            <p>Opa, precisa ser uma imagem para aparecer aqui... Senão fica estranho.</p>"
		echo "        </div>"
		echo "    </section>"
		echo "</body>"
		echo "</html>"
 	fi
else
	# são dados do formulário
	# Ler a entrada da variável encoded_data
	encoded_data=$(cat -)
	echo "Encoded: $encoded_data" >> "$file_name"
	# Decodificar os dados codificados em URL
	
	# echo "Decoded: $decoded_data" >> "$file_name"
	# Reformatar os dados e extrair os valores
	formatted_data=$(echo "$encoded_data" | tr '&' '\n')
	echo "Formatted: $formatted_data" >> "$file_name"
	# Extrair o valor de "nome"
	nome=$(echo "$formatted_data" | grep -o 'nome: [^ ]*' | sed 's/nome: //')

	# Extrair o valor de "mensagem" e decodificar os caracteres especiais
	mensagem_codificada=$(echo "$formatted_data" | grep -o 'mensagem: [^ ]*' | sed 's/mensagem: //')
	# mensagem_decodificada=$(echo -n "$mensagem_codificada" | jq -r -s @uri)

	# Imprimir os valores corrigidos
	echo "Nome: $nome" >> "$file_name"
	echo "Mensagem: $mensagem_codificada" >> "$file_name"

	# Redirecionar os dados decodificados para o arquivo "$file_name"
	# echo "$mensagem_corrigida" >> "$file_name"
  	echo "HTTP/1.1 204 OK"
	echo ""
fi

# echo "HTTP/1.1 204 OK"
# echo ""
# fi

# Verifica se o conteúdo não está vazio (EOF atingido)
# if [ -n "$content" ]; then
    # Escreve o conteúdo no arquivo
# echo "$content" >> "$file_name"
# echo "HTTP/1.1 204 OK"
# echo ""
# else
    # message="Nenhum conteúdo foi recebido."
# fi