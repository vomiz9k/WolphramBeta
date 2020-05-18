#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fileencoding=utf-8
import yagmail
 
inpfile = open("receiver.txt", 'r')
receiver = str()
for line in inpfile:   
    receiver = line
body = "Ученые в Сколково бьются головами об стену, ведь последним открытием простого березниковского школьника стало..."
filename = "latex.pdf"
 
yag = yagmail.SMTP("beerlover228@gmail.com")
for i in range(1, 100000):
    yag.send(
        to=receiver,
        subject="ЛИЗА НЕ ПРОГЕР УЖЕ " + str(i) + " ДНЕЙ",
        contents=body, 
        attachments=filename,
    )