# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    wordle.py                                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: qpupier <qpupier@student.42lyon.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/28 11:59:20 by yabokhar          #+#    #+#              #
#    Updated: 2026/01/07 14:06:11 by yabokhar         ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

from tkinter import *
from tkinter import ttk
from datetime import date
from hashlib import sha256
import random
import time
import os
DIR = os.path.dirname(os.path.abspath(__file__))
open(os.path.join(DIR, "words.txt"))

BACKGROUND = 'black'
COLOR_LETTERS = 'white'
COLOR_BOX = 'gray'
COLOR_CURRENT = 'lightblue'
COLOR_VALID = 'green'
COLOR_ALMOST = 'gold'
COLOR_KEYBOARD_WRONG = 'black'
NB_LETTERS = 5
NB_LINES = 6
TITLE_SIZE = 50
FONT = ("Roboto", 30, "bold")
FONT_BUTTON = ("Roboto", 20)
FONT_MESSAGES = ("Arial", int(TITLE_SIZE / 2))
BOX_SIZE = 80
PADX = 10
PADY = 20
KEYBOARD_NB_LETTERS_WIDTH = 10
KEYBOARD_SPACE = 10
KEYBOARD_PADDING = KEYBOARD_SPACE / 2
KEYBOARD_BOX_SIZE = BOX_SIZE - KEYBOARD_SPACE
BORDER = 42
PATH_NAME = 'words.txt'
FILE_NAME = '.secret_word.txt'

def	get_player_guess():
	player_guess = ""
	for letter in letters[current_line]:
		player_guess += letter.cget('text')
	return	player_guess

def	change_color(letter, x, y, style_name, color):
	if y < NB_LINES:
		boxes[y][x].configure(style = style_name)
		letters[y][x].configure(bg = color)
	for i in range(len(keyboard_labels)):
		if letter == keyboard_labels[i].cget('text'):
			if color == COLOR_VALID:
				keys[i].configure(style = "KeyboardValid.TFrame")
				keyboard_labels[i].configure(bg = COLOR_VALID, fg = COLOR_LETTERS)
			elif color == COLOR_ALMOST and keyboard_labels[i].cget('bg') != COLOR_VALID:
				keys[i].configure(style = "KeyboardAlmost.TFrame")
				keyboard_labels[i].configure(bg = COLOR_ALMOST, fg = COLOR_LETTERS)
			elif color == COLOR_BOX and keyboard_labels[i].cget('bg') != COLOR_VALID and keyboard_labels[i].cget('bg') != COLOR_ALMOST:
				keys[i].configure(style = "KeyboardWrong.TFrame")
				keyboard_labels[i].configure(bg = COLOR_KEYBOARD_WRONG, fg = COLOR_BOX)

def	check_valid_almost(player_guess, copy):
	for i in range(len(player_guess)):
		letter = player_guess[i]
		if letter == copy[i]:
			change_color(letter, i, current_line, 'Valid.TFrame', COLOR_VALID)
			if i == len(copy):
				copy = copy[:i]
			else:
				copy = copy[:i] + '-' + copy[(i + 1):]
			if i == len(player_guess):
				player_guess = player_guess[:i]
			else:
				player_guess = player_guess[:i] + '-' + player_guess[(i + 1):]
	for i in range(len(player_guess)):
		letter = player_guess[i]
		if letter != '-':
			if letter in copy:
				if i == len(player_guess):
					player_guess = player_guess[:i]
				else:
					player_guess = player_guess[:i] + '-' + player_guess[(i + 1):]
				change_color(letter, i, current_line, 'Almost.TFrame', COLOR_ALMOST)
				x = copy.find(letter)
				if x == len(copy):
					copy = copy[:x]
				else:
					copy = copy[:x] + '-' + copy[(x + 1):]
			else:
				change_color(letter, i, current_line, 'Wrong.TFrame', COLOR_BOX)

def game_i_guess():
	global secret_word

	player_guess = get_player_guess()
	if (secret_word == player_guess):
		j = 0
		while j < 5:
			change_color(player_guess[j], j, current_line, 'Valid.TFrame', COLOR_VALID)
			j += 1
		message['text'] = "Congratulations! You found the word!"
		return True
	check_valid_almost(player_guess, secret_word)
	message['text'] = ""
	return False

