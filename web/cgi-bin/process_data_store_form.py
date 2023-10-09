#!/usr/bin/env python3

import os
import sys
import urllib.parse

# Lê o nome do arquivo como argumento
file_path = sys.argv[1]

# Lê o conteúdo da solicitação HTTP
	# content_length = int(os.environ.get("CONTENT_LENGTH", 0)) # ??? nós não estamos passando CONTENT_LENGTH
	# content = sys.stdin.read(content_length) # ???
content = ""
if "E_ARQUIVO" in os.environ:
	# A variável está definida, o que sugere que dados binários são enviados
	content_length = int(os.environ["CONTENT_LENGTH"])
	dados = sys.stdin.buffer.read(content_length)
	content = dados
else:
	dados = sys.stdin.read()
	content = dados

# Função para decodificar dados codificados em URL
def urldecode(data):
    return urllib.parse.unquote_plus(data)

# Inicialize mensagem_decodificada como uma string vazia
mensagem_decodificada = ""    

# Verifica se a variável de ambiente CONTENT_TYPE está definida
if "CONTENT_TYPE" in os.environ:
    content_type = os.environ["CONTENT_TYPE"]
    location = os.environ["LOCATION"]
    file_name = os.environ["FILE_NAME"]

    # Verifica se o valor da variável contém a palavra "image"
    if "image" in content_type:
        with open(file_path, "wb") as f:
            f.write(content)

        print("<!DOCTYPE html>")
        print("<html lang='en'>")
        print("<head>")
        print("    <meta charset='UTF-8'>")
        print("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>")
        print("    <title>Upload de um arquivo</title>")
        print("    <link rel='stylesheet' href='/assets/styles.css'>")
        print("</head>")
        print("<body>")
        print("<header>")
        print("<div class='navMenu'>")
        print("<a href='/'>home</a>")
        print("<a href='/tour.html'>tour</a>")
        print("<a href='#'>tests</a>")
        print("<div class='dot'></div>")
        print("</div>")
        print("</header>")
        print("<section class='section-container'>")
        print("<div class='upload-container'>")
        print("<h1>Olá! Vamos treinar um pouco com o POST?</h1>")
        print("<span class='upload-span'>Esse projeto exige que seja feito alguns scripts CGI.</span>")
        print("<span class='upload-span'>Nesse caso, Vamos mexer um pouco com a funcionalidade de postar um arquivo de imagem no nosso servidor, e exibir essa imagem aqui.</span>")
        print("</div>")
        print("<div class='upload-container'>")
        print("<h2>LEMBRE-SE</h2>")
        print("<span class='upload-span'>Tudo tem limite</span>")
        print("<span class='upload-span'>A imagem submetida precisa ter até - megas. Mais do que isso e o servidor não irá aceitar!</span>")
        print("</div>")
        print("<div id='choose-image'>")
        print("<div>")
        print("<span class='upload-span'>Vamos lá? A imagem aparecerá nesse <span class='colored'>espaço destacado</span></span>")
        print("</div>")
        print("<div id='image'>")
        print(f"<img src='{location}/{file_name}' alt='Imagem postada no servidor'>")
        print("</div>")
        print("</div>")
        print("</section>")
        print("</body>")
        print("</html>")
    else:
        print("Content-Type: text/html")
        print("")
        print("<!DOCTYPE html>")
        print("<html lang='en'>")
        print("<head>")
        print("    <meta charset='UTF-8'>")
        print("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>")
        print("    <title>Upload de um arquivo</title>")
        print("    <link rel='stylesheet' href='/assets/styles.css'>")
        print("</head>")
        print("<body>")
        print("<section class='section-container'>")
        print("<div class='upload-container'>")
        print("<h1>Olá! Vamos treinar um pouco com o POST?</h1>")
        print("<span class='upload-span'>Esse projeto exige que seja feito alguns scripts CGI.</span>")
        print("<span class='upload-span'>Nesse caso, Vamos mexer um pouco com a funcionalidade de postar um arquivo de imagem no nosso servidor, e exibir essa imagem aqui.</span>")
        print("</div>")
        print("<div class='upload-container'>")
        print("<h2>LEMBRE-SE</h2>")
        print("<span class='upload-span'>Tudo tem limite</span>")
        print("<span class='upload-span'>A imagem submetida precisa ter até - megas. Mais do que isso e o servidor não irá aceitar!</span>")
        print("</div>")
        print("<div id='choose-image'>")
        print("<p>Opa, precisa ser uma imagem para aparecer aqui... Senão fica estranho.</p>")
        print("</div>")
        print("</section>")
        print("</body>")
        print("</html>")
else:
    # São dados do formulário
    # Lê a entrada do pipe (stdin)
	# entrada = sys.stdin.read().strip()
	decoded_entrada = urllib.parse.unquote(content)
	# Divide a string em pares chave-valor usando '&' como separador
	pares = decoded_entrada.split('&')
	
	# Abre um arquivo para escrita
	with open(file_path, 'w') as arquivo:
		# Itera pelos pares e escreve no formato desejado no arquivo
		for par in pares:
			chave, valor = par.split('=')
			arquivo.write(f"{chave}: {valor}\n")
	# Decodificar os dados codificados em URL
    # decoded_data = urldecode(content)

    # # Reformatar os dados e extrair os valores
    # formatted_data = decoded_data.split('&')

    # # Extrair o valor de "nome"
    # nome = None
    # for item in formatted_data:
    #     if item.startswith('nome='):
    #         nome = item.split('=')[1]

    # # Extrair o valor de "mensagem" e decodificar os caracteres especiais
    # mensagem_codificada = ""
    # for item in formatted_data:
    #     if item.startswith('mensagem='):
    #         mensagem_codificada = item.split('=')[1]
    #         mensagem_decodificada = urldecode(mensagem_codificada)

    # # Imprimir os valores corrigidos
    # with open(file_name, "w") as f:
    #     if nome:
    #         f.write(f"Nome: {nome}\n")
    #     if mensagem_decodificada:
    #         f.write(f"Mensagem: {mensagem_decodificada}\n")

	print("HTTP/1.1 204 OK")
	print("")
