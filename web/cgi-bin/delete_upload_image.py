import os
import sys

# Verifique se foi fornecido um argumento com o nome do diretório
if len(sys.argv) != 2:
	print("Uso: python script.py <caminho_do_diretorio>")
	sys.exit(1)

# Caminho do diretório passado como argumento
caminho_do_diretorio = sys.argv[1]

# Verifica se o diretório existe
if os.path.isdir(caminho_do_diretorio):
	# Lista os arquivos no diretório
	arquivos = os.listdir(caminho_do_diretorio)

	# Cria o conteúdo HTML
	html = f"""
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Delete - Enviar Solicitação</title>
	<link rel="stylesheet" href="../assets/styles.css">
</head>
<body>
	<section class="main-section">
		<div class="navMenu">
			<a href="/">home</a>
			<a href="/about.html">about</a>
			<a href="/delete.html">delete</a>
			<a href="/tour.html">tour</a>
			<a href="#">tests</a>
			<div class="dot"></div>
		</div>
	</section>
	<section class="section-container">
		<div class="section-button">
			<h1>Arquivos no Diretório</h1>
			<form method="POST" action="/images/delete">
				<input type="hidden" name="_method" value="DELETE"> <!-- form com campo oculto para especificar o método DELETE -->
				<ul>
"""
	# Adicione opções para cada arquivo no diretório
	for arquivo in arquivos:
		html += f"                    <li>{arquivo} <button type=\"submit\" name=\"arquivoSelecionado\" value=\"{arquivo}\">Excluir</button></li>"

	html += """
				</ul>
			</form>
		</div>
	</section>
	<footer>
		<div class=\"contact\">
			<a href=\"https://github.com/usuario-github-lfranca\">lfranca-</a>
			<a href=\"https://github.com/seu-usuario-github\">cleticia</a>
		</div>
		<h3 class=\"title-footer\">&copy; 2023 WebServ</h3>
	</footer>
</body>
</html>
"""

	# Imprime o conteúdo HTML modificado
	print(html)