def	change_box(new_box, new_line, same_line):
	global current_box, current_line
	
	if same_line:
		change_color(False, current_box, current_line, 'Box.TFrame', COLOR_BOX)
	change_color(False, new_box, new_line, 'Current.TFrame', COLOR_CURRENT)
	current_box = new_box
	current_line = new_line

def	all_fill():
	for i in range(NB_LETTERS):
		if not letters[current_line][i].cget('text'):
			return False
	return True

def	write_letter(letter):
	letters[current_line][current_box]['text'] = letter
	if current_box < NB_LETTERS - 1:
		change_box(current_box + 1, current_line, True)

def	backspace():
	if not letters[current_line][current_box].cget('text') and current_box:
		change_box(current_box - 1, current_line, True)
	letters[current_line][current_box]['text'] = ""
def	enter():
	global get_keys
	
	if all_fill():
		if not get_player_guess().lower() in word_list and not get_player_guess() in word_list:
			message['text'] = "This word is not in our dictionnary"
		elif not game_i_guess():
			change_box(0, current_line + 1, False)
		else:
			get_keys = False
			replay['state'] = "active"
			if daily:
				share['state'] = "active"

def	key_pressed(event):
	global current_line, current_box, get_keys

	if event.keysym == "Escape":
		window.destroy()
	if not get_keys:
		return
	if 'a' <= event.char <= 'z' or 'A' <= event.char <= 'Z':
		write_letter(event.char.upper())
	elif event.keysym == "BackSpace":
		backspace()
	elif event.keysym == "Left" and current_box:
		change_box(current_box - 1, current_line, True)
	elif event.keysym == "Right" and current_box < NB_LETTERS - 1:
		change_box(current_box + 1, current_line, True)
	elif (event.keysym == "Return" or event.keysym == "KP_Enter"):
		enter()
	if current_line == NB_LINES:
		message['text'] = "You lost... the word was " + secret_word
		get_keys = False
		replay['state'] = "active"

def	config():
	s = ttk.Style()
	s.configure("Game.TFrame", background='red')
	s.configure("Line.TFrame", background='green')
	s.configure("Box.TFrame", background=COLOR_BOX, borderwidth = 3, relief="sunken")
	s.configure("Keyboard.TFrame", background=COLOR_BOX, borderwidth = 3, relief="raised")
	s.configure("KeyboardWrong.TFrame", background=COLOR_KEYBOARD_WRONG, borderwidth = 3, relief="sunken")
	s.configure("KeyboardAlmost.TFrame", background=COLOR_ALMOST, borderwidth = 3, relief="raised")
	s.configure("KeyboardValid.TFrame", background=COLOR_VALID, borderwidth = 3, relief="raised")
	s.configure("Current.TFrame", background=COLOR_CURRENT, borderwidth = 3, relief="sunken")
	s.configure("Valid.TFrame", background=COLOR_VALID, borderwidth = 3, relief="raised")
	s.configure("Almost.TFrame", background=COLOR_ALMOST, borderwidth = 3, relief="raised")
	s.configure("Wrong.TFrame", background=COLOR_BOX, borderwidth = 3, relief="raised")
	s.configure("Empty.TFrame", background=BACKGROUND)

def	create_key(position, line, last):
	if last:
		key = ttk.Frame(keyboard, width=KEYBOARD_BOX_SIZE * 1.5, height=KEYBOARD_BOX_SIZE, style="Keyboard.TFrame")
		key.grid(column = position, row = line, columnspan = 3, padx = KEYBOARD_PADDING, pady = KEYBOARD_PADDING)
	else:
		key = ttk.Frame(keyboard, width=KEYBOARD_BOX_SIZE, height=KEYBOARD_BOX_SIZE, style="Keyboard.TFrame")
		key.grid(column = position, row = line, columnspan = 2, padx = KEYBOARD_PADDING, pady = KEYBOARD_PADDING)
	key.grid_propagate(False)
	key.columnconfigure(0, weight=1)
	key.rowconfigure(0, weight=1)
	return key

