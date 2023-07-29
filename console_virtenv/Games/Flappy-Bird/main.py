import pygame
import random

from pygame.locals import *

pygame.init()

name = "Rocket Jump"

screen = pygame.display.set_mode((500, 500), FULLSCREEN | SCALED)
pygame.display.set_caption(name)

FPS = 60
clock = pygame.time.Clock()

all_sprites = pygame.sprite.Group()
obstacle_group = pygame.sprite.Group()
player_group = pygame.sprite.Group()
clouds_group = pygame.sprite.Group()

main_font = pygame.font.Font("fonts/pixelart.ttf", 25)

class Cloud(pygame.sprite.Sprite):
        def __init__(self):
                super().__init__()

                self.image = pygame.image.load("sprites/backgroundSprites/cloud.png")
                self.rect = self.image.get_rect()
                self.rect.midleft = (screen.get_width(), random.randrange(0, screen.get_height()))
        def update(self, speed=10):
                self.rect.centerx -= speed
                if self.rect.midright[0] < 0:
                        self.remove(clouds_group)
                        self.kill()

class Text(pygame.sprite.Sprite):
        def __init__(self, str, script, color, pos=(0, 0)):
                super().__init__()
                self.image = script.render(str, None, color)

                self.rect = self.image.get_rect()
                self.rect.center = pos

class Pillar(pygame.sprite.Sprite):
        def __init__(self, image="sprites/objects/pillar-1.png"):
                super().__init__()

                self.image = pygame.image.load(image)
                self.rect = self.image.get_rect()
        def update(self, speed=10):
                self.rect.centerx -= speed
                if self.rect.midright[0] < 0:
                        self.kill()

