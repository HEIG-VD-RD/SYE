### Rapport lab09 
#### Rafael Dousse

## Questions b)
Si on lance `time_loop &`, le processus va entrer dans l'état prêt mais ne passe pas à l'exécution car sa priorité n'est pas plus élevée que celle du shell. Ensuite, on peut lancer une commande comme "ls" et le compteur démarre ses 30 itérations avant la sortie de "ls". Donc le processus "time_loop" reste bloqué car il n'a pas la priorité nécessaire. Cependant, au moment ou une nouvelle commande avec une priorité par défaut similaire est lancée, "time_loop" reprend la main, achève son exécution avant de laisser "ls" s'exécuter.

```
so3% time_loop &
[2]
so3% ls
[2] 0
[2] 1
[2] 2
[2] 3
[2] 4
[2] 5
[2] 6
[2] 7
[2] 8
[2] 9
[2] 10
[2] 11
[2] 12
[2] 13
[2] 14
[2] 15
[2] 16
[2] 17
[2] 18
[2] 19
[2] 20
[2] 21
[2] 22
[2] 23
[2] 24
[2] 25
[2] 26
[2] 27
[2] 28
[2] 29
dev/
reds.bin
cat.elf
echo.elf
ls.elf
sh.elf
time_loop.elf
[2] Terminated
so3%
 ```

## Questions c)
Ici on a changé la  priorité du shell avec une priorité moins importante que le time_loop. Le processus s'éxecute donc directement car il est prioritaire par rapport au shell.

```
so3% renice 1 1
so3% time_loop &
[2] 0
[2] 1
[2] 2
[2] 3
[2] 4
[2] 5
[2] 6
[2] 7
[2] 8
[2] 9
[2] 10
^C
[2] Terminated
[2]
so3% las
^C
las: exec failed.
[3] Terminated
```


## Questions d)

#### - Sans changer la priorité du shell

On observe que le processus démarre, mais le shell reprend rapidement le contrôle. Chaque fois qu'une commande est saisie à nouveau, "time_loop" reprend brièvement le contrôle pour afficher le numéro d'itération. Cependant, à cause de la pause induite par "usleep", le shell reprend à nouveau le contrôle et le `time_loop` est mit en attente contrairement. C'est différent que si on utilise pas le `b` car sans le b on a une boucle while est dans ce cas la le shell ne pourrait pas reprendre le main car le while est plus réellement bloquant. 
 
```
so3% time_loop b &
[2]
so3% ls
dev/
reds.bin
cat.elf
echo.elf
ls.elf
sh.elf
time_loop.elf
so3% ls
[2] 0
dev/
reds.bin
cat.elf
echo.elf
ls.elf
sh.elf
time_loop.elf
so3% 

```

#### - En changeant la priorité du shell

Ici, on change la priorité du shell avec une priorité inférieure a nouveau et on lance le `time_loop` avec le `b`. On remarque que le teime_loop démarre directement par contre si on lance une commande comme `ls` le le nouveau processus prend la main après une nouvelle intervention du time_loop car il a une priorité égal et le `time_loop` est mit en attente avant de continuer. On remarque aussi qu'à la fin du time_loop, le shell ne reprend pas directement la main. 

```
so3% time_loop b &
[2]
so3% renice 1 1
so3% [2] 1
[2] 2
[2] 3
[2] 4
[2] 5
ls
[2] 6
dev/
reds.bin
cat.elf
echo.elf
ls.elf
sh.elf
time_loop.elf
[5] Terminated
so3% [2] 7
[2] 8
[2] 9
[2] 10
[2] 11
[2] 12
[2] 13
[2] 14
[2] 15
[2] 16
[2] 17
[2] 18
[2] 19
[2] 20
[2] 21
[2] 22
[2] 23
[2] 24
[2] 25
[2] 26
[2] 27
[2] 28
[2] 29
[2] Terminated
```