def	create_keyboard():
	global	keyboard, keyboard_labels, keys

	keyboard = ttk.Frame(window, width = BOX_SIZE * KEYBOARD_NB_LETTERS_WIDTH, height = BOX_SIZE * 3, style="Empty.TFrame")
	keyboard.grid(column = 1, row = 4, columnspan = 3)
	keyboard.grid_propagate(False)
	keys = []
	for i in range(10):
		keys.append(create_key(i * 2, 0, False))
		if i < 9:
			keys.append(create_key(1 + (i * 2), 1, False))
			if i < 7:
				keys.append(create_key(3 + (i * 2), 2, False))
	keys.append(create_key(0, 2, True))
	keys.append(create_key(20 - 3, 2, True))
	keyboard_letters = "QAZWSXEDCRFVTGBYHNUJMIKOLP"
	keyboard_labels = []
	for i in range(len(keyboard_letters)):
		keyboard_label = Label(keys[i], text = keyboard_letters[i], font=FONT, bg=COLOR_BOX, fg = COLOR_LETTERS)
		keyboard_label.grid()
		keyboard_labels.append(keyboard_label)
		keyboard_label.bind('<ButtonPress>', lambda e, letter = keyboard_letters[i]: write_letter(letter))
		keys[i].bind('<ButtonPress>', lambda e, letter = keyboard_letters[i]: write_letter(letter))
	key_enter = Label(keys[-2], text = "ENTER", font=("Roboto", 15, "bold"), bg=COLOR_BOX, fg = COLOR_LETTERS)
	key_enter.grid()
	key_suppr = Label(keys[-1], text = "SUPPR", font=("Roboto", 15, "bold"), bg=COLOR_BOX, fg = COLOR_LETTERS)
	key_suppr.grid()
	key_enter.bind('<ButtonPress>', lambda e, letter = keyboard_letters[i]: enter())
	keys[-2].bind('<ButtonPress>', lambda e, letter = keyboard_letters[i]: enter())
	key_suppr.bind('<ButtonPress>', lambda e, letter = keyboard_letters[i]: backspace())
	keys[-1].bind('<ButtonPress>', lambda e, letter = keyboard_letters[i]: backspace())

def	create_game():
	global boxes, letters

	Label(window, text="Wordle", font=("Comicsansms", TITLE_SIZE), bg = BACKGROUND, fg = '#EAB63F').grid(column = 2, row = 0)
	game = ttk.Frame(window, style="Empty.TFrame")
	game.grid(column = 2, row = 1, rowspan = 2)
	boxes = []	
	for j in range(NB_LINES):
		line = []
		for i in range(NB_LETTERS):
			box = ttk.Frame(game, width=BOX_SIZE, height=BOX_SIZE, style="Box.TFrame")
			box.grid_propagate(False)
			box.columnconfigure(0, weight=1)
			box.rowconfigure(0, weight=1)
			line.append(box)
		boxes.append(line)
	letters = []
	for j in range(len(boxes)):
		line_letters = []
		for i in range(len(boxes[j])):
			boxes[j][i].grid(column = i + 1, row = j + 1, padx = PADX, pady = PADY)
			line_letters.append(Label(boxes[j][i], font=FONT, bg=COLOR_BOX, fg = COLOR_LETTERS))
			line_letters[i].grid()
		letters.append(line_letters)
	create_keyboard()

def	new_secret():
	try:
		file = open(FILE_NAME, 'w')
	except Exception as e:
		print("wordle:", e)
	secret_word = random.choice(word_list).upper()
	file.write(sha256(today.encode()).hexdigest() + "\n" + sha256(secret_word.encode()).hexdigest())
	return secret_word

def	read_file(file):
	try:
		words = file.read().splitlines()
	except:
		print("wordle: No words")
		quit()
	if not words or len(words) != 2 or len(words[0]) != 64 or len(words[1]) != 64:
		print("wordle: Corrupted file")
		quit()
	if words[0] != sha256(today.encode()).hexdigest():
		return new_secret()
	for word in word_list:
		if words[1] == sha256(word.upper().encode()).hexdigest():
			return word.upper()
	print("wordle: Corrupted file")
	quit()

