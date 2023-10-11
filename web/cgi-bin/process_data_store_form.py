#!/usr/bin/env python3
import os
import sys
import urllib.parse
from unidecode import unidecode

import re

def remove_special_characters(file):
    # Use uma expressão regular para remover caracteres especiais, como [ e ]
    cleaned_file = re.sub(r'[^a-zA-Z0-9.]+', '', file)
    return cleaned_file

def correct_file_path(file_path, corrected_file_name):
    # Obtém o diretório onde o arquivo está localizado
    directory, fl = os.path.split(file_path)

    # Substitui o nome do arquivo pelo nome corrigido
    new_file_path = os.path.join(directory, corrected_file_name)

    return new_file_path

file_path = sys.argv[1]

content = ""
if "E_FILE" in os.environ:

	content_length = int(os.environ["CONTENT_LENGTH"])
	all_data = sys.stdin.buffer.read(content_length)
	content = all_data
else:
	all_data = sys.stdin.read()
	content = all_data

def urldecode(data):
    return urllib.parse.unquote_plus(data)

# Verifica se a variável de ambiente CONTENT_TYPE está definida

if "CONTENT_TYPE" in os.environ:
	content_type = os.environ["CONTENT_TYPE"]
	location = os.environ["LOCATION"]
	file_name_to_correct = os.environ["FILE_NAME"]
	file_name_to_unicode = file_name_to_correct.replace(' ', '_')
	file_name_to_unquote = unidecode(file_name_to_unicode)
	file_name_to_clean = urldecode(file_name_to_unquote)
	file_name = remove_special_characters(file_name_to_clean)
	file_path_upload_file = correct_file_path(file_path, file_name)

	# Verifica se o valor da variável contém a palavra "image"
	if "image" in content_type:
		with open(file_path_upload_file, "wb") as f:
			f.write(content)

		print("<!DOCTYPE html>")
		print("<html lang='en'>")
		print("<head>")
		print("    <meta charset='UTF-8'>")
		print("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>")
		print("    <title>Upload of a file</title>")
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
		print("<h1>Hello! Let's practice a bit with POST?</h1>")
		print("<span class='upload-span'>This project requires the creation of some CGI scripts.</span>")
		print("<span class='upload-span'>In this case, let's play around with the functionality of uploading an image file to our server and displaying that image here.</span>")
		print("</div>")
		print("<div class='upload-container'>")
		print("<h2>REMEMBER</h2>")
		print("<span class='upload-span'>Everything has a limit</span>")
		print("<span class='upload-span'>The submitted image must be up to - megabytes. More than that, and the server will not accept it!</span>")
		print("</div>")
		print("<div id='choose-image'>")
		print("<div>")
		print("<span class='upload-span'>Let's go? The image will appear in this <span class='colored'>highlighted space</span></span>")
		print("</div>")
		print("<div id='image'>")
		print(f"<img src='{location}/{file_name}' alt='submit-image'>")
		print("</div>")
		print("</div>")
		print("</section>")
		print("</body>")
		print("</html>")
	else:
		with open(file_path, "wb") as f:
			f.write(content)
		print("<!DOCTYPE html>")
		print("<html lang='en'>")
		print("<head>")
		print("    <meta charset='UTF-8'>")
		print("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>")
		print("    <title>Upload of a file</title>")
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
		print("<h1>Hello! Let's practice a bit with POST?</h1>")
		print("<span class='upload-span'>This project requires the creation of some CGI scripts.</span>")
		print("<span class='upload-span'>In this case, let's play around with the functionality of uploading an image file to our server and displaying that image here.</span>")
		print("</div>")
		print("<div class='upload-container'>")
		print("<h2>REMEMBER</h2>")
		print("<span class='upload-span'>Everything has a limit</span>")
		print("<span class='upload-span'>The submitted image must be up to - megabytes. More than that, and the server will not accept it!</span>")
		print("</div>")
		print("<div id='choose-image'>")
		print("<p>Wait, it needs to be an image to appear here... Otherwise it's weird.</p>")
		print(f"<a href='{location}/{file_name}' download='{file_name}'>Clique aqui para baixar</a>")
		print("</div>")
		print("</section>")
		print("</body>")
		print("</html>")
else:
	decoded_entrance = urllib.parse.unquote(content)
	pairs = decoded_entrance.split('&')
	with open(file_path, 'w') as file_to_write:
		for pair in pairs:
			if '=' in pair:
				key, value = pair.split('=')
				new_value_unquoted = urllib.parse.unquote(value)
				value_cleaned = urldecode(new_value_unquoted)
				new_value = value_cleaned.replace('+', ' ')
				file_to_write.write(f"{key}: {new_value}\n")
			else:
				key = urllib.parse.unquote(pair)
				file_to_write.write(f"{key}\n")
	print("HTTP/1.1 204 OK")
	print("")