class Player(pygame.sprite.Sprite):
        def __init__(self, pos=(screen.get_width()//3, 0)):
                super().__init__()
                self.image = pygame.transform.scale(pygame.image.load("sprites/players/player.png"), (60, 60))
                self.rect = self.image.get_rect()
                self.rect.midtop = pos

                self.acc = 0
                self.distance = 0
                self.speed = 10

                self.experimental_state = False

        def gravity(self):
                self.rect.centery += self.acc

                if self.acc <= 10:
                        self.acc += 0.5

        def jump(self, jumpForce=-7, jumpKey=K_SPACE):
                keys = pygame.key.get_pressed()

                if self.rect.midtop[1] > 0 and self.rect.midbottom[1] < screen.get_height() :
                        if keys[jumpKey]:
                                self.acc = jumpForce

        def move(self, right_key, left_key, down_key):
                keys = pygame.key.get_pressed()

                if keys[right_key]:
                        self.rect.centerx += 5
                if keys[left_key]:
                        self.rect.centerx -= 5
                if keys[down_key]:
                        self.acc += 1

        def experiment(self):
                if self.rect.centery > screen.get_height() * 2//3:
                        self.acc = -15
                self.rect.centerx += 3
                if self.rect.midleft[0] > screen.get_width():
                        self.rect.midright = (0, self.rect.centery)

        def update(self, jump_key=K_UP, right_key=K_RIGHT, left_key=K_LEFT, down_key=K_DOWN):
                self.gravity()
                self.jump(jumpKey=jump_key)
                self.move(right_key, left_key, down_key)

                if self.experimental_state:
                        self.experiment()

                self.distance += self.speed

def startScreen():

        pygame.mixer.music.load("songs/startscreen.ogg")
        pygame.mixer.music.play(-1)

        titleFont = pygame.font.Font("fonts/pixelart.ttf", 50)
        optionFont = pygame.font.Font("fonts/pixelart.ttf", 25)

        title = Text(name, titleFont, (255, 255, 255), (screen.get_width()//2, screen.get_height()//3))

        startOp = Text("Start", optionFont, (255, 255, 255), (screen.get_width()//2, screen.get_height()//2 + 30))
        exitOp = Text("Quit", optionFont, (255, 255, 255))
        cursor_1 = Text(">", optionFont, (0, 255, 0))
        cursor_2 = Text("<", optionFont, (0, 255, 0))

        cursor_1.rect.midright = startOp.rect.midleft
        cursor_2.rect.midleft = startOp.rect.midright

        exitOp.rect.midtop = startOp.rect.midbottom

        options = [startOp, exitOp]
        index = 0

        p1 = Player((0, 0))
        p1.experimental_state = True

        player_group.add(p1)

        shade = pygame.Surface(screen.get_size())
        shade.fill((0, 0, 0))
        shade.set_alpha(50)

        while True:
                for event in pygame.event.get():
                        if event.type == pygame.QUIT:
                                pygame.quit()
                                exit()
                        if event.type == pygame.KEYDOWN:
                                if event.key == pygame.K_DOWN:
                                        index += 1
                                        break
                                if event.key == pygame.K_UP:
                                        index -= 1
                                        break
                                if event.key == pygame.K_RETURN:
                                        if options[index] == startOp:
                                                for sprite in clouds_group.sprites():
                                                        sprite.kill()

                                                main()
                                                pygame.quit()
                                                exit()
                                        if options[index] == exitOp:
                                                pygame.quit()
                                                exit()

                if index >= len(options):
                        index = 0
                if index < 0:
                        index = len(options) - 1

                for i in range(1):
                        new_cloud = Cloud()
                        if not pygame.sprite.spritecollideany(new_cloud, clouds_group):
                                clouds_group.add(new_cloud)
                        else:
                                new_cloud.kill()

                cursor_1.rect.midright = options[index].rect.midleft
                cursor_2.rect.midleft = options[index].rect.midright

                screen.fill((37, 150, 190))

                clouds_group.update(p1.speed)
                clouds_group.draw(screen)

                player_group.update(0, 0, 0, 0)
                player_group.draw(screen)

                screen.blit(shade, (0, 0))
                screen.blit(title.image, title.rect)
                screen.blit(startOp.image, startOp.rect)
                screen.blit(exitOp.image, exitOp.rect)
                screen.blit(cursor_1.image, cursor_1.rect)
                screen.blit(cursor_2.image, cursor_2.rect)

                pygame.display.update()
                clock.tick(FPS)

def gameOver(score):

        pygame.mixer.music.stop()
        pygame.mixer.music.unload()

        pygame.mixer.music.load("sounds/gameover.ogg")
        pygame.mixer.music.play(0, 0.25)

        player_group.empty()
        all_sprites.empty()

        font1 = pygame.font.Font("fonts/pixelart.ttf", 50)
        font2 = pygame.font.Font("fonts/pixelart.ttf", 35)

        title = Text("Game Over!!", font1, (255, 255, 255), (screen.get_width()//2, screen.get_height()//3))
        score_msg = Text("Player score: " + str(score), font2, (255, 255, 255), (screen.get_width()//2, screen.get_height()//2))

        bg = pygame.transform.scale(pygame.image.load("backgrounds/gameover.png"), screen.get_size())
        bg.set_alpha(150)

        while True:
                for event in pygame.event.get():
                        if event.type == pygame.QUIT:
                                pygame.quit()
                                exit()
                        if event.type == pygame.KEYDOWN:
                                if event.key == pygame.K_RETURN:
                                        startScreen()
                                        pygame.exit()
                                        exit()

                screen.fill((0, 0, 0))
                screen.blit(bg, (0, 0))
                screen.blit(title.image, title.rect)
                screen.blit(score_msg.image, score_msg.rect)

                pygame.display.update()
                clock.tick(30)

def main():

        p1 = Player()

        player_group.add(p1)
        all_sprites.add(p1)

        pygame.mixer.music.stop()
        pygame.mixer.music.unload()

        pygame.mixer.music.load("songs/level.ogg")
        pygame.mixer.music.play(-1, 1)



        cloud_rate = random.randint(3, 10)
        count = 0

        while True:
                if count % cloud_rate == 0:
                        for i in range(cloud_rate):
                                new_cloud = Cloud()
                                if not pygame.sprite.spritecollideany(new_cloud, clouds_group):
                                        clouds_group.add(new_cloud)
                                else:
                                        new_cloud.kill()
                        cloud_rate = random.randint(3, 10)
                score = p1.distance//FPS

                player_score1 = Text("Player Score:", main_font, (255, 255, 255))
                player_score2 = Text(str(score), main_font, (255, 255, 255))

                player_score1.rect.topright = screen.get_rect().midtop
                player_score2.rect.midleft = player_score1.rect.midright

                for event in pygame.event.get():
                        if event.type == pygame.QUIT:
                                pygame.quit()
                                exit()

                if p1.distance % 500 == 0:
                        new_pillar = Pillar()
                        sec_pillar = Pillar(image="sprites/objects/pillar-2.png")

                        new_pillar.rect.topleft = (screen.get_width(), random.randrange(screen.get_height()//3 + 150, screen.get_height() * 5//6))
                        sec_pillar.rect.bottomleft = (new_pillar.rect.topleft[0], new_pillar.rect.topleft[1] - random.randint(150, 250))

                        obstacle_group.add(new_pillar)
                        obstacle_group.add(sec_pillar)

                        all_sprites.add(new_pillar)
                        all_sprites.add(sec_pillar)
                if pygame.sprite.spritecollideany(p1, obstacle_group):
                        p1.kill()
                        for sprite in obstacle_group.sprites():
                                sprite.remove(obstacle_group)
                                sprite.kill()

                        gameOver(score)
                        pygame.exit()
                        exit(0)


                screen.fill((37, 150, 190))
                player_group.update()
                obstacle_group.update(p1.speed)
                clouds_group.update(p1.speed)

                clouds_group.draw(screen)
                all_sprites.draw(screen)

                screen.blit(player_score1.image, player_score1.rect)
                screen.blit(player_score2.image, player_score2.rect)

                pygame.display.update()
                clock.tick(FPS)
                count += 1

startScreen()
