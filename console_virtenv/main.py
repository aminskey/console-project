import pygame
import subprocess
import platform

from pygame.locals import *
from cv2 import VideoCapture
from os import chdir, listdir, path, getcwd

pygame.init()

res = (800, 500)
FPS = 60

screen = pygame.display.set_mode(res, FULLSCREEN | SCALED)
clock = pygame.time.Clock()

rootpath = getcwd()

gamesGroup = pygame.sprite.Group()

filter = "\n\t_#*"

MAX_FRAMES = 2000

class Game():
        def __init__(self, dir, confFile=".gameconf", window=screen):

                self.conf_data = {
                        "game-file": None,
                        "preview-video": None,
                        "preview-bg": None,
                        "rom-img": None,
                        "name": None,
                        "description-file": None
                }

                self.wd = dir
                self.conf_file = f"{self.wd}/{confFile}"
                self.readData()

                self.img = pygame.image.load(f"{self.wd}/{self.conf_data['rom-img']}")
                self.video = f"{self.wd}/{self.conf_data['preview-video']}"
                self.video_bg = f"{self.wd}/{self.conf_data['preview-bg']}"

        def readData(self):
                with open(f"{getcwd()}/{self.conf_file}", "r") as f:
                        data = f.readlines()
                        f.close()

                for line in data:
                        newline = line.strip("\n").split("=")
                        self.conf_data[newline[0]] = newline[1]

