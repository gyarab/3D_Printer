import pygame
import math as m
import time

obrazovkax = 600
obrazovkay = 600
DISPLAYSURF = pygame.display.set_mode((obrazovkax, obrazovkay), 0, 32)

def show(seznam):

    white = (255, 255, 255)
    green = (0, 255, 0)
    blue = (50, 10, 255)
    black = (0, 0, 0)
    barva = black
    tloustka = 2
    pygame.display.set_caption("bagr")
    DISPLAYSURF.fill(white)
    pygame.display.update()
    pygame.init()

    currentColor = black
    lastXY = [seznam[0][0], obrazovkay - seznam[0][1]]
    for x in seznam:
        transformed = [x[0], obrazovkay - x[1]]
        pygame.draw.line(DISPLAYSURF, currentColor, lastXY, transformed, 5)
        pygame.draw.rect(DISPLAYSURF, blue, pygame.Rect( transformed[0],  transformed[1], 10, 10))
        lastXY = transformed

    pygame.display.update()

def otoc(XY, uhel):
    return [XY[0]*m.cos(m.radians(uhel)) - XY[1] * m.sin(m.radians(uhel)),
            XY[0]*m.sin(m.radians(uhel)) + XY[1] * m.cos(m.radians(uhel))]

def posunuti(XY, posun):
    return [XY[0] + posun[0], XY[1] + posun[1]]

def getArc(segments = 8):
    vraceni = []
    first = [200, 200]
    second = [200, 400]
    center = [200, 300]

    posun = [first[0], first[1]]
    minusPosun = [-first[0], -first[1]]
    if center[0] == first[0]:
        if center[1] > first[1]:
            beta = 90
        else:
            beta = -90
    elif center[0] < first[0]:
        beta = 180 + m.degrees(m.atan(float(center[1] - first[1]) / float(center[0] - first[0])))
    else:
        beta = m.degrees(m.atan(float(center[1] - first[1]) / float(center[0] - first[0])))
    vraceni.append(first)
    first = [0, 0]
    second = posunuti(second, minusPosun)
    center = posunuti(center, minusPosun)
    second = otoc(second, -beta)
    center = otoc(center, -beta)

    alpha = 180.0/segments
    r = center[0]
    i = 1
    x = 0
    while True:
        x = r*(1 - m.cos(m.radians(alpha*i)))
        if x >= second[0]:
            break
        y = ( x * (2 * r - x))**(0.5)

        XY = [x, y]

        vraceni.append(posunuti(otoc(XY, beta), posun))
        i += 1
    vraceni.append(posunuti(otoc(second, beta), posun))
    return vraceni

def getBackArc(segments = 8):
    vraceni = []
    first = [200, 200]
    second = [200, 400]
    center = [200, 300]

    posun = [first[0], first[1]]
    minusPosun = [-first[0], -first[1]]
    if center[0] == first[0]:
        if center[1] > first[1]:
            beta = 90
        else:
            beta = -90
    elif center[0] < first[0]:
        beta = 180 + m.degrees(m.atan(float(center[1] - first[1]) / float(center[0] - first[0])))
    else:
        beta = m.degrees(m.atan(float(center[1] - first[1]) / float(center[0] - first[0])))
    print beta
    vraceni.append(first)
    first = [0, 0]
    second = posunuti(second, minusPosun)
    center = posunuti(center, minusPosun)
    second = otoc(second, -beta)
    center = otoc(center, -beta)

    alpha = 180.0/segments
    r = center[0]
    i = 1
    x = 0
    while True:
        x = r*(1 - m.cos(m.radians(alpha*i)))
        if x >= second[0]:
            break
        y = -( x * (2 * r - x))**(0.5)

        XY = [x, y]

        vraceni.append(posunuti(otoc(XY, beta), posun))
        i += 1
    vraceni.append(posunuti(otoc(second, beta), posun))
    return vraceni

while True:
    for x in range(1, 15):
        show(getArc(x))
        time.sleep(0.5)








