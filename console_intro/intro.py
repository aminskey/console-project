import cv2
import pygame

from pygame.locals import *

def return_frames(surface, file="backgrounds/intro_anim.gif"):
	video = cv2.VideoCapture("backgrounds/intro_anim.gif")
	shape = video.read()[1].shape[1::-1]

	ret = True
	slides = []

	while True:
		ret, frame = video.read()
		if not ret:
			break

		img = pygame.image.frombuffer(frame, shape, "BGR")
		img = pygame.transform.scale(img, surface.get_size())
		slides.append(img)

	return slides

pygame.init()

screen = pygame.display.set_mode((500, 310), FULLSCREEN | SCALED)
pygame.display.set_caption("Intro")

intro_gif = return_frames(screen)
logo = pygame.transform.scale(pygame.image.load("console_logo.png"), (screen.get_width(), screen.get_height()//3))
logoRect = logo.get_rect()
logoRect.center = screen.get_rect().center

shade = pygame.Surface(screen.get_size())
shade.fill((0, 0, 0))

FPS = 20
clock = pygame.time.Clock()

pygame.mixer.music.load("songs/opening.ogg")
pygame.mixer.music.play()

gifIndex = 0
alphaVal = 0

counter = 0

while True:
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			pygame.quit()
			exit()

	if alphaVal < 255:
		alphaVal += 4
	if alphaVal >= 255:
		counter += 2

	if counter >= 355:
		pygame.quit()
		exit()

	shade.set_alpha(counter)
	logo.set_alpha(alphaVal)

	gifIndex += 1
	if gifIndex > len(intro_gif) - 1:
		gifIndex = 0

	bg = intro_gif[gifIndex]

	screen.blit(bg, (0, 0))
	screen.blit(shade, (0, 0))
	screen.blit(logo, logoRect)


	pygame.display.update()
	clock.tick(FPS)
