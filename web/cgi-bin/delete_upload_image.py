import os
import sys

if len(sys.argv) != 2:
	print("Usage: python script.py <directory path>")
	sys.exit(1)

dir_path = sys.argv[1]

if os.path.isdir(dir_path):
	files = os.listdir(dir_path)
	html = f"""

<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Delete some file</title>
	<link rel="stylesheet" href="../assets/styles.css">
</head>
<body>
	<section class="main-section">
		<div class="navMenu">
			<a href="/">home</a>
			<a href="/about.html">about</a>
			<a href="/tour.html">tour</a>
			<div class="dot"></div>
		</div>
	</section>
	<section class="section-container">
		<div class="section-button">
			<h1>Content in Directory</h1>
			<form method="POST" action="/images/delete">
				<input type="hidden" name="_method" value="DELETE">
				<ul>
"""
	for file in files:
		html += f"                    <li>{file} <button class='delete-button' type=\"submit\" name=\"arquivoSelecionado\" value=\"{file}\">Excluir</button></li>"

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

	print(html)
