Window W H FL FS
W - width int
H - height int
FL - Frame limit int
FS - fullscreen mode int (1 or 0)

Font F S R G B
F - font file string
S - fontsize int
RGB - int

Player SR CR S FR FG FB OR OG OB OT V
SR - Shape radius int
CR - Collision radius int
S - speed float
FR, FG, FB - fill color int
ORGB - outline color int
OT - outline thickness int
V - shape vertices int (default 8)

Enemy SR, CR, SMIN, SMAX, OR, OG, OB, OT, VMIN, VMAX, L, SI
SR - Shape radius int
CR - Collision radius int
SMIN,SMAX - min/max speed float
ORGB - outline color int
OT - outline thickness int
VMIN, VMAX - min/max vertices int
L - small lifespan int
SI - spawn interval int
(enemies have random fill colours)

Bullet SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L
SR - Shape radius int
CR - Collision radius int
FR, FG, FB - fill color int
ORGB - outline color int
OT - outline thickness int
V - shape vertices int
S - speed float
L - lifespan (90 = 1.5s, 30 = 0.5s)






