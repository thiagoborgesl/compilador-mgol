
inicio 
	varinicio
		literal A;
		inteiro B;
		inteiro D;
		real C ;
	varfim;
	leia B;
	leia A;
	se(B>2)
	entao
		se(B<=4)
		entao
			escreva "B esta entre 2 e 4";
		fimse
	fimse
	B<-B+1;
	B<-B+2;
	B<-B+3;
	D<-B;
	C<-5.0;
	repita (B<5)
		C<-B+2;
		escreva C;
		B<-B+1;
	fimrepita
	escreva D;
	escreva C;
	escreva A;
fim