class GameWindow(pygame.sprite.Sprite):
        def __init__(self, game):
                super().__init__()

                size = (res[1] - (res[1] * 1//2) - 50)
                self.image = pygame.Surface((size - 50, size - 30))
                self.image.fill((176, 176, 176))

                self.rect = self.image.get_rect()

                self.game = game
                self.cover = pygame.transform.scale(self.game.img, (self.image.get_width() - 10, self.image.get_height() * 2//3))
                self.cover_rect = self.cover.get_rect()
                self.cover_rect.topleft = (5, 5)

                self.image.blit(self.cover, self.cover_rect)

class Text(pygame.sprite.Sprite):
        def __init__(self, msg, font, color, pos=(0, 0)):
                super().__init__()

                self.image = font.render(msg, None, color)
                self.rect = self.image.get_rect()
                self.rect.center = pos



def readFrames(file, window=screen):
        video = VideoCapture(file)
        ret = True
        shape = video.read()[1].shape[1::-1]

        frames = []

        counter = 0

        while ret:
                ret, frame = video.read()
                if not ret or counter > MAX_FRAMES:
                        break

                img = pygame.transform.scale(pygame.image.frombuffer(frame, shape, "BGR"), window.get_size())
                frames.append(img)
                counter += 1
        video.release()
        return frames

def getAllGames(dir="Games"):
        items = listdir(dir)
        games = []

        for item in items:
                if path.exists(f"{getcwd()}/{dir}/{item}/.gameconf"):
                        print(f"adding {item}")
                        games.append(item)

        return games

def runGame(game):
        gamesGroup.empty()
        chdir(f"{getcwd()}/{game.wd}")
        pygame.mixer.music.stop()
        subprocess.run(["python3.10", game.conf_data["gamefile"]])
        chdir(rootpath)
        main()

def preview_game(game):
        tmp_font = pygame.font.Font("fonts/dizzyfence.ttf", 20)
        msg = Text("Loading.... Please Wait ....", tmp_font, (0, 0, 0), screen.get_rect().center)

        screen.fill((255, 255, 255))
        screen.blit(msg.image, msg.rect)
        pygame.display.update()

        slides = readFrames(game.video)
        index = 0

        pygame.mixer.music.load(game.video_bg)
        pygame.mixer.music.play()

        running = True

        while running:
                for event in pygame.event.get():
                        if event.type == pygame.QUIT:
                                pygame.quit()
                                exit()
                        if event.type == pygame.KEYDOWN:
                                if event.key == K_ESCAPE:
                                        running = False
                                        break
                if index >= len(slides):
                        index = 0
                        running = False
                screen.blit(slides[index], (0, 0))
                index += 1

                pygame.display.update()
                clock.tick(30)

        pygame.mixer.music.load("songs/bg.ogg")
        pygame.mixer.music.play(-1)

def options_screen(game):
        text_font = pygame.font.Font("fonts/pixelart.ttf", 20)
        bg = pygame.image.load("misc/option_screen.png")

        cursor = Text(">", text_font, (255, 255, 255), (0, 0))
        preview = Text("Preview", text_font, (255, 255, 255), (screen.get_width()//2 - (len("preview")*10), screen.get_height()//2))
        play = Text("Play", text_font, (255, 255, 255), (screen.get_width()//2 + (len("play")*10), screen.get_height()//2))

        options = [preview, play]

        index = 0

        while True:
                for event in pygame.event.get():
                        if event.type == pygame.QUIT:
                                pygame.quit()
                                exit()
                        if event.type == pygame.KEYDOWN:
                                if event.key == K_ESCAPE:
                                        return
                                if event.key == K_RIGHT:
                                        if index < len(options) - 1:
                                                index += 1
                                        else:
                                                index = 0
                                        break
                                if event.key == K_LEFT:
                                        if index > 0:
                                                index -= 1
                                        else:
                                                index = len(options) - 1
                                        break
                                if event.key == K_RETURN:
                                        if options[index] == preview:
                                                preview_game(game)
                                                return
                                        if options[index] == play:
                                                runGame(game)
                                                return

                cursor.rect.midright = options[index].rect.midleft

                screen.blit(bg, ((screen.get_width()-bg.get_width())//2, (screen.get_height()-bg.get_height())//2))
                screen.blit(cursor.image, cursor.rect)
                for item in options:
                        screen.blit(item.image, item.rect)

                pygame.display.update()
                clock.tick(FPS)

def main():

        dataWin = pygame.Surface((res[0], res[1]//2))
        dataWin.fill((0, 0, 0))
        dataWin.set_alpha(200)

        dataRect = dataWin.get_rect()
        dataRect.midbottom = screen.get_rect().midbottom

        prevWin = pygame.Surface((dataWin.get_width()//2-100, dataWin.get_height() - 50))
        prevWin.fill((255, 255, 255))

        prevRect = prevWin.get_rect()
        prevRect.topleft = (50, 25)

        dataWin.blit(prevWin, prevRect)

        head = pygame.Surface((res[0], 50))
        head.fill((0, 0, 0))
        head.set_alpha(200)

        scanlines = pygame.transform.scale(pygame.image.load("backgrounds/scanlines.png"), prevWin.get_size())
        scanlines.set_alpha(100)

        games = getAllGames()

        loading_font = pygame.font.Font("fonts/dizzyfence.ttf", 75)
        title_font = pygame.font.Font("fonts/ka1.ttf", 40)
        norm_font = pygame.font.Font("fonts/pixelart.ttf", 10)

        screen.fill((255, 255, 255))
        loading = Text("Loading....", loading_font, (0, 0, 0), screen.get_rect().center)

        screen.blit(loading.image, loading.rect)
        pygame.display.update()

        i = 0
        for game in games:
                gameData = Game(f"Games/{game}", window=prevWin)
                gameImg = GameWindow(gameData)

                gameImg.rect.midtop = (screen.get_width()//2 + i*(gameImg.image.get_width() + 20), head.get_height() + 15)
                gameImg.add(gamesGroup)
                i += 1

        index = 0

        pygame.mixer.music.load("songs/bg.ogg")
        pygame.mixer.music.play(-1)

        framesCount = 0

        description = []
        curr_game = gamesGroup.sprites()[index].game

        with open(f"{getcwd()}/{curr_game.wd}/{curr_game.conf_data['description-file']}") as f:
                text = f.readlines()
                f.close()

        i = 0
        for line in text:
                line = line.strip(filter)
                obj = Text(line, norm_font, (255, 255, 255))
                description.append(obj)
                if i == 0:
                        obj.rect.topleft = (dataWin.get_width() // 2, 20)
                else:
                        obj.rect.topleft = description[i - 1].rect.bottomleft
                i += 1

        counter = 0

        while True:
                for event in pygame.event.get():
                        if event.type == QUIT:
                                pygame.quit()
                                quit()
                        if event.type == pygame.KEYDOWN:
                                if event.key == pygame.K_RETURN:
                                        options_screen(curr_game)
                                if len(games) > 0:
                                        if event.key == pygame.K_RIGHT:
                                                if index < len(games) - 1:
                                                        index += 1
                                                        for sprite in gamesGroup.sprites():
                                                                sprite.rect.centerx -= gameImg.image.get_width() + 20
                                                else:
                                                        index = 0
                                                        for sprite in gamesGroup.sprites():
                                                                sprite.rect.centerx += (gameImg.image.get_width() + 20) * (len(gamesGroup.sprites()) - 1)


                                        if event.key == pygame.K_LEFT:
                                                if index > 0:
                                                        index -= 1
                                                        for sprite in gamesGroup.sprites():
                                                                sprite.rect.centerx += gameImg.image.get_width() + 20
                                                elif index < 0:
                                                        index = len(games) - 1
                                                        for sprite in gamesGroup.sprites():
                                                                sprite.rect.centerx -= gameImg.image.get_width() + 20 * (len(gamesGroup.sprites()) - 1)

                                description = []
                                curr_game = gamesGroup.sprites()[index].game

                                with open(f"{getcwd()}/{curr_game.wd}/{curr_game.conf_data['description-file']}") as f:
                                        text = f.readlines()
                                        f.close()

                                i = 0
                                for line in text:
                                        line = line.strip(filter)
                                        obj = Text(line, norm_font, (255, 255, 255))
                                        description.append(obj)
                                        if i == 0:
                                                obj.rect.topleft = (dataWin.get_width() // 2, 20)
                                        else:
                                                obj.rect.topleft = description[i - 1].rect.bottomleft
                                        i += 1

                '''
                if framesCount >= len(curr_game.video) - 1:
                        framesCount = 0
                else:
                        framesCount += 1
                '''

                title = Text(curr_game.conf_data["name"], title_font, (255, 255, 255), (screen.get_width()//2, 20))

                screen.fill((0, 0, 255))
                dataWin.fill((0, 0, 0))

                if len(description) > 0:
                        for obj in description:
                                if description[-1].rect.centery > dataRect.centery//4:
                                        if counter % 200 == 0:
                                                obj.rect.centery -= 0.001

                                dataWin.blit(obj.image, obj.rect)

                #prevWin.blit(curr_game.video[framesCount], (0, 0))
                prevWin.blit(scanlines, (0, 0))
                dataWin.blit(prevWin, prevRect)

                head.fill((0, 0, 0))
                head.blit(title.image, title.rect)

                screen.blit(dataWin, dataRect)
                screen.blit(head, (0, 0))

                gamesGroup.draw(screen)

                pygame.display.update()
                clock.tick(FPS)
                counter += 1

main()
