/usr/bin/time -f %e commande

Verrou pôur les accès à la pile

boucle 
	depile <-- Si pile pas vide dépile sinon bloque
	decoupebloc
	empile

rapide_th :

while(1) {
	| lock
	| while pile vide et nb_theard_work > 0
		cond_wait
	si pile vide : fin de l'algo
		broadcast
		return
	depile
	unlock
	decoupe
	verouille
	empile
	deverouille
}