def get_secret_word():
	try:
		file = open(FILE_NAME, 'r')
	except FileNotFoundError as e:
		return new_secret()
	except Exception as e:
		print("wordle:", e)
		quit()
	return (read_file(file))

def start():
	global get_keys

	change_box(0, 0, False)
	get_keys = True

def	restart():
	global secret_word, daily

	daily = False
	replay['state'] = "disabled"
	share['state'] = "disabled"
	secret_word = random.choice(word_list).upper()
	message['text'] = ""
	for j in range(min(current_line + 1, NB_LINES)):
		for i in range(NB_LETTERS):
			letters[j][i]['text'] = ""
			change_color(False, i, j, 'Box.TFrame', COLOR_BOX)
	for i in range(len(keyboard_labels)):
		keys[i].configure(style = "Keyboard.TFrame")
		keyboard_labels[i].configure(bg = COLOR_BOX, fg = COLOR_LETTERS)
	start()

def	sharing():
	window.clipboard_clear()
	window.clipboard_append(f"Wordle: {today} => {current_line + 1}/{NB_LINES}\n\n")
	for j in range(min(current_line + 1, NB_LINES)):
		for i in range(NB_LETTERS):
			style = boxes[j][i].cget('style')
			if style == "Valid.TFrame":
				window.clipboard_append("🟩")
			elif style == "Almost.TFrame":
				window.clipboard_append("🟨")
			else:
				window.clipboard_append("⬛")
		window.clipboard_append("\n")
	window.clipboard_append("\nby Yabokhar & Qpupier")
	window.update()
	message['text'] = "Results copied"

def	set_window():
	global	window, replay, message, share

	window = Tk()
	window.title("wordle")
	window.geometry("1700x1400")
	window.minsize(BOX_SIZE * KEYBOARD_NB_LETTERS_WIDTH + (BORDER * 2), (TITLE_SIZE * 2) + (BOX_SIZE + (PADY * 2)) * NB_LINES + (PADY * 2) + BOX_SIZE * 3 + BORDER)
	window.config(background=BACKGROUND)
	window.columnconfigure(0, weight = 1, minsize = BORDER)
	window.columnconfigure(4, weight = 1, minsize = BORDER)
	window.rowconfigure(0, weight = 1, minsize = TITLE_SIZE * 2)
	window.rowconfigure(3, weight = 1, minsize = PADY * 2)
	window.rowconfigure(5, weight = 1, minsize = BORDER)
	Button(window, text = "Quit", command = window.destroy, font = FONT_BUTTON).grid(column = 0, row = 1, columnspan = 2, rowspan = 2)
	replay = Button(window, text = "Replay", command = restart, state = 'disabled', font = FONT_BUTTON)
	replay.grid(column = 3, row = 1, columnspan = 2)
	share = Button(window, height = 3, text = "Copy results\nto clipboard\nfor sharing!", command = sharing, state = 'disabled', font = ("Roboto", 10))
	share.grid(column = 3, row = 2, columnspan = 2)
	message = Label(window, font = FONT_MESSAGES, fg = COLOR_LETTERS, bg = BACKGROUND)
	message.grid(column = 1, row = 3, columnspan = 3)

def load_words():
	words = False
	file = False
	try:
		file = open(PATH_NAME, 'r')
		words = file.read().splitlines()
	except Exception as e:
		print(f"wordle: {e}")
		quit()
	if not words:
		print("Word list is invalid")
		quit()
	for word in words:
		if len(word.strip()) != NB_LETTERS:
			print("Word list is invalid")
			quit()
	return words

if __name__ == "__main__":
	global word_list, today, secret_word, current_line, current_box, daily

	word_list = load_words()
	today = date.fromtimestamp(time.time()).strftime('%d/%m/%Y')
	secret_word = get_secret_word()
	set_window()
	config()
	create_game()
	current_line = 0
	current_line = 0
	daily = True
	start()
	window.bind("<Key>", key_pressed)
	window.mainloop()
	exit(0